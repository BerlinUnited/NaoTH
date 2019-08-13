import sys, os

# add the current directory to search path
sys.path.append(os.path.dirname(__file__))

# add Framework stuff (opencv) to search path
versionPart = 'python2.7' if sys.version_info[0] == 2 else 'python3.6'
fwPath = os.path.join(os.getenv('EXTERN_PATH_NATIVE'),'lib', versionPart, 'site-packages')
if os.path.exists(fwPath):
	sys.path.append(fwPath)

# import some of our classes
from LogReader import LogReader, Parser
from SPLMessage import SPLMessage, MixedTeamMessage
from .BehaviorParser import BehaviorParser