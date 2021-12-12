from asyncio import set_event_loop, Protocol
from uvloop import new_event_loop
from .app import App
from .protocol import Protocol

class Server:

    def __init__(self, app: App, port: int) -> None:
        self._loop = new_event_loop()
        self._app = app
        self._port = port

    def listen(self) -> None:
        set_event_loop(self._loop)
        self._loop.create_server()
