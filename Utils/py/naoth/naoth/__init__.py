import sys
import os

# add protobuf directory to search path
directory = os.path.dirname(__file__)
sys.path.append(os.path.join(directory, 'messages'))
