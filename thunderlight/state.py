from typing import Any

class State(dict):

    def __init__(self) -> None:
        pass

    def __setattr__(self, key: Any, value: Any) -> None:
        self[key] = value

    def __getattr__(self, key: Any) -> Any:
        try:
            return self[key]
        except KeyError:
            message = "'{}' object has no attribute '{}'"
            raise AttributeError(message.format(self.__class__.__name__, key))

    def __delattr__(self, key: Any) -> None:
        del self[key]
