import os, sys

from distutils.core import setup, Extension
from distutils import sysconfig

cpp_args = ['-std=c++11']

sfc_module = Extension(
    'RobotApp', sources = ['RobotApp.cpp'],
    include_dirs=['pybind11/include'],
    language='c++',
    extra_compile_args = cpp_args,
    )

setup(
    name = 'RobotApp',
    version = '0.1',
    description = 'Python package with Robot Application Module (PyBind11)',
    ext_modules = [sfc_module],
)