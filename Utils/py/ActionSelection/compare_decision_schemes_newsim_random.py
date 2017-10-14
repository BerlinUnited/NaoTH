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
from compare_decision_schemes import play_striker as striker

import multiprocessing as mp
import time

"""
For every position(x, y) and a fixed rotation the time and the number of kicks and turns are calculated for different strategies.
The output is written in two pickle files. The strategy evaluation scripts for a single position are located in
the compare_decision_schemes sub folder. The script takes care not to overwrite previous calculated data.

Example:
    run without any parameters

        $ python compare_decision_schemes.py
"""

# set up the available actions
actions = {
  "none": a.Action("none", 0, 0, 0, 0),
  "kick_short": a.Action("kick_short", 1080, 150, 0, 7),
  "sidekick_left": a.Action("sidekick_left", 750, 150, 90, 10),
  "sidekick_right": a.Action("sidekick_right", 750, 50, -90, 10)
}

all_actions = striker.select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])


def make_run(pose):
      run = {'pose': pose, 'sim': {}}

      origin = State()
      origin.pose = pose

      # make an alias
      re = striker.run_experiment
      print ('optimal_one')
      run['sim']['optimal_one'] = re(origin, striker.optimal_kick_strategy, striker.select(actions, ["none", "kick_short"]))
      print ('optimal_all')
      run['sim']['optimal_all'] = re(origin, striker.optimal_kick_strategy, all_actions)
      print ('fast')
      run['sim']['fast'] = re(origin, striker.direct_kick_strategy, all_actions)
      print ('optimal_value')
      run['sim']['optimal_value'] = re(origin, striker.optimal_value_strategy, all_actions)
    
      return run

def main():

    num_random_pos = 10
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
    
    positions = [m2d.Pose2D(m2d.Vector2(x,y),r) for (x,y,r) in zip(random_x,random_y,random_r)]
    
    pool = mp.Pool(processes=4)
    result = pool.map_async(make_run, positions)
    
    while not result.ready():
        print("num left: {}".format(result._number_left))
        time.sleep(1)
    
    experiment['frames'] = result.get()
    
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
