#!/usr/bin/python
import os
from setuptools import setup, find_packages

version_file = open(os.path.join(".", 'VERSION'))

setup(name='naoth',
      version=version_file.read().strip(),
      author='NaoTH Berlin United',
      author_email='nao-team@informatik.hu-berlin.de',
      description='Python utils for the NaoTH toolchain',
      packages=find_packages(exclude=["tests"]),
      zip_safe=False,
      setup_requires=['wheel'],
      install_requires=[
          'protobuf', 'numpy'
      ],
      python_requires='>=3.6.9',
      )
