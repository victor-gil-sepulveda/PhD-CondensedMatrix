"""
Created on 25/02/2013

@author: victor
"""
from distutils.core import setup, Extension
import numpy
import distutils.sysconfig
import os

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()


setup(
      name = 'CondensedMatrix',
      version = '0.0.1',
      description = 'Fast access implementation of a distance matrix (symmetric, 0 diagonal).',
      author = 'Victor Alejandro Gil Sepulveda',
      author_email = 'victor.gil.sepulveda@gmail.com',
      url = 'https://github.com/victor-gil-sepulveda/PhD-CondensedMatrix.git',
      packages = ['condensedMatrix'],
      package_dir = {'condensedMatrix':'./condensedMatrix'},
      py_modules = [
                  'condensedMatrix.matrixHandler',
                  ],
      include_dirs = [
                      numpy.get_include(),
                      distutils.sysconfig.get_python_inc()
                      ],
      ext_modules = [
                   Extension('condensedMatrix.matrix', [
                                                 'src/Matrix.cpp',
                                                 'src/Statistics.cpp'
                                                 ],
                             extra_compile_args = ['-O3']
                    )
      ],
      license = 'LICENSE.txt',
      long_description = read('README.md')
)
