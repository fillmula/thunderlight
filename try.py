from lightning import Ctx, Next, use, get, make


@use
async def error_handler(ctx: Ctx, next: Next) -> None:
    try:
        await next(ctx)
    except Exception as e:
        print("ERROR IS")
        print(e)
        ctx.res.code = 500
        ctx.res.body = '{"type": "Internal Server Error"}'


@use
async def middleware1(ctx: Ctx, next: Next) -> None:
    print("Hello from middleware 1")
    await next(ctx)
    print("Bye from middleware 1")


@get('/users')
async def users(ctx: Ctx) -> None:
    ctx.res.body = 'user works'


@get('/users/:id')
async def users(ctx: Ctx) -> None:
    ctx.res.body = ctx.req.args['id']


app = make()
