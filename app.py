from thunderlight import Ctx, Next, use, get, post
from time import time


@use
async def mylog(ctx: Ctx, next: Next):
    start = time()
    await next(ctx)
    end = time()
    print(f"({ctx.res.code}) {ctx.req.method} {ctx.req.path} - {round((end - start) * 1000, 2)}ms")


@get("/")
async def home(ctx: Ctx):
    ctx.res.empty()


@get("/hello")
async def hello(ctx: Ctx):
    ctx.res.text("Hello, World!")


@get("/hello/2")
async def hello_2(ctx: Ctx):
    ctx.res.json({"data": {"index": 2}})


@get("/users/:id")
async def user(ctx: Ctx):
    ctx.res.json({"data": {"user": "id"}})
