import os
import pickle
from tools import field_info as field
from compare_decision_schemes.current_impl_goaltime import main as current_impl
from compare_decision_schemes.particle_filter_goaltime import main as particle_filter
from state import State
import timeit
"""
For every position(x, y) and a fixed rotation the time and the number of kicks and turns are calculated for different strategies.
The output is written in two pickle files. The strategy evaluation scripts for a single position are located in
the compare_decision_schemes subfolder. The script takes care not to overwrite previous calculated data.

Example:
    run without any parameters

        $ python compare_decision_schemes.py
"""


def main():
    state = State()
    file_idx = 0

    x_step = 300
    y_step = 300
    fixed_rot = 180

    num_iter = 3  # repeats for current implementation
    timing_container = []
    kick_container = []

    # use this to iterate over the whole green
    field_x_range = range(int(-field.x_field_length * 0.5), int(field.x_field_length * 0.5) + x_step, x_step)
    field_y_range = range(int(-field.y_field_length * 0.5), int(field.y_field_length * 0.5) + y_step, y_step)

    # use this to just iterate over the playing field
    x_range = range(int(-field.x_length * 0.5), int(field.x_length * 0.5) + x_step, x_step)
    y_range = range(int(-field.y_length * 0.5), int(field.y_length * 0.5) + x_step, y_step)

    # run for the whole field
    num_done = 0
    time_mean = 0
    for x in x_range:
        for y in y_range:
            start_time = timeit.default_timer()
        
            c_time, c_kicks, c_turns = current_impl(x, y, fixed_rot, state, num_iter)
            p_time, p_kicks, p_turns = particle_filter(x, y, fixed_rot, state, num_iter)
            print("CurrentImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(fixed_rot) + " " + str(c_time))
            print("ParticleImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(fixed_rot) + " " + str(p_time))

            timing_container.append([x, y, fixed_rot, c_time, p_time])
            kick_container.append([x, y, fixed_rot, c_kicks, c_turns, p_kicks, p_turns])
            
            num_done += 1
            num_todo = len(x_range)*len(y_range) - num_done
            time_mean = 0.5*(time_mean + timeit.default_timer() - start_time)
            print ("compitation time: {0}s ({1}s to go)".format(timeit.default_timer() - start_time, time_mean*num_todo))
            

    # make sure not to overwrite anything
    while (os.path.exists('{}{:d}.pickle'.format('data/strategy_times' + "-rot" + str(fixed_rot) + "-", file_idx)) or
           os.path.exists('{}{:d}.pickle'.format('data/strategy_actions' + "-rot" + str(fixed_rot) + "-", file_idx))):
        file_idx += 1

    pickle.dump(timing_container, open('data/strategy_times' + "-rot" + str(fixed_rot) + "-" + str(file_idx) + '.pickle', "wb"))
    pickle.dump(kick_container, open('data/strategy_actions' + "-rot" + str(fixed_rot) + "-" + str(file_idx) + '.pickle', "wb"))


if __name__ == "__main__":
    main()
