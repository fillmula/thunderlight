from typing import Any, Awaitable
from .json import JSON
from .app import App


class Result:

    def __init__(self, code: int, headers: dict[str, str], body: bytes) -> None:
        self._code = code
        self._headers = headers
        self._body = body
        self._json_coder = JSON()

    @property
    def code(self) -> int:
        return self._code

    @property
    def headers(self) -> dict[str, str]:
        return self._headers

    @property
    def body(self) -> bytes:
        return self._body

    @property
    def json(self) -> Any:
        if not hasattr(self, '_json'):
            self._json = self._json_coder.decode(self._body)
        return self._json

    @property
    def is_empty(self) -> bool:
        return self._body == b''


class Client:

    def __init__(self, app: App) -> None:
        self._app = app

    async def get(self, url: str, headers: dict[str, str] | None = None, body: Any = None) -> Awaitable[Result]:
        return await self.request('GET', url, headers, body)

    async def post(self, url: str, headers: dict[str, str] | None = None, body: Any = None) -> Awaitable[Result]:
        return await self.request('POST', url, headers, body)

    async def patch(self, url: str, headers: dict[str, str] | None = None, body: Any = None) -> Awaitable[Result]:
        return await self.request('PATCH', url, headers, body)

    async def delete(self, url: str, headers: dict[str, str] | None = None, body: Any = None) -> Awaitable[Result]:
        return await self.request('DELETE', url, headers, body)

    async def request(self, method: str, url: str, headers: dict[str, str] | None = None, body: Any = None) -> Awaitable[Result]:
        pass
