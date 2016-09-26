from distutils.core import setup, Extension
from Cython.Build import cythonize


setup(
    ext_modules=cythonize(
        Extension('ac/ac_cpp',
                  sources=['ac/ac.pyx'],
                  language='c++',
                  extra_compile_args=['-std=c++11', '-mmacosx-version-min=10.9'],
                  extra_link_args=['-std=c++11'])
    )
)
