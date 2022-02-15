from thunderlight import Ctx, Next, post, use, get, gimme, apply


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
    ctx.res.json({"data": {"here": "works nice"}})


@get('/users/:id')
@apply(set_operator)
@apply(set_log)
async def users(ctx: Ctx) -> None:
    ctx.res.json({"data": {"id": ctx.req.args['id']}})


@post('/images')
async def upload(ctx: Ctx) -> None:
    data = await ctx.req.form()
    files = []
    for k, v in data.items():
        files.append({
            "key": k,
            "file-type": v.content_type
        })
    ctx.res.json({"message": {"status": "OK", "files": files }})

@get('/text')
async def text(ctx: Ctx) -> None:
    ctx.res.text("This is text")


@get('/html')
async def html(ctx: Ctx) -> None:
    ctx.res.html("<div>Hello world</div>")


@get('/redirect')
async def redirect(ctx: Ctx) -> None:
    ctx.res.redirect('/users')



app = gimme()
