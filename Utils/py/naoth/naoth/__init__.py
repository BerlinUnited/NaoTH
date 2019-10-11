import sys, os

# add the current directory to search path
sys.path.append(os.path.dirname(__file__))

# import some of our classes
from LogReader import LogReader, Parser
from SPLMessage import SPLMessage, MixedTeamMessage
from .BehaviorParser import BehaviorParser