"""setup.py"""
from pathlib import Path
from setuptools import setup, Extension

# The text of the README file
README = (Path(__file__).parent / "README.md").read_text()

setup(
    name='thunderlight',
    version='0.2.0',
    description=('A Python HTTP Server in C.'),
    long_description=README,
    long_description_content_type='text/markdown',
    author='Victor Teo',
    author_email='victor.teo@fillmula.com',
    license='MIT',
    packages=['thunderlight'],
    package_data={'thunderlight': []},
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
    ext_modules=[
        #Extension('thunderlight', ['thunderlight/main.c']),
        Extension('thunderlight.req', ['thunderlight/req.c', 'thunderlight/req_headers.c', 'thunderlight/request.c']),
        Extension('thunderlight.req_headers', ['thunderlight/req_headers.c', 'thunderlight/request.c']),
        Extension('thunderlight.res', ['thunderlight/res.c', 'thunderlight/response.c', 'thunderlight/buffer.c', 'thunderlight/res_headers.c', 'thunderlight/response_headers.c', 'thunderlight/hash.c']),
        Extension('thunderlight.res_headers', ['thunderlight/res_headers.c', 'thunderlight/response_headers.c', 'thunderlight/hash.c']),
        Extension('thunderlight.state', ['thunderlight/state.c', 'thunderlight/duostate.c', 'thunderlight/hash.c']),
        Extension('thunderlight.ctx', [
            'thunderlight/ctx.c', 'thunderlight/context.c',
            'thunderlight/req.c', 'thunderlight/req_headers.c', 'thunderlight/request.c',
            'thunderlight/res.c', 'thunderlight/response.c', 'thunderlight/buffer.c', 'thunderlight/res_headers.c', 'thunderlight/response_headers.c', 'thunderlight/hash.c',
            'thunderlight/state.c', 'thunderlight/duostate.c'
        ]),
        Extension('thunderlight.not_found', [
            'thunderlight/not_found.c',
            'thunderlight/ctx.c', 'thunderlight/context.c',
            'thunderlight/req.c', 'thunderlight/req_headers.c', 'thunderlight/request.c',
            'thunderlight/res.c', 'thunderlight/response.c', 'thunderlight/buffer.c', 'thunderlight/res_headers.c', 'thunderlight/response_headers.c', 'thunderlight/hash.c',
            'thunderlight/state.c', 'thunderlight/duostate.c'
        ]),
        Extension('thunderlight.middleware_chain', ['thunderlight/middleware_chain.c']),
        Extension('thunderlight.server', ['thunderlight/server.c']),
    ]
)
