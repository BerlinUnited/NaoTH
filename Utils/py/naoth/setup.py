#!/usr/bin/python
from setuptools import setup, find_packages
from pathlib import Path

# The directory containing this file
HERE = Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text()
VERSION_STRING = (HERE / "VERSION").read_text()

setup(name='naoth',
      version=VERSION_STRING,
      author='NaoTH Berlin United',
      author_email='nao-team@informatik.hu-berlin.de',
      description='Python utils for the NaoTH toolchain',
      long_description=README,
      long_description_content_type="text/markdown",
      packages=find_packages(exclude=["tests"]),
      license='Apache License 2.0',
      zip_safe=False,
      setup_requires=['wheel'],
      install_requires=[
          'protobuf==3.20.3', 'numpy'
      ],
      python_requires='>=3.6.9',
      )
