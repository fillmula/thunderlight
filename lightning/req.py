from typing import Any, Coroutine
from starlette.requests import Request


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
