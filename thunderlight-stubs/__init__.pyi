from typing import Any, Callable, Awaitable


class ReqMatch:
    """The url path match result. This is a dict like object.
    """
    ...


class ReqHeaders:
    """The request headers. Use lowercase string key to fetch values. This is a
    dict like object.
    """
    ...


class Req:

    @property
    def method(self) -> str:
        """The request method.
        """
        ...

    @property
    def version(self) -> str:
        """The HTTP version of the request.
        """
        ...

    @property
    def path(self) -> str:
        """The request path of this request.
        """
        ...

    @property
    def args(self) -> ReqMatch | None:
        """The url path match result.
        """
        ...

    @property
    def query(self) -> str | None:
        """The request's query string.
        """
        ...

    @property
    def headers(self) -> ReqHeaders:
        """The request headers.
        """
        ...

    @property
    def body(self) -> bytes:
        """The request's body.
        """
        ...

    @property
    def json(self) -> Any:
        """The request's body JSON object.
        """
        ...


class ResHeaders:
    """The response headers. This is a dict like object.
    """
    ...


class Res:

    @property
    def code(self) -> int:
        """The response's status code.
        """
        ...

    @code.setter
    def code(self, code) -> None:
        """Set the response's status code.
        """
        ...

    @property
    def headers(self) -> ResHeaders:
        """The response's headers.
        """
        ...

    @property
    def body(self) -> Any:
        """The response's body.
        """
        ...

    @body.setter
    def body(self, body) -> None:
        """Set the response's body. This can be a str or a bytes.
        """
        ...

    def json(self, data: Any) -> None:
        """Set the response's body to the json object and set the content type
        header.
        """
        ...

    def text(self, data: str) -> None:
        """Set the response's body to the data string and set the content type
        header.
        """
        ...

    def html(self, data: str) -> None:
        """Set the response's body to the data string and set the content type
        header.
        """
        ...

    def empty(self, *args: Any, **kwargs: Any) -> None:
        """Set the response's body to empty and set the status code to 204.
        """
        ...


class State:
    """The shared state of this context.
    """
    pass


class Ctx:

    @property
    def req(self) -> Req:
        """The request object of this context.
        """
        ...

    @property
    def res(self) -> Res:
        """The response object of this context.
        """
        ...

    @property
    def state(self) -> State:
        """The shared state of this context.
        """
        ...


Handler = Callable[[Ctx], Awaitable[None]]
Next = Callable[[Ctx], Awaitable[None]]
Middleware = Callable[[Ctx, Next], Awaitable[None]]


class _RouteWrapper:
    """The route wrapper for a given url and method.
    """

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        ...

    def __call__(self, *args: Any, **kwargs: Any) -> None:
        ...


class _HandlerApplier:
    """The middleware mixed handler applier for a given handler.
    """

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        ...

    def __call__(self, *args: Any, **kwargs: Any) -> Handler:
        ...


class App:

    def __init__(self):
        pass

    def use(self, middleware: Middleware) -> None:
        """Append the middleware to the bottom of this app's middleware stack.
        """
        ...

    def get(self, route: str) -> _RouteWrapper:
        """Append this handler to the bottom of this app's GET handler stack.
        """
        ...

    def post(self, route: str) -> _RouteWrapper:
        """Append this handler to the bottom of this app's POST handler stack.
        """
        ...

    def patch(self, route: str) -> _RouteWrapper:
        """Append this handler to the bottom of this app's PATCH handler stack.
        """
        ...

    def delete(self, route: str) -> _RouteWrapper:
        """Append this handler to the bottom of this app's DELETE handler stack.
        """
        ...


class Server:
    """The server object of thunderlight.
    """

    def __init__(self, app: App, port: int) -> None:
        ...

    def listen(self) -> None:
        """Launch the server which serves app on the given port.
        """
        ...


def main(app: App) -> App:
    """Make the app the default destination for module methods.
    """
    ...


def gimme() -> App:
    """Get the default destination app.
    """
    ...


def make() -> App:
    """Create a new app and assign to the default destination.
    """
    ...


def use(middleware: Middleware) -> None:
    """Apply a middleware to the default destination.
    """
    ...


def get(url: str) -> _RouteWrapper:
    """Register a get handler to the default app's GET handler stack.
    """


def post(url: str) -> _RouteWrapper:
    """Register a post handler to the default app's POST handler stack.
    """


def patch(url: str) -> _RouteWrapper:
    """Register a patch handler to the default app's PATCH handler stack.
    """


def delete(url: str) -> _RouteWrapper:
    """Register a delete handler to the default app's DELETE handler stack.
    """


def apply(middleware: Middleware) -> _HandlerApplier:
    """Apply a middleware to a single handler.
    """
    ...
