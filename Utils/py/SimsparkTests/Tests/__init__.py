import glob
import importlib
import os

functions = {}

for __file in glob.glob(os.path.join(os.path.dirname(__file__), '*.py')):
    __name = os.path.basename(__file)
    if not __name.startswith('_'):
        functions[__name[:-3]] = getattr(importlib.import_module(__name__ + '.' + __name[:-3]), __name[:-3], None)
