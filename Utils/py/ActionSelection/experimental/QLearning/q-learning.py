import gamefield
import threading
import time

discount = 0.9 # learning rate
actions = gamefield.actions
states = gamefield.field
specials = gamefield.specials
Q = {}


for key in states: # auslagern  in gamefield ??
    if key not in gamefield.specials:

        temp = {}
        # set triangles for every position
        for action in actions:
            temp[action] = 0.
            gamefield.set_cell_color(key, temp[action])
        # initiate the Q matrix - dictionary
        Q[key] = temp

for key in gamefield.specials:
    # get good and bad fields in (specials)
    for action in actions:
        temp[action] = gamefield.specials[key][0]
    # initiate the Q matrix - dictionary
    Q[key] = temp

def do_action(action):
    agent = gamefield.agent_position
    reward = -gamefield.score
    if action == actions[0]: # right
        gamefield.try_move((1, 0))
    elif action == actions[1]: # up
        gamefield.try_move((0, -1))
    elif action == actions[2]: # down
        gamefield.try_move((0, 1))
    elif action == actions[3]: # left (excluded for now)
        gamefield.try_move((-1, 0))
    else:
        return
    s2 = gamefield.agent_position
    reward += gamefield.score
    return agent, action, reward, s2


def max_Q(s):
    val = None
    act = None
    for a, q in Q[s].items():
        if val is None or (q > val):
            val = q
            act = a
    return act, val


def inc_Q(s, a, alpha, inc):
    Q[s][a] *= 1 - alpha
    Q[s][a] += alpha * inc
    gamefield.set_cell_color(s, Q[s][a])



def run():
    global discount
    time.sleep(1)
    alpha = 1
    t = 1
    while True:
        # Pick the right action
        s = gamefield.agent_position
        max_act, max_val = max_Q(s)
        gamefield.update_arrow(s,max_act)
        (s, a, r, s2) = do_action(max_act)

        # Update Q
        max_act, max_val = max_Q(s2)
        inc_Q(s, a, alpha, r + discount * max_val)

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