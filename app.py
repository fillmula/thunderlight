from thunderlight import Ctx, Next, use, get


@use
async def middleware1(ctx: Ctx, next: Next):
    print("before 1")
    await next(ctx)
    print("after 1")


@use
async def middleware2(ctx: Ctx, next: Next):
    print("before 2")
    await next(ctx)
    print("after 2")


@use
async def middleware3(ctx: Ctx, next: Next):
    print("before 3")
    await next(ctx)
    print("after 3")


@get("/")
async def home(ctx: Ctx):
    ctx.res.empty()


@get("/hello")
async def hello(ctx: Ctx):
    ctx.res.text("Hello, World!")


@get("/json")
async def json(ctx: Ctx):
    ctx.res.json({"data": {"name": "Leon", "age": 20}})
