import gamefield
import threading
import time
import numpy as np

# TODO: change Color display
# TODO: Something is wrong with the game color update; every second move just gets updated
#

discount = 0.91 # future influence - discount factor
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

def update_Q_rand(state): # old does not contain the actions of the states
    # value Iteration
    # Q learning stategy
    # no maximisation over rewards

    Qm[state[1],state[0]] = np.random.uniform(0.0,1.0)
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
    path = {}
    while True:
        position = gamefield.agent_position
        update_Q_rand(position)
        print actions
        print "input: "
        try:
            action_input = input()
            do_action(actions[action_input])

        except:
            pass
        path[position] = [actions[action_input], Qm[position[1],position[0]]]

        print gamefield.agent_position

        if gamefield.reset:
            update_path(path) # update the path
            gamefield.reset_agent()
            time.sleep(0.01)
            path.clear()

t = threading.Thread(target=run)
t.daemon = True
t.start()
gamefield.start_field()



# evaluate action

# update action

# Q Values - matrix