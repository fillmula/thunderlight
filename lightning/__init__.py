from typing import Callable, Awaitable, Coroutine, Any
from re import sub, match, findall
from starlette.requests import Request
from starlette.responses import Response
from starlette.types import ASGIApp, Receive, Scope, Send


class Req:
    def __init__(self, request: Request, args: dict[str, str], path) -> None:
        self._request = request
        self._args = args
        self._path = path

    @property
    def method(self):
        return self._request.method

    @property
    def path(self) -> str:
        return self._path

    @property
    def args(self):
        return self._args

    @property
    def query(self) -> str:
        return self._request.url.query

    @property
    def hash(self) -> str:
        return self._request.url.fragment

    @property
    def body(self) -> Coroutine[Any, Any, bytes]:
        return self._request.body


class Res:

    def __init__(self) -> None:
        self._code: int = 200
        self._body: str = ''
        self._headers: dict[str, str] = {}

    @property
    def code(self) -> int:
        return self._code

    @code.setter
    def code(self, code: int) -> None:
        self._code = code

    @property
    def headers(self) -> dict[str, str]:
        return self._headers

    @headers.setter
    def headers(self, headers: dict[str, str]) -> None:
        self._headers = headers

    @property
    def body(self) -> str:
        return self._body

    @body.setter
    def body(self, body: str) -> None:
        self._body = body

    def _make_response(self) -> Response:
        return Response(content=self.body,
                        status_code=self.code,
                        headers=self.headers)


class State(dict):

    def __init__(self) -> None:
        pass


class Ctx:

    def __init__(self, req: Req) -> None:
        self._req = req
        self._res = Res()
        self._state = State()

    @property
    def req(self):
        return self._req

    @property
    def res(self):
        return self._res

    @property
    def state(self):
        return self._state


Next = Callable[[Ctx], Awaitable[None]]


Middleware = Callable[[Ctx, Next], Awaitable[None]]


Handler = Callable[[Ctx], Awaitable[None]]


MIDDLEWARES: list[Middleware] = []


class PathMatcher:

    def __init__(self, path: str, handler: Handler) -> None:
        self.rule = path
        self.handler = handler
        self.tokens = list(map(lambda t: t[1:], findall(':[^/]+', path)))
        self.regex = '^' + sub('(:[^/]+)', '([^/]+)', path) + '$'

    def match(self, path: str) -> tuple[dict[str, str], Handler] | None:
        mdata = match(self.regex, path)
        if mdata is None:
            return None
        else:
            args: dict[str, str] = {}
            for index, token in enumerate(self.tokens):
                print("HERE RUN")
                args[token] = mdata[index + 1]
            return (args, self.handler)


GETS: list[PathMatcher] = []


POSTS: list[PathMatcher] = []


PATCHES: list[PathMatcher] = []


DELETES: list[PathMatcher] = []


def use(middleware: Middleware):
    MIDDLEWARES.append(middleware)


def get(path: str) -> Callable[[Handler], None]:
    def wrapper(handler: Handler) -> None:
        GETS.append(PathMatcher(path, handler))
    return wrapper


def post(path: str) -> Callable[[Handler], None]:
    def wrapper(handler: Handler) -> None:
        POSTS.append(PathMatcher(path, handler))
    return wrapper


def patch(path: str) -> Callable[[Handler], None]:
    def wrapper(handler: Handler) -> None:
        PATCHES.append(PathMatcher(path, handler))
    return wrapper


def delete(path: str) -> Callable[[Handler], None]:
    def wrapper(handler: Handler) -> None:
        DELETES.append(PathMatcher(path, handler))
    return wrapper


def make() -> ASGIApp:
    middleware = _build_middleware()
    async def app(scope: Scope, receive: Receive, send: Send):
        request = Request(scope, receive, send)
        path = sub('/$', '', request.url.path)
        args, handler = _args_and_handler(request.method, path)
        ctx = Ctx(Req(request, args, path))
        await middleware(ctx, handler)
        response = ctx.res._make_response()
        await response(scope, receive, send)
    return app


def stop() -> None:
    pass


def _args_and_handler(method: str, path: str) -> tuple[dict[str, str], Handler]:
    match method:
        case 'GET':
            stack = GETS
        case 'POST':
            stack = POSTS
        case 'PATCH':
            stack = PATCHES
        case 'DELETE':
            stack = DELETES
    for matcher in stack:
        mdata = matcher.match(path)
        if mdata is not None:
            return mdata
    return ({}, _not_found_next)


def _build_middleware() -> Middleware | None:
    match len(MIDDLEWARES):
        case 0:
            return None
        case 1:
            return MIDDLEWARES[0]
        case _:
            retval = MIDDLEWARES[-1]
            for i in range(len(MIDDLEWARES) - 2, -1, -1):
                retval = _apply(MIDDLEWARES[i], retval)
            return retval


def _apply(outer: Middleware, inner: Middleware) -> Middleware:
    async def middleware(ctx: Ctx, next: Next) -> None:
        async def outer_next(ctx: Ctx) -> None:
            await inner(ctx, next)
        await outer(ctx, outer_next)
    return middleware


async def _not_found_next(ctx: Ctx) -> None:
    ctx.res.code = 404
    ctx.res.body = '{"type": "Not Found"}'
