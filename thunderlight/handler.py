from typing import Callable, Awaitable
from .ctx import Ctx


Handler = Callable[[Ctx], Awaitable[None]]
