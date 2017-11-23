import os
import pickle
from random import randint
from tools import field_info as field
from state import State
from tools import action as a
import timeit
import copy
import numpy as np
from naoth import math2d as m2d
import play_striker as striker

import multiprocessing as mp

"""
  For a certain amount of random positions on the field the path to the goal is calculated for each
  strategy and written in a pickle file.

Example:
    run without any parameters

        $ python compare_decision_schemes_newsim_random.py
"""

# set up the available actions
actions = {
  "none": a.Action("none", 0, 0, 0, 0),
  "kick_short": a.Action("kick_short", 1080, 150, 0, 7),
  "sidekick_left": a.Action("sidekick_left", 750, 150, 90, 10),
  "sidekick_right": a.Action("sidekick_right", 750, 50, -90, 10)
}

all_actions = striker.select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])

counter = None


def init(args):
    """ store the counter for later use """
    global counter
    counter = args


def wrap_experiment(strategy, actions):
    return lambda origin: striker.run_experiment(origin, strategy, actions)


simulations = {
  'optimal_one': wrap_experiment(striker.optimal_kick_strategy, striker.select(actions, ["none", "kick_short"])),
  'optimal_all': wrap_experiment(striker.optimal_kick_strategy, all_actions),
  'fast': wrap_experiment(striker.direct_kick_strategy_cool, all_actions),
  'fast_best': wrap_experiment(striker.direct_kick_strategy_cool_best, all_actions),
  'optimal_value': wrap_experiment(striker.optimal_value_strategy, all_actions)
}


def make_run((idx, pose)):

    run = {'pose': pose, 'sim': {}}

    origin = State()
    origin.pose = pose
    
    for name, s in simulations.iteritems():
        run['sim'][name] = s(origin)
      
    global counter
    with counter.get_lock():
        counter.value += 1
        print("{0} done".format(counter.value))
  
    return run


def main():

    num_random_pos = 20
    random_x = [randint(int(-field.x_length / 2 + 25), int(field.x_length / 2 - 25)) for p in range(num_random_pos)]
    random_y = [randint(int(-field.y_length / 2 + 25), int(field.y_length / 2 - 25)) for p in range(num_random_pos)]
    random_r = np.random.randint(360, size=num_random_pos)
    random_r = np.radians(random_r)

    # record the experiment header
    experiment = {
      'kind': 'random',
      'actions': actions,
      'frames': []
    }
    
    positions = [(idx, m2d.Pose2D(m2d.Vector2(x, y), r)) for idx, (x, y, r) in enumerate(zip(random_x, random_y, random_r))]
    
    counter = mp.Value('i', 0)

    pool = mp.Pool(initializer=init, initargs=(counter, ), processes=4)
    experiment['frames'] = pool.map(make_run, positions)
    pool.close()
    
    # make sure not to overwrite anything
    file_idx = 0
    while True:
        output_file = 'data/simulation_{0}.pickle'.format(file_idx)
        if os.path.exists(output_file):
            file_idx += 1
        else:
            pickle.dump(experiment, open(output_file, "wb"))
            break


if __name__ == "__main__":
    main()
