from typing import Callable
from .app import App
from .middleware import Middleware
from .handler import Handler
from .ctx import Ctx


app = App()
"""The default app object. Global methods are applied to this object. This
makes our API consice.
"""


def make() -> App:
    """Create a new instance of App and assign it to the default app which
    makes using global decorators to install handlers and middlewares to this
    app possible.
    """
    return main(App())


def apply(middleware: Middleware) -> Callable[[Handler], Handler]:
    """Apply a middleware to a request handler. The decorated return value is
    a handler with a middleware applied.
    """
    def applier(handler: Handler) -> Handler:
        async def new_handler(ctx: Ctx) -> None:
            await middleware(ctx, handler)
        return new_handler
    return applier


def main(new_app: App) -> App:
    """Assign an app instance to the default app. The default app can have
    handlers and middlewares installed by using global decorators.
    """
    global app
    app = new_app


def use(middleware: Middleware) -> None:
    """Install a middleware to the default app. The later installed goes in the
    request handle stack later and goes out earlier.
    """
    app.use(middleware)


def get(path: str) -> Callable[[Handler], None]:
    """Install a get request handler to the default app.
    """
    return app.get(path)


def post(path: str) -> Callable[[Handler], None]:
    """Install a post request handler to the default app.
    """
    return app.post(path)


def patch(path: str) -> Callable[[Handler], None]:
    """Install a patch request handler to the default app.
    """
    return app.patch(path)


def delete(path: str) -> Callable[[Handler], None]:
    """Install a delete request handler to the default app.
    """
    return app.delete(path)


def gimme() -> App:
    """Get a reference to the default app that all global methods point to.
    This is useful when launching a server.
    """
    return app
