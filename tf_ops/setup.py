import os
from setuptools import setup, find_packages
from setuptools.dist import Distribution
from setuptools.command.install import install

class BinaryDistribution(Distribution):
  """This class is needed in order to create OS specific wheels."""

  def has_ext_modules(self):
    return True

  def is_pure(self):
    return False

class InstallPlatlib(install):
    def finalize_options(self):
        install.finalize_options(self)
        self.install_lib = self.install_platlib


with open(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name="mecab-tf",
    version="0.996.0a1",
    python_requires=">=3.6",
    packages=find_packages(),
    url="https://github.com/jeongukjae/python-bind",
    long_description=long_description,
    long_description_content_type='text/markdown',
    author="Jeong Ukjae",
    author_email="jeongukjae@gmail.com",
    include_package_data=True,
    zip_safe=False,
    distclass=BinaryDistribution,
    cmdclass={'install': InstallPlatlib},
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
