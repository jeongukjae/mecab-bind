import os
from setuptools import setup, find_packages

# https://stackoverflow.com/questions/45150304/how-to-force-a-python-wheel-to-be-platform-specific-when-building-it
try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel

    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False
except ImportError:
    bdist_wheel = None

with open(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name="mecab-tf",
    version="0.996.0a1",
    python_requires=">=3.6",
    packages=find_packages(),
    package_data={"mecab_tf.python.ops": ["_mecab_ops.so", "libmecab.so.2"]},
    url="https://github.com/jeongukjae/python-bind",
    long_description=long_description,
    long_description_content_type='text/markdown',
    author="Jeong Ukjae",
    author_email="jeongukjae@gmail.com",
    cmdclass={"bdist_wheel": bdist_wheel},
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: Implementation :: CPython",
        "Topic :: Software Development :: Libraries",
        "Typing :: Typed",
    ],
)
