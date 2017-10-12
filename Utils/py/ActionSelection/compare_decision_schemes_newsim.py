import os
import pickle
from tools import field_info as field
from compare_decision_schemes.current_impl_goaltime import main as current_impl
from compare_decision_schemes.particle_filter_goaltime import main as particle_filter
from state import State
from tools import action as a
import timeit
import copy
import numpy as np
from compare_decision_schemes import play_striker as Striker

"""
For every position(x, y) and a fixed rotation the time and the number of kicks and turns are calculated for different strategies.
The output is written in two pickle files. The strategy evaluation scripts for a single position are located in
the compare_decision_schemes subfolder. The script takes care not to overwrite previous calculated data.

Example:
    run without any parameters

        $ python compare_decision_schemes.py
"""


def run_experiment(origin, strategy, action_list):
  
    state = copy.deepcopy(origin)
    simulator = Striker.Simulator(state, strategy, action_list)

    # record the whole state of the simulator
    results = [copy.deepcopy(simulator)]

    iter = 0
    while True:

        simulator.step()

        results += [copy.deepcopy(simulator)]

        if simulator.goal_scored:
            # print ("success")
            break
        elif not simulator.inside_field:
            # print ("failure")
            break
        elif iter > 30:
            print ("WARNING: max iterations reached")
            break

    return results
  

def main():

    x_step = 300
    y_step = 300
    
    # use this to iterate over the whole green
    # field_x_range = range(int(-field.x_field_length * 0.5), int(field.x_field_length * 0.5) + x_step, x_step)
    # field_y_range = range(int(-field.y_field_length * 0.5), int(field.y_field_length * 0.5) + y_step, y_step)

    # use this to just iterate over the playing field
    x_range = range(int(-field.x_length / 2 + x_step/2), int(field.x_length / 2), x_step)
    y_range = range(int(-field.y_length / 2 + y_step/2), int(field.y_length / 2), y_step)
    
    origin = State()
    origin.pose.rotation = np.radians(180)
    origin.pose.translation.x = 0
    origin.pose.translation.y = 0
    
    # set up the available actions
    actions = {
      "none": a.Action("none", 0, 0, 0, 0),
      "kick_short": a.Action("kick_short", 1080, 150, 0, 7),
      "sidekick_left": a.Action("sidekick_left", 750, 150, 90, 10),
      "sidekick_right": a.Action("sidekick_right", 750, 50, -90, 10)
    }
    
    # record the experiment header
    experiment = {
      'x_step': x_step,
      'y_step': y_step,
      'fixed_rot': origin.pose.rotation,
      'x_size': len(x_range),
      'y_size': len(y_range),
      'actions': actions,
      'frames': []
    }

    all_actions = Striker.select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])
    
    # run for the whole field
    num_done = 0
    time_mean = 0
    for ix, x in enumerate(x_range):
        for iy, y in enumerate(y_range):
            start_time = timeit.default_timer()
            
            run = {'x': x, 'y': y, 'ix': ix, 'iy': iy, 'sim': {}}
            
            origin.pose.translation.x = x
            origin.pose.translation.y = y
    
            o_sim = run_experiment(origin, Striker.optimal_kick_strategy, Striker.select(actions, ["none", "kick_short"]))
            run['sim']['optimal_one'] = o_sim
            
            p_sim = run_experiment(origin, Striker.optimal_kick_strategy, all_actions)
            run['sim']['optimal_all'] = p_sim
        
            c_sim = run_experiment(origin, Striker.direct_kick_strategy, all_actions)
            run['sim']['fast'] = c_sim
            
            experiment['frames'] += [run]
            
            num_done += 1
            num_todo = len(x_range)*len(y_range) - num_done
            time_mean = 0.5*(time_mean + timeit.default_timer() - start_time)
            print ("computation time: {0}s ({1}s to go)".format(timeit.default_timer() - start_time, time_mean*num_todo))
            
            break
        break
            
    # make sure not to overwrite anything
    file_idx = 0
    while True:
        file = 'data/simulation_{0}.pickle'.format(file_idx)
        if os.path.exists(file):
            file_idx += 1
        else:
            pickle.dump(experiment, open(file, "wb"))
            break


if __name__ == "__main__":
    main()
