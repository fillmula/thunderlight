from thunderlight import App, Ctx


app = App()


@app.get("/")
async def home(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, Home!".encode("utf-8")


@app.get("/hello")
async def hello(ctx: Ctx):
    try:
        print("RUN HELLO HERE")
        print(ctx.res)
    except Exception as e:
        print(e)
    ctx.res.code = 404
    print("code is", ctx.res.code)
    ctx.res.headers['Content-Type'] = "text/plain"
    print("headers are", ctx.res.headers)
    ctx.res.body = "Hello, World!".encode("utf-8")
    print("body is", ctx.res.body)
