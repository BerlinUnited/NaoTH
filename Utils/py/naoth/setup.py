#!/usr/bin/python

from setuptools import setup, find_packages

setup(name='naoth',
      version='0.3',
      author='NaoTH Berlin United',
      author_email='nao-team@informatik.hu-berlin.de',
      description='Python utils for the NaoTH toolchain',
      packages=find_packages(),
      zip_safe=False,
      setup_requires=['wheel'],
      install_requires=[
          'protobuf', 'numpy'
      ],
      python_requires='>=3.6.9',
      )
