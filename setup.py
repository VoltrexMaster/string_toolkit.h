from setuptools import setup, Extension

setup(
    name='string-toolkit',
    version='0.0.4',
    description='Python wrapper for the string_toolkit.h C/C++ header',
    ext_modules=[
        Extension('st', ['python_binding.c'])
    ],
    license='MIT',
    long_description=open('README_PYTHON.md', 'r', encoding='utf-8').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/vierofernando/string_toolkit.h',
    keywords=['string', 'string-manipulation', 'c', 'cpp', 'c-plus-plus']
)