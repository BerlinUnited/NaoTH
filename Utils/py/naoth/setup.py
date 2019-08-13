#!/usr/bin/python

from setuptools import setup

setup(name='naoth',
      version='0.2',
      description='Python Utils for the NaoTH toolchain',
      packages=["naoth"],
      install_requires=[
          'protobuf',
      ],
      zip_safe=False)
