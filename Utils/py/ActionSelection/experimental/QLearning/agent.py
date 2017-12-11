import gamefield
from strategies import *
import time
import threading

"""
set important parameters regarding the environment and behavior

for simplification we assume:
 - all actions are alway available
 - we know which states exist

to set:
 - which policy to use
 - available actions
 - available states
"""
policy = None
actions = None
states = None



def start_agent(policy, states, actions):
    pass

if __name__ == "__main__":

    # collect data
    """
    if states != None:
        num_states = len(states)
    if actions != None:
        num_actions = len(actions)
    """

    t = threading.Thread(target=start_agent(policy, states, actions))
    t.daemon = True
    t.start()
    gamefield.start_field()