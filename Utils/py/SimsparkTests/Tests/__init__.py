import glob
import importlib
import os

functions = {}

for __file in glob.glob(os.path.join(os.path.dirname(__file__), '*.py')):
    __name = os.path.basename(__file)
    if not __name.startswith('_'):
        __module = importlib.import_module(__name__ + '.' + __name[:-3])
        __module_desc = __module.__doc__.strip() if __module.__doc__ else ''
        functions[__name[:-3]] = (getattr(__module, __name[:-3], None), __module_desc)
