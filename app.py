from thunderlight import Ctx, Next, use, get
from time import time


@use
async def middleware1(ctx: Ctx, next: Next):
    start = time()
    await next(ctx)
    end = time()
    print(f"{ctx.res.code} - {ctx.req.path} - {(end - start) * 1000}ms")


@get("/")
async def home(ctx: Ctx):
    ctx.res.empty()


@get("/hello")
async def hello(ctx: Ctx):
    ctx.res.text("Hello, World!")


@get("/json")
async def json(ctx: Ctx):
    ctx.res.json({"data": {"name": "Leon", "age": 20}})
