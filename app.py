from thunderlight import App, Ctx


app = App()


@app.get("/")
async def home(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, Home!".encode("utf-8")


@app.get("/hello")
async def hello(ctx: Ctx):
    ctx.res.code = 200
    print("code is", ctx.res.code)
    ctx.res.headers['Content-Type'] = "text/plain"
    print("headers are", ctx.res.headers)
    print("SEE", ctx.res.headers['Content-Type'])
    ctx.res.body = "Hello, World!".encode("utf-8")
    print("body is", ctx.res.body)
