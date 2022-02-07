from typing import Callable, Awaitable
from _thunderlight import Ctx


Next = Callable[[Ctx], Awaitable[None]]
Handler = Callable[[Ctx], Awaitable[None]]
Middleware = Callable[[Ctx, Next], Awaitable[None]]
