from typing import Callable, Awaitable
from .ctx import Ctx


Next = Callable[[Ctx], Awaitable[None]]
