from typing import Any, Callable
from re import sub
from .matcher import Matcher
from .middleware import Middleware
from .handler import Handler
from .next import Next
from .matcher import Matcher
from .ctx import Ctx
from .req import Req
from .res import Res
from .asgi import Receive, Scope, Send
from .json import JSON


class App:

    def __init__(self) -> None:
        """Create a new server application.
        """
        self._middlewares: list[Middleware] = []
        self._gets: list[Matcher] = []
        self._posts: list[Matcher] = []
        self._patches: list[Matcher] = []
        self._deletes: list[Matcher] = []
        self._json = JSON()

    def use(self, middleware: Middleware) -> None:
        self._middlewares.append(middleware)
        self._middleware = self._build_middleware()

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
            case _:
                return ({}, _not_found)
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
        async def asgi_handle(receive: Receive, send: Send):
            path = scope['path']
            path = sub('/$', '', path) if len(path) > 1 else path
            args, handler = self._args_and_handler(scope['method'], path)
            ctx = Ctx(Req(scope, receive, args, path, self._json), Res(self._json))
            await self._middleware(ctx, handler)
            await ctx.res(scope, receive, send)
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
