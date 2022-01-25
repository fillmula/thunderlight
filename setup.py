"""setup.py"""
from pathlib import Path
from setuptools import setup, Extension

# The text of the README file
README = (Path(__file__).parent / "README.md").read_text()

setup(
    name='thunderlight',
    version='0.2.7',
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
    packages=["thunderlight-stubs"],
    ext_modules=[
        Extension('thunderlight', [
            'thunderlight/server.c',
            'thunderlight/protocol.c',
            'thunderlight/app.c', 'thunderlight/matcher.c', 'thunderlight/middleware_chain.c', 'thunderlight/applier.c',
            'thunderlight/ctx.c', 'thunderlight/context.c',
            'thunderlight/req.c', 'thunderlight/req_headers.c', 'thunderlight/req_match.c', 'thunderlight/request.c', 'thunderlight/mresult.c',
            'thunderlight/res.c', 'thunderlight/response.c', 'thunderlight/buffer.c', 'thunderlight/res_headers.c', 'thunderlight/response_headers.c', 'thunderlight/hash.c',
            'thunderlight/state.c', 'thunderlight/duostate.c',
            'thunderlight/not_found.c', 'thunderlight/space.c', 'thunderlight/json.c',
            'thunderlight/module.c'
        ]),
    ]
)
