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


@get("/users")
async def users(ctx: Ctx):
    ctx.res.json({"data": {"users": []}})


@get("/users/:id")
async def user(ctx: Ctx):
    print(ctx.req.args)
    print(ctx.req.args['id'])
    ctx.res.json({"data": {"user": "id"}})


@get("/users/:id/posts")
async def user_posts(ctx: Ctx):
    ctx.res.json({"data": {"posts": []}})


@get("/users/:id/posts/:post_id")
async def user_post(ctx: Ctx):
    print(ctx.req.args)
    ctx.res.json({"data": {"post": ctx.req.args["post_id"]}})
