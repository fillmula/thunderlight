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
    ctx.res.empty()


@app.get("/hello")
async def hello(ctx: Ctx):
    ctx.res.text("Hello, World!")


@app.get("/json")
async def json(ctx: Ctx):
    ctx.res.json({"data": {"name": "Leon", "age": 20}})
