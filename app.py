from thunderlight.app import App
from thunderlight.ctx import Ctx

app = App()

@app.get("/hello")
async def hello(ctx: Ctx):
    ctx.res.code = 200
    ctx.res.headers['Content-Type'] = "text/plain"
    ctx.res.body = "Hello, World!"
