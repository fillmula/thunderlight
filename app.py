from typing import Any
from thunderlight import App, Ctx


app = App()


@app.use
async def middleware1(ctx: Ctx, next: Any):
    print("before out")
    await next(ctx)
    print("after out")


# @app.use
# async def middleware2(ctx: Ctx, next: Any):
#     print("before mid")
#     await next(ctx)
#     print("after mid")


# @app.use
# async def middleware3(ctx: Ctx, next: Any):
#     print("before in")
#     await next(ctx)
#     print("after in")


@app.get("/")
async def home(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, Home!".encode("utf-8")


@app.get("/hello")
async def hello(ctx: Ctx):
    print(ctx.req)
    print(ctx.req.method)
    print(ctx.req.path)
    print(ctx.req.query)
    print(ctx.req.version)
    print(ctx.req.headers)
    print(ctx.req.body)
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, World!"
    print(ctx.res)
    print(ctx.res.code)
    print(ctx.res.headers)
    print(ctx.res.body)
