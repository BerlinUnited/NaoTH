import glob
import importlib
import os

import Utils

__all__ = ['cases']

cases = {}

for __file in glob.glob(os.path.join(os.path.dirname(__file__), '*.py')):
    __name = os.path.basename(__file)
    if not __name.startswith('_'):
        __module = importlib.import_module(__name__ + '.' + __name[:-3])
        __module_desc = __module.__doc__.strip() if __module.__doc__ else ''
        for name in dir(__module):
            obj = getattr(__module, name)
            if isinstance(obj, type) and issubclass(obj, Utils.TestRun) and obj != Utils.TestRun:
                cases[name] = (obj, obj.__doc__.strip() if obj.__doc__ else '')
