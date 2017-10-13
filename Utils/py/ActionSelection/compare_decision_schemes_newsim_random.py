import os
import pickle
from random import randint
from tools import field_info as field
from state import State
from tools import action as a
import timeit
import copy
import numpy as np
from compare_decision_schemes import play_striker as striker

"""
For every position(x, y) and a fixed rotation the time and the number of kicks and turns are calculated for different strategies.
The output is written in two pickle files. The strategy evaluation scripts for a single position are located in
the compare_decision_schemes sub folder. The script takes care not to overwrite previous calculated data.

Example:
    run without any parameters

        $ python compare_decision_schemes.py
"""

def main():

    x_step = 300
    y_step = 300
    
    # use this to iterate over the whole green
    # field_x_range = range(int(-field.x_field_length * 0.5), int(field.x_field_length * 0.5) + x_step, x_step)
    # field_y_range = range(int(-field.y_field_length * 0.5), int(field.y_field_length * 0.5) + y_step, y_step)

    # use this to just iterate over the playing field
    #x_range = range(int(-field.x_length / 2 + x_step/2), int(field.x_length / 2), x_step)
    #y_range = range(int(-field.y_length / 2 + y_step/2), int(field.y_length / 2), y_step)
    
    origin = State()
    origin.pose.rotation = np.radians([180])
    origin.pose.translation.x = 0
    origin.pose.translation.y = 0
    
    # set up the available actions
    actions = {
      "none": a.Action("none", 0, 0, 0, 0),
      "kick_short": a.Action("kick_short", 1080, 150, 0, 7),
      "sidekick_left": a.Action("sidekick_left", 750, 150, 90, 10),
      "sidekick_right": a.Action("sidekick_right", 750, 50, -90, 10)
    }
    
    all_actions = striker.select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])
    
    # run for the whole field
    num_done = 0
    time_mean = 0

    num_random_pos = 10000
    random_x = [randint(int(-field.x_length / 2 + 25), int(field.x_length / 2 - 25)) for p in range(num_random_pos)]
    random_y = [randint(int(-field.y_length / 2 + 25), int(field.y_length / 2 - 25)) for p in range(num_random_pos)]
    random_r = np.random.randint(360, size=num_random_pos)
    random_r = np.radians(random_r)

    # record the experiment header
    experiment = {
      'kind': 'random',
      #'x_step': x_step,
      #'y_step': y_step,
      #'fixed_rot': origin.pose.rotation,
      #'x_size': len(x_range),
      #'y_size': len(y_range),
      'actions': actions,
      'frames': []
    }
    
    for idx, pos in enumerate(random_x):
        start_time = timeit.default_timer()

        run = {'x': random_x[idx], 'y': random_y[idx], 'ix': idx, 'iy': idx, 'sim': {}}

        origin.pose.translation.x = random_x[idx]
        origin.pose.translation.y = random_y[idx]
        origin.pose.rotation = random_r[idx]

        o_sim = striker.run_experiment(origin, striker.optimal_kick_strategy, striker.select(actions, ["none", "kick_short"]))
        run['sim']['optimal_one'] = o_sim

        p_sim = striker.run_experiment(origin, striker.optimal_kick_strategy, all_actions)
        run['sim']['optimal_all'] = p_sim

        c_sim = striker.run_experiment(origin, striker.direct_kick_strategy, all_actions)
        run['sim']['fast'] = c_sim
        
        c_sim = striker.run_experiment(origin, striker.optimal_value_strategy, all_actions)
        run['sim']['optimal_value'] = c_sim

        experiment['frames'] += [run]

        num_done += 1
        num_todo = len(random_x) - num_done
        time_mean = 0.5*(time_mean + timeit.default_timer() - start_time)
        print ("computation time: {0}s ({1}s to go)".format(timeit.default_timer() - start_time, time_mean*num_todo))
            
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
