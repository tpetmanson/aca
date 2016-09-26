from distutils.core import setup, Extension
from Cython.Build import cythonize


setup(
    ext_modules=cythonize(
        Extension('ac_cpp',
                  sources=['src/match.cpp', 'src/node.cpp', 'src/automaton.cpp'],
                  language='c++',
                  extra_compile_args=['-std=c++11', '-mmacosx-version-min=10.9'],
                  extra_link_args=['-std=c++11'])
    )
)
