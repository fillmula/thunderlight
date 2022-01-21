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


print("can get")
@get("/")
async def home(ctx: Ctx):
    ctx.res.empty()

print("can get")
@get("/hello")
async def hello(ctx: Ctx):
    ctx.res.text("Hello, World!")

print("can get")
@get("/json")
async def json(ctx: Ctx):
    ctx.res.json({"data": {"name": "Leon", "age": 20}})
print("can exit")
