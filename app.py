from thunderlight.app import App
from thunderlight.ctx import Ctx
from asyncio import ensure_future
app = App()


@app.get("/")
async def home(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, Home!".encode("utf-8")


@app.get("/hello")
async def hello(ctx: Ctx):
    ctx.res.code = 404
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, World!".encode("utf-8")
