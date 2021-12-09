from re import sub, findall, match
from .handler import Handler


class Matcher:

    def __init__(self, path: str, handler: Handler) -> None:
        self.rule = path
        self.handler = handler
        self.tokens = list(map(lambda t: t[1:], findall(':[^/]+', path)))
        self.regex = '^' + sub('(:[^/]+)', '([^/]+)', path) + '$'

    def match(self, path: str) -> tuple[dict[str, str], Handler] | None:
        mdata = match(self.regex, path)
        if mdata is None:
            return None
        else:
            args: dict[str, str] = {}
            for index, token in enumerate(self.tokens):
                args[token] = mdata[index + 1]
            return (args, self.handler)
