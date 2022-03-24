from typing import Any
from urllib.parse import quote
from mimetypes import guess_type
from anyio import open_file
from .json import JSON
from .asgi import Scope, Receive, Send


class Res:

    def __init__(self, json: JSON) -> None:
        self._code: int = 200
        self._body: bytes = b''
        self._headers: dict[str, str] = {}
        self._json = json
        self._file_path: str | None = None

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

    def file(self, path: str) -> None:
        self._headers['content-type'] = guess_type(path)[0] or 'text/plain'
        self._file_path = path


    async def __call__(self, scope: Scope, receive: Receive, send: Send) -> None:
        await send(
            {
                "type": "http.response.start",
                "status": self.code,
                "headers": list(self.headers.items()),
            }
        )
        if self._file_path is not None:
            async with await open_file(self._file_path, mode="rb") as file:
                more_body = True
                while more_body:
                    chunk = await file.read(1024 * 60)
                    more_body = len(chunk) == 1024 * 60
                    await send(
                        {
                            "type": "http.response.body",
                            "body": chunk,
                            "more_body": more_body,
                        }
                    )
        else:
            await send({
                "type": "http.response.body",
                "body": self.body,
                "more_body": False
            })
