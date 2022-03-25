⚡️Thunderlight is the fastest Python HTTP Server
=========

Thunderlight is the fastest Python HTTP Server. It's dedicated for micro service
and JSON API. In the future, a C version of this API is also supported. You can
write Python or C in your server project to speed things up.

## Installation

Install Thunderlight with `pip`.

```sh
pip install thunderlight
```

## Documentation

### `get`

Register a get request handler to the default app.

```python
@get('/posts/:id')
async def posts(ctx: Ctx) -> None:
    ctx.res.json((await Post.id(ctx.req.args.id)).tojson())
```

### `post`

Register a post request handler to the default app.

```python
@post('/posts')
async def posts(ctx: Ctx) -> None:
    ctx.res.json(Post(**(await ctx.req.json())).save().tojson())
```

### `patch`

Register a patch request handler to the default app.

```python
@patch('/posts/:id')
async def posts(ctx: Ctx) -> None:
    ctx.res.json((await Post.id(ctx.req.args.id)).set(await ctx.req.json()).save().tojson())
```

### `delete`

Register a delete request handler to the default app.

```python
@delete('/posts/:id')
async def posts(ctx: Ctx) -> None:
    ctx.res.empty((await Post.id(ctx.req.args.id)).delete())
```

### `use`

Append this middleware to the default app's middleware stack.

```python
@use
async def measure_time(ctx: Ctx, next: Next) -> None:
    start = time()
    await next(ctx)
    time_elapsed = time() - start
    print(f'request {ctx.req.path} takes {time_elapsed} second.')
```

### `apply`

Apply a middleware to a request handler. This is aka middleware for a single
request handler.

```python
async def validate_user(ctx: Ctx, next: Next) -> None:
    if ctx.req.headers['Authorization'] is not None:
        await next(ctx)
    else:
        ctx.res.code = 401
        ctx.res.json({"error": {"type": "Unauthorized"}})


@get('/users')
@apply(validate_user)
async def users(ctx: Ctx) -> None:
    ctx.res.json(await User.find())
```

### `App`

Create a new server application.

```python
app = App()

@app.get('/articles')
async def articles(ctx: Ctx) -> None:
    ctx.res.json(await Article.find())
```

### `Ctx`

The context represents a request session.

#### `ctx.req`

The request object. This object is readonly.

#### `ctx.res`

The response object. Modify this object to present data to the user.

#### `ctx.state`

The custom state object that is internal to your custom handling code.

### `Req`

This class represents the incoming request. This object is readonly.

#### `req.client`

The request client.

#### `req.scheme`

The request HTTP scheme.
#### `req.version`

The request HTTP version.

#### `req.method`

The request HTTP method.

#### `req.path`

The path of the request URL.

#### `req.args`

The args matched from url routing.

#### `req.qs`

The query string of the request URL.

#### `req.headers`

The request headers.

#### `await req.body()`

The raw request body.

#### `await req.json()`

The request's json body.

#### `await req.form()`

The request's form body.

#### `await req.dict()`

The request's json body or form body.


### `Res`

The class represents the response returned to the frontend.

#### `res.code`

The status code.

#### `res.headers`

The response headers.

#### `res.body`

The response body.

#### `res.json()`

Response json to the frontend.

#### `res.text()`

Response text to the frontend

#### `res.html()`

Response html text to the frontend

#### `res.redirect()`

Response a redirect request to the frontend

#### `res.file()`

Response a file to the frontend

#### `res.empty()`

Empty response

### `State`

You can attach anything to the state and fetch by key. This state shares the
same state across middlewares and the route handler.

```python
@app.get('/articles')
async def articles(ctx: Ctx) -> None:
    ctx.state.user = custom_user
    ctx.state.user # custom_user
```

## Changelog

### 0.6.0 (Mar 25th, 2022)
* Added generic response headers
* Added wildcard URL route matching
* Added static file serving

### 0.5.1 (Mar 8th, 2022)
* Temporarily add static file

### 0.5.0 (Feb 24th, 2022)
* Revert to Python implementation as the C implementation is not stable

### 0.2.8 (Jan 28th, 2022)
* Fixed a bug that causes patch and delete requests to crash

### 0.2.7 (Jan 25th, 2022)
* Fixed a bug that causes errors in handlers not propagated
* Add `__contains__` method to `ReqHeaders`

### 0.2.6 (Jan 25th, 2022)
* Fixed a bug that would cause delete routes crashing

### 0.2.5 (Jan 24th, 2022)
* Remove uvicorn and related dependencies
* Rewrite with C
* Fix req.json property

### 0.1.0 (Dec 10th, 2021)
* Global methods added: `get`, `post`, `patch`, `delete`, `use` and `apply`.
* Classes added: `App`, `Ctx`, `Req`, `Res`, `State`.
* Function definition added: `Handler`, `Next` and `Middleware`.
* Global helper methods added: `main`, `gimme` and `make`.
