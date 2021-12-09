from lightning import Ctx, Next, use, get, gimme, apply


@use
async def error_handler(ctx: Ctx, next: Next) -> None:
    try:
        await next(ctx)
    except Exception as e:
        print(e)
        ctx.res.code = 500
        ctx.res.body = '{"error": {"type": "Internal Server Error"}}'


@use
async def middleware1(ctx: Ctx, next: Next) -> None:
    print("Hello from middleware 1")
    await next(ctx)
    print("Bye from middleware 1")


async def set_operator(ctx: Ctx, next: Next) -> None:
    print("Hello from set operator")
    await next(ctx)
    print("Bye from set operator")


async def set_log(ctx: Ctx, next: Next) -> None:
    print("Hello from set log")
    await next(ctx)
    print("Bye from set log")


@get('/users')
async def users(ctx: Ctx) -> None:
    ctx.res.body = 'user works'


@get('/users/:id')
@apply(set_operator)
@apply(set_log)
async def users(ctx: Ctx) -> None:
    ctx.res.body = ctx.req.args['id']


app = gimme()
