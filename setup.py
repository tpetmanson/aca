import setuptools
import platform
from distutils.core import setup, Extension
from Cython.Build import cythonize
from distutils.command.sdist import sdist as _sdist
from setuptools import find_packages

# this is for making the package documentation look nice on PyPi
try:
    import pypandoc
    LONG_DESCRIPTION = pypandoc.convert('README.md', 'rst')
except(IOError, ImportError):
    LONG_DESCRIPTION = open('README.md').read()


NAME = "aca"
VERSION = '0.9'
DESCRIPTION = 'Aho-Corasick automaton implementation in C++'
AUTHOR = 'Timo Petmanson @Funderbeam'
AUTHOR_EMAIL = 'tpetmanson@gmail.com'
LICENSE = 'GPLv3'
URL = 'https://github.com/tpetmanson/aca'

CLASSIFIERS = [
    'Intended Audience :: Developers',
    'Topic :: Text Processing',
    'Topic :: Text Processing :: Linguistic',
    'Programming Language :: Python :: 3.4',
    'Programming Language :: Python :: 3.5'
]


REQUIREMENTS = [
    'Cython>=0.25.0,<0.26',
    'wheel>0.25.0',
    'six',
    'pytest']

EXTRA_ARGS = ['-std=c++11']

osname = platform.system().lower()
if 'linux' in osname:
    pass
if 'darwin' in osname:
    EXTRA_ARGS.append('-mmacosx-version-min=10.9')
if 'windows' in osname:
    pass # TODO

EXTENSIONS = [
    Extension('aca.aca_cpp',
              sources=['aca/aca_cpp.pyx'],
              language='c++',
              extra_compile_args=EXTRA_ARGS,
              extra_link_args=['-std=c++11'])]

setup(
    name=NAME,
    version=VERSION,
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    author=AUTHOR,
    author_email=AUTHOR_EMAIL,
    url=URL,
    license=LICENSE,
    classifiers=CLASSIFIERS,
    setup_requires=REQUIREMENTS,
    install_requires=REQUIREMENTS,
    packages=find_packages(),
    include_package_data=True,

    ext_modules=cythonize(EXTENSIONS)
)
