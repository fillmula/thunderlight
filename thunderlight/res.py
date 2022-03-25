from typing import Any
from os import stat, stat_result
from stat import S_ISREG
from hashlib import md5
from urllib.parse import quote
from email.utils import formatdate
from mimetypes import guess_type
from anyio import open_file, to_thread
from .json import JSON
from .asgi import Scope, Receive, Send


class Res:

    def __init__(self, json: JSON) -> None:
        self._code: int = 200
        self._body: bytes = b''
        self._headers: dict[str, str] = {}
        self._json = json
        self._file_path: str | None = None
        self._file_not_found: bool | None = None

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

    def set_stat_headers(self, sresult: stat_result):
        content_length = str(sresult.st_size)
        last_modified = formatdate(sresult.st_mtime, usegmt=True)
        etag_base = str(sresult.st_mtime) + "-" + str(sresult.st_size)
        etag = md5(etag_base.encode(), usedforsecurity=False).hexdigest()
        self.headers.setdefault("content-length", content_length)
        self.headers.setdefault("last-modified", last_modified)
        self.headers.setdefault("etag", etag)

    async def _add_common_headers(self) -> None:
        if self._file_path is not None:
            try:
                sresult = await to_thread.run_sync(stat, self._file_path)
                self.set_stat_headers(sresult)
            except FileNotFoundError:
                self._file_not_found = True
                self.headers['content-type'] = 'application/json'
                self.code = 404
                self.body = '{"error": {"type": "Not Found"}}'
                content_length = str(len(self.body))
                self.headers['content-length'] = content_length
            else:
                mode = sresult.st_mode
                if not S_ISREG(mode):
                    self._file_not_found = True
                    self.headers['content-type'] = 'application/json'
                    self.code = 404
                    self.body = '{"error": {"type": "Not Found"}}'
                    content_length = str(len(self.body))
                    self.headers['content-length'] = content_length
        else:
            content_length = str(len(self.body))
            self.headers['content-length'] = content_length

    async def __call__(self, scope: Scope, receive: Receive, send: Send) -> None:
        await self._add_common_headers()
        await send(
            {
                "type": "http.response.start",
                "status": self.code,
                "headers": list(self.headers.items()),
            }
        )
        if self._file_path is not None:
            if self._file_not_found:
                await send({
                    "type": "http.response.body",
                    "body": self.body,
                    "more_body": False
                })
            else:
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
