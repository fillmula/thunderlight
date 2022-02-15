from .req import Req
from .res import Res
from .state import State


class Ctx:

    def __init__(self, req: Req, res: Res) -> None:
        self._req = req
        self._res = res
        self._state = State()

    @property
    def req(self):
        return self._req

    @property
    def res(self):
        return self._res

    @property
    def state(self):
        return self._state
