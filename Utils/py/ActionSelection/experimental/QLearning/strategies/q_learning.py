import gamefield
import threading
import time
import numpy as np

class q_learning():
    def __init__(self, num_actions, num_states):
        """
        initializes the q_learning algorithm
        
        :param actions: integer
        :param states: integer
        """
        self.actions = actions
        self.states = states

        Q = np.zeros((num_states, num_actions))

    def run_algorithm(self):
        pass

discount = 0.91  # future influence - discount factor
actions = gamefield.actions
states = gamefield.field
specials = gamefield.specials
number_of_actions = len(actions) - 1

Qm = np.zeros((gamefield.rows * gamefield.columns, len(actions)))
"""
row - y coord of the field
column - x coord of the field
actions = [1,...,n]
e.g.:
(i,j) position with action 1 is the rows*(j-1) + i th row and 1 st column of the matrix
"""


def position_to_matix_coord(position, action):
    return (gamefield.rows * (position[1] - 1) + position[0], actions.index(action))


def Q_values_actions(position):
    """

    :param position: position of the agent -- tupel
    :return: row of the Q matrix -- Q values of all actions
    """
    return Qm[(gamefield.rows * (position[1] - 1) + position[0]), :]


def do_action(action):
    old_state = gamefield.agent_position
    reward = -gamefield.score
    if action == actions[0]:  # right
        gamefield.try_move((1, 0))
    elif action == actions[1]:  # up
        gamefield.try_move((0, -1))
    elif action == actions[2]:  # down
        gamefield.try_move((0, 1))
    elif action == actions[3]:  # left
        gamefield.try_move((-1, 0))
    else:
        return
    new_state = gamefield.agent_position
    reward += gamefield.score  # compute difference between score before and after action
    return old_state, action, reward, new_state


def update_Q(state, action, reward, max_val, alpha):  # new, right version
    # value Iteration
    # Q learning stategy
    # no maximisation over rewards
    entry = position_to_matix_coord(state, action)
    Qm[entry] += alpha * (reward + discount * max_val - Qm[entry])
    if Qm[entry] > 1:
        print Qm[entry]


def max_Q(state):
    """
    gives back the maximal q value at the given state
    returns the value und action

    :param state: position of the robot 
    :return: (max q value, action)
    """
    Q_vals = Q_values_actions(state)
    argmax = np.argmax(Q_vals)
    act = actions[argmax]
    val = Q_vals[argmax]
    return val, act


def update_path(path):
    """
    updates the visual interface with the taken path and values

    :param path: 
    :return: 
    """
    for key in path:
        (position, action) = (key, path[key][0])
        gamefield.set_cell_color(key, Qm[position_to_matix_coord(position, action)])
        gamefield.update_arrow(position, action)


def run():
    global discount
    time.sleep(1)
    alpha = 0.9  # learning rate
    t = 1
    path = {}
    while True:
        # TODO: Add exploration;
        # example: https://studywolf.wordpress.com/2012/11/25/reinforcement-learning-q-learning-and-exploration/
        # TODO: case of multiple equally good Q actions

        # Pick the right action
        current_state = gamefield.agent_position

        (max_val, act) = max_Q(current_state)

        (state_old, act, reward, current_state) = do_action(act)

        # update Q Value
        (max_val, max_act) = max_Q(current_state)
        update_Q(state_old, act, reward, max_val, alpha)

        path[state_old] = [act, Qm[position_to_matix_coord(state_old, act)]]

        t += 1.0
        if gamefield.reset:  # reset agent
            update_path(path)  # update the path
            path.clear()  # clear dictionary
            gamefield.reset_agent()
            time.sleep(0.01)
            t = 1.0

        # Update the learning rate
        alpha = pow(t, -0.1)  # series over alpha = infty, series over alpha**2 convergent

        # MODIFY THIS SLEEP IF THE GAME IS GOING TOO FAST.
        # time.sleep(0.01)


t = threading.Thread(target=run)
t.daemon = True
t.start()
gamefield.start_field()