from _thunderlight import Ctx, App as _App
from .callables import Middleware, Next


def _apply(outer: Middleware, inner: Middleware) -> Middleware:
    async def middleware(ctx: Ctx, next: Next) -> None:
        async def outer_next(ctx: Ctx) -> None:
            await inner(ctx, next)
        await outer(ctx, outer_next)
    return middleware


def _build_middleware(ms: list[Middleware]) -> Middleware | None:
    match len(ms):
        case 0:
            return None
        case 1:
            return ms[0]
        case _:
            retval = ms[-1]
            for i in range(len(ms) - 2, -1, -1):
                retval = _apply(ms[i], retval)
            return retval


class App(_App):


    def __init__(self) -> None:
        super().__init__()
        self.set_prepare(self.prepare)


    def prepare(self):
        self.entrance_middleware = _build_middleware(self.middlewares)
