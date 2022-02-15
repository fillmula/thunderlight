from typing import Callable, Awaitable
from .ctx import Ctx
from .next import Next


Middleware = Callable[[Ctx, Next], Awaitable[None]]
