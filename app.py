from thunderlight import App, Ctx, Next


app = App()


@app.use
async def middleware1(ctx: Ctx, next: Next):
    print("before 1")
    await next(ctx)
    print("after 1")


@app.use
async def middleware2(ctx: Ctx, next: Next):
    print("before 2")
    await next(ctx)
    print("after 2")


@app.use
async def middleware3(ctx: Ctx, next: Next):
    print("before 3")
    await next(ctx)
    print("after 3")


@app.get("/")
async def home(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, Home!".encode("utf-8")


@app.get("/hello")
async def hello(ctx: Ctx):
    print("inside before")
    ctx.res.empty("any")
    print("inside after")
