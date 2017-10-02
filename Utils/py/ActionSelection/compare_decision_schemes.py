import os
import pickle
from tools import field_info as field
from compare_decision_schemes.current_impl_goaltime import main as current_impl
from compare_decision_schemes.particle_filter_goaltime import main as particle_filter
from state import State

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

    cell_width = 200

    num_iter = 10  # repeats for current implementation
    timing_container = []
    kick_container = []

    x_range = range(int(-field.x_length * 0.5) + cell_width, int(field.x_length * 0.5), 2 * cell_width)
    y_range = range(int(-field.y_length * 0.5) + cell_width, int(field.y_length * 0.5), 2 * cell_width)
    rot = 45

    # run for the whole field
    for x in x_range:
        for y in y_range:
            c_time, c_kicks, c_turns = current_impl(x, y, rot, state, num_iter)
            p_time, p_kicks, p_turns = particle_filter(x, y, rot, state, num_iter)
            print("CurrentImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(rot) + " " + str(c_time))
            print("ParticleImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(rot) + " " + str(p_time))

            timing_container.append([x, y, rot, c_time, p_time])
            kick_container.append([x, y, rot, c_kicks, c_turns, p_kicks, p_turns])

    # make sure not to overwrite anything
    while (os.path.exists('{}{:d}.pickle'.format('data/strategy_times' + "-rot" + str(rot) + "-", file_idx)) or
           os.path.exists('{}{:d}.pickle'.format('data/strategy_actions' + "-rot" + str(rot) + "-", file_idx))):
        file_idx += 1
    pickle.dump(timing_container, open('data/strategy_times' + "-rot" + str(rot) + "-" + str(file_idx) + '.pickle', "wb"))
    pickle.dump(kick_container, open('data/strategy_actions' + "-rot" + str(rot) + "-" + str(file_idx) + '.pickle', "wb"))


if __name__ == "__main__":
    main()
