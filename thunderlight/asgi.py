from typing import MutableMapping, Any, Awaitable, Callable


Scope = MutableMapping[str, Any]


Message = MutableMapping[str, Any]


Receive = Callable[[], Awaitable[Message]]


Send = Callable[[Message], Awaitable[None]]


ASGIApp = Callable[[Scope, Receive, Send], Awaitable[None]]
