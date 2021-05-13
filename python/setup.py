import os
from setuptools import setup, Extension

try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False
except ImportError:
    bdist_wheel = None

setup(
    name="mecab-bind",
    version="0.996.0",
    python_requires=">=3.6",
    packages=['mecab'],
    package_data={"mecab": ["bind.so", "__init__.pyi"]},
    url="https://github.com/jeongukjae/python-bind",
    author="Jeong Ukjae",
    author_email="jeongukjae@gmail.com",
    cmdclass={'bdist_wheel': bdist_wheel},
    classifiers=[
        "Development Status :: 4 - Beta",
        "License :: OSI Approved :: BSD License",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: Implementation :: CPython",
        "Topic :: Software Development :: Libraries",
        "Typing :: Typed",
    ],
)
