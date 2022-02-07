"""setup.py"""
from pathlib import Path
from setuptools import setup, Extension

# The text of the README file
README = (Path(__file__).parent / "README.md").read_text()

setup(
    name='thunderlight',
    version='0.3.0',
    description=('Fastest Python HTTP Server, written in C.'),
    long_description=README,
    long_description_content_type='text/markdown',
    author='Victor Teo',
    author_email='victor.teo@fillmula.com',
    license='MIT',
    zip_safe=False,
    url='https://github.com/fillmula/thunderlight',
    include_package_data=True,
    python_requires='>=3.10',
    classifiers=[
        "Programming Language :: Python :: 3.10"
    ],
    install_requires=[
        'uvloop>=0.16.0',
    ],
    scripts=['scripts/thunderlight'],
    package_data={"thunderlight-stubs": ["__init__.pyi"]},
    packages=["thunderlight-stubs", "thunderlight"],
    ext_modules=[
        Extension('_thunderlight', [
            'ext/server.c',
            'ext/protocol.c',
            'ext/app.c', 'ext/matcher.c', 'ext/middleware_chain.c', 'ext/applier.c',
            'ext/ctx.c', 'ext/context.c',
            'ext/req.c', 'ext/req_headers.c', 'ext/req_match.c', 'ext/request.c', 'ext/mresult.c',
            'ext/res.c', 'ext/response.c', 'ext/buffer.c', 'ext/res_headers.c', 'ext/response_headers.c', 'ext/hash.c',
            'ext/state.c', 'ext/duostate.c',
            'ext/not_found.c', 'ext/space.c', 'ext/json.c', 'ext/loop.c',
            'ext/module.c'
        ]),
    ]
)
