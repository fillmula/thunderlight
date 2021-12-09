from typing import Callable
from .app import App
from .middleware import Middleware
from .handler import Handler
from .ctx import Ctx
from .next import Next


app = App()


def make() -> App:
    return main(App())


def main(new_app: App) -> App:
    global app
    app = new_app


def use(middleware: Middleware) -> None:
    app.use(middleware)


def get(path: str) -> Callable[[Handler], None]:
    return app.get(path)


def post(path: str) -> Callable[[Handler], None]:
    return app.post(path)


def patch(path: str) -> Callable[[Handler], None]:
    return app.patch(path)


def delete(path: str) -> Callable[[Handler], None]:
    return app.delete(path)


def gimme() -> App:
    return app
