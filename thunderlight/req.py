from typing import Any, AsyncGenerator
from multipart.multipart import parse_options_header
from .formparsers import FormData, FormParser, MultiPartParser
from .asgi import Scope, Receive
from .json import JSON


class Req:
    def __init__(self,
                 scope: Scope,
                 receive: Receive,
                 args: dict[str, str],
                 path: str,
                 json: JSON) -> None:
        self._scope = scope
        self._receive = receive
        self._args = args
        self._path = path
        self._json_coder = json
        self._stream_consumed = False

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
        if not hasattr(self, '_headers'):
            self._headers = dict([(k.decode('utf8'), v.decode('utf8')) for (k, v) in self._scope['headers']])
        return self._headers

    async def body(self) -> bytes:
        if not hasattr(self, "_body"):
            chunks = []
            async for chunk in self._stream():
                chunks.append(chunk)
            self._body = b"".join(chunks)
        return self._body

    async def json(self) -> Any | None:
        if not hasattr(self, "_json"):
            body = await self.body()
            self._json = self._json_coder.decode(body)
        return self._json

    async def form(self) -> FormData | None:
        if not hasattr(self, "_form"):
            content_type_header = self.headers.get("content-type")
            content_type, _ = parse_options_header(content_type_header)
            if content_type == b"multipart/form-data":
                multipart_parser = MultiPartParser(self.headers, self._stream())
                self._form = await multipart_parser.parse()
            elif content_type == b"application/x-www-form-urlencoded":
                form_parser = FormParser(self.headers, self._stream())
                self._form = await form_parser.parse()
            else:
                self._form = FormData()
        return self._form

    async def dict(self) -> dict[str, Any]:
        if self.headers.get('content-type').startswith('multipart/form-data'):
            return await self.form()
        else:
            return await self.json()

    async def _stream(self) -> AsyncGenerator[bytes, None]:
        if hasattr(self, "_body"):
            yield self._body
            yield b""
            return

        if self._stream_consumed:
            raise RuntimeError("Stream consumed")
        self._stream_consumed = True

        while True:
            message = await self._receive()
            if message["type"] == "http.request":
                body = message.get("body", b"")
                if body:
                    yield body
                if not message.get("more_body", False):
                    break
        yield b""
