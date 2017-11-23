import gamefield
import threading
import time
import numpy as np

# TODO: change Color display
# TODO: Something is wrong with the game color update; every second move just gets updated
# -> caused by the given policy

discount = 0.91 # future influence - discount factor
actions = gamefield.actions
states = gamefield.field
specials = gamefield.specials
number_of_actions = len(actions)

Qm = np.zeros((gamefield.rows, gamefield.columns))

Qm_new = np.zeros((gamefield.rows * gamefield.columns, len(actions)))
"""
row - y coord of the field
column - x coord of the field
actions = [1,...,n]
e.g.:
(i,j) position with action 1 is the rows*(j-1) + i th row and 1 st column of the matrix
"""

def position_to_matix_coord(position,action):
    return (gamefield.rows*(position[1]-1)+position[0],actions.index(action))

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

def update_Q(state, reward, max_val, alpha): # old does not contain the actions of the states
    # value Iteration
    # Q learning stategy
    # no maximisation over rewards

    Qm[state[1],state[0]] += alpha * (reward + discount * max_val - Qm[state[1],state[0]])
    #gamefield.set_cell_color(state, Qm[state[1],state[0]])

def update_Q2(state, action, reward, max_val, alpha): # new, right version
    # value Iteration
    # Q learning stategy
    # no maximisation over rewards
    entry = position_to_matix_coord(state, action)
    Qm[entry] += alpha * (reward + discount * max_val - Qm[entry])

    #Qm[state[1],state[0]] += alpha * (reward + discount * max_val - Qm[state[1],state[0]])
    #gamefield.set_cell_color(state, Qm[state[1],state[0]])


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

def update_path(path):
    for key in path:
        gamefield.set_cell_color(key,Qm[key[1],key[0]])
        gamefield.update_arrow(key,path[key][0])


def run():
    global discount
    time.sleep(1)
    alpha = 0.9 # learning rate
    t = 1
    path = {}
    while True:
        # Pick the right action
        s = gamefield.agent_position

        (max_val, act) = max_Q(s)


        (s_old, act, reward, s_new) = do_action(act)

        # update Q Value
        (max_val, max_act) = max_Q(s_new)
        update_Q(s, reward, max_val, alpha)

        path[s] = [act, Qm[s[1],s[0]]]

        # TODO restarts to early
        # Check if the game has restarted
        t += 1.0
        if gamefield.reset:
            update_path(path) # update the path
            path.clear() # clear dictionary
            gamefield.reset_agent()
            time.sleep(0.01)
            t = 1.0

        # Update the learning rate
        alpha = pow(t, -0.1)

        # MODIFY THIS SLEEP IF THE GAME IS GOING TOO FAST.
        # time.sleep(0.01)


t = threading.Thread(target=run)
t.daemon = True
t.start()
gamefield.start_field()



# evaluate action

# update action

# Q Values - matrix