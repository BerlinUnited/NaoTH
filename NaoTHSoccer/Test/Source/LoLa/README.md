# Run lola.py on the nao

This script needs the python messagepack package. The easiest way to install it is by using the deprecated msgpack-python package. This is a messagepack implementation purely in python and thus can be installed on the nao without the need of a compiler.

- on your local machine run `pip download msgpack-python -d .` This will download a .tar.gz file
- untar the file and move to the nao robot
- run `python setup.py install --user` inside the directory where the setup.py is located
- copy `lola.py` to the nao and run it with `python lola.py`