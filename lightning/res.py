from starlette.responses import Response


class Res:

    def __init__(self) -> None:
        self._code: int = 200
        self._body: str = ''
        self._headers: dict[str, str] = {}

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
    def body(self) -> str:
        return self._body

    @body.setter
    def body(self, body: str) -> None:
        self._body = body

    def _make_response(self) -> Response:
        return Response(content=self.body,
                        status_code=self.code,
                        headers=self.headers)
