from typing import Any
from urllib.parse import quote
from .json import JSON
from .asgi import Scope, Receive, Send


class Res:

    def __init__(self, json: JSON) -> None:
        self._code: int = 200
        self._body: bytes = b''
        self._headers: dict[str, str] = {}
        self._json = json

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
    def body(self) -> bytes:
        return self._body

    @body.setter
    def body(self, body: bytes) -> None:
        if type(body) is str:
            body = body.encode('utf-8')
        self._body = body

    def json(self, data: Any) -> None:
        self._headers['content-type'] = "application/json"
        self._body = self._json.encode(data)

    def text(self, text: str) -> None:
        self._headers['content-type'] = 'text/plain'
        self._body = text.encode('utf-8')

    def html(self, html: str) -> None:
        self._headers['content-type'] = 'text/html'
        self._body = html.encode('utf-8')

    def redirect(self, url: str) -> None:
        self._code = 307
        self._headers["location"] = quote(str(url), safe=":/%#?=@[]!$&'()*+,;")


    def empty(self, *args, **kwargs) -> None:
        self.code = 204

    async def __call__(self, scope: Scope, receive: Receive, send: Send) -> None:
        await send(
            {
                "type": "http.response.start",
                "status": self.code,
                "headers": list(self.headers.items()),
            }
        )
        await send({"type": "http.response.body", "body": self.body})
