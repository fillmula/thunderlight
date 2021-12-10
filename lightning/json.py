from typing import Any
try:
    import orjson
except:
    orjson = None
try:
    import ujson
except:
    ujson = None
import json


class JSON:

    def __init__(self) -> None:
        try:
            import orjson
            self.encode = self.orjson_encode
            self.decode = self.orjson_decode
        except:
            try:
                import ujson
                self.encode = self.ujson_encode
                self.decode = self.ujson_decode
            except:
                self.encode = self.json_encode
                self.decode = self.json_decode

    def orjson_encode(self, data: Any) -> bytes:
        return orjson.dumps(data)

    def orjson_decode(self, data: bytes) -> Any:
        return orjson.loads(data)

    def ujson_encode(self, data: Any) -> bytes:
        return ujson.dumps(data).encode('utf-8')

    def ujson_decode(self, data: bytes) -> Any:
        return ujson.loads(data)

    def json_encode(self, data: Any) -> bytes:
        return json.dumps(data).encode('utf-8')

    def json_decode(self, data: bytes) -> Any:
        return json.loads(data)
