import gamefield
import threading
import time
import numpy as np

# TODO: change Color display
# TODO: Something is wrong with the game color update; every second move just gets updated
#

discount = 0.95 # future influence - discount factor
actions = gamefield.actions
states = gamefield.field
specials = gamefield.specials
number_of_actions = len(actions) - 1

Qm = np.zeros((gamefield.rows, gamefield.columns))



def do_action(action):
    old_state = gamefield.agent_position
    reward = -gamefield.score
    if action == actions[0]: # right
        gamefield.try_move((1, 0))
    elif action == actions[1]: # up
        gamefield.try_move((0, -1))
    elif action == actions[2]: # down
        gamefield.try_move((0, 1))
    elif action == actions[3]: # left
        gamefield.try_move((-1, 0))
    else:
        return
    new_state = gamefield.agent_position
    reward += gamefield.score
    return old_state, action, reward, new_state

def update_Q(state, reward, max_val, alpha):
    # Q learning stategy
    # no maximisation over rewards
    Qm[state[1],state[0]] += alpha * (reward + discount * max_val - Qm[state[1],state[0]])
    gamefield.set_cell_color(state, Qm[state[1],state[0]])


def max_Q(state):
    Qs = []
    for action in actions:
        if action == "up":
            Qs.append(Qm[state[1]-1,state[0]])
        elif action == "down":
            Qs.append(Qm[state[1]+1,state[0]])
        elif action == "right":
            Qs.append(Qm[state[1],state[0]+1])
        elif action == "left":
            Qs.append(Qm[state[1],state[0]-1])

    val = max(Qs)
    act = actions[Qs.index(val)]
    return val, act

"""
def max_Q(s): # TODO add different policy, maximise over two steps ?!
    val = None
    act = None
    for a, q in Q[s].items():
        if val is None or (q > val):
            val = q
            act = a
    return act, val

def take_Q(s): # Take Q, with uncertainty ##not right like this!!
    val = None
    act = None
    Qvals = Q[s].items()
    for a, q in Q[s].items():
        if val is None or (q > val):
            val = q
            act = a
    i = random.randint(1,10)
    Qvals.remove((act,val))
    if i < 8:
        return act, val
    else:
        j = random.randint(0,number_of_actions-1)
        return Qvals[j]
        
def inc_Q(s, a, alpha, inc):
    Q[s][a] *= 1 - alpha
    Q[s][a] += alpha * inc
    gamefield.set_cell_color(s, Q[s][a])
"""

def run():
    global discount
    time.sleep(1)
    alpha = 0.9 # learning rate
    t = 1
    while True:
        # Pick the right action
        s = gamefield.agent_position
        (max_val, act) = max_Q(s)
        gamefield.update_arrow(s, act)

        (s, act, reward, s_new) = do_action(act)

        # update Q Value
        (max_val, act) = max_Q(s_new)
        update_Q(s, reward, max_val, alpha)

        # Check if the game has restarted
        t += 1.0
        if gamefield.reset:
            gamefield.reset_agent()
            time.sleep(0.01)
            t = 1.0

        # Update the learning rate
        alpha = pow(t, -0.1)

        # MODIFY THIS SLEEP IF THE GAME IS GOING TOO FAST.
        #time.sleep(0.01)


t = threading.Thread(target=run)
t.daemon = True
t.start()
gamefield.start_field()



# evaluate action

# update action

# Q Values - matrix