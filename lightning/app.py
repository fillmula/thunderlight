from typing import Any, Callable
from re import sub
from starlette.requests import Request
from .matcher import Matcher
from .middleware import Middleware
from .handler import Handler
from .next import Next
from .matcher import Matcher
from .ctx import Ctx
from .req import Req
from .asgi import ASGIApp, Receive, Scope, Send


class App:

    def __init__(self) -> None:
        """Create a new server application.
        """
        self._middlewares: list[Middleware] = []
        self._gets: list[Matcher] = []
        self._posts: list[Matcher] = []
        self._patches: list[Matcher] = []
        self._deletes: list[Matcher] = []
        self._asgi_app: ASGIApp | None = None

    def use(self, middleware: Middleware) -> None:
        self._middlewares.append(middleware)

    def get(self, path: str) -> Callable[[Handler], None]:
        def wrapper(handler: Handler) -> None:
            self._gets.append(Matcher(path, handler))
        return wrapper

    def post(self, path: str) -> Callable[[Handler], None]:
        def wrapper(handler: Handler) -> None:
            self._posts.append(Matcher(path, handler))
        return wrapper

    def patch(self, path: str) -> Callable[[Handler], None]:
        def wrapper(handler: Handler) -> None:
            self._patches.append(Matcher(path, handler))
        return wrapper

    def delete(self, path: str) -> Callable[[Handler], None]:
        def wrapper(handler: Handler) -> None:
            self._deletes.append(Matcher(path, handler))
        return wrapper

    def _make(self) -> None:
        middleware = self._build_middleware()
        async def app(scope: Scope, receive: Receive, send: Send):
            request = Request(scope, receive, send)
            path = sub('/$', '', request.url.path)
            args, handler = self._args_and_handler(request.method, path)
            ctx = Ctx(Req(request, args, path))
            await middleware(ctx, handler)
            response = ctx.res._make_response()
            await response(scope, receive, send)
        self._asgi_app = app

    def _args_and_handler(self, method: str, path: str) -> tuple[dict[str, str], Handler]:
        match method:
            case 'GET':
                stack = self._gets
            case 'POST':
                stack = self._posts
            case 'PATCH':
                stack = self._patches
            case 'DELETE':
                stack = self._deletes
        for matcher in stack:
            mdata = matcher.match(path)
            if mdata is not None:
                return mdata
        return ({}, _not_found)

    def _build_middleware(self) -> Middleware | None:
        match len(self._middlewares):
            case 0:
                return None
            case 1:
                return self._middlewares[0]
            case _:
                retval = self._middlewares[-1]
                for i in range(len(self._middlewares) - 2, -1, -1):
                    retval = _apply(self._middlewares[i], retval)
                return retval

    def __call__(self, scope: Scope) -> Any:
        print("SCOPE PRINT")
        print(scope)
        if not hasattr(self, '_middleware'):
            self._middleware = self._build_middleware()
        async def asgi_handle(receive: Receive, send: Send):
            request = Request(scope, receive, send)
            path = sub('/$', '', request.url.path)
            args, handler = self._args_and_handler(request.method, path)
            ctx = Ctx(Req(request, args, path))
            await self._middleware(ctx, handler)
            response = ctx.res._make_response()
            await response(scope, receive, send)
        return asgi_handle


def _apply(outer: Middleware, inner: Middleware) -> Middleware:
    async def middleware(ctx: Ctx, next: Next) -> None:
        async def outer_next(ctx: Ctx) -> None:
            await inner(ctx, next)
        await outer(ctx, outer_next)
    return middleware


async def _not_found(ctx: Ctx) -> None:
    ctx.res.code = 404
    ctx.res.body = '{"error": {"type": "Not Found"}}'
