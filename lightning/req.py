from typing import Any, Coroutine
from .asgi import Scope, Receive


class Req:
    def __init__(self,
                 scope: Scope,
                 receive: Receive,
                 args: dict[str, str],
                 path: str) -> None:
        self._scope = scope
        self._receive = receive
        self._args = args
        self._path = path

    @property
    def client(self) -> tuple[str, int]:
        return self._scope['client']

    @property
    def scheme(self) -> str:
        return self._scope['scheme']

    @property
    def version(self) -> str:
        return self._scope['http_version']

    @property
    def method(self) -> str:
        return self._scope['method']

    @property
    def path(self) -> str:
        return self._path

    @property
    def args(self) -> dict[str, str]:
        return self._args

    @property
    def qs(self) -> str:
        return self._scope['query_string'].decode('utf8')

    @property
    def headers(self) -> dict[str, str]:
        if hasattr(self, '_headers'):
            return self._headers
        self._headers = dict([(k.decode('utf8'), v.decode('utf8')) for (k, v) in self._scope['headers']])
        return self._headers

    @property
    def body(self) -> Coroutine[Any, Any, bytes]:
        return self._request.body
