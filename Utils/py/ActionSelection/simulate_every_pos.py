from __future__ import print_function  # needed for unpacking elements of a list for printing
import argparse
import pickle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import field_info as field
from state import State

"""
For every position(x, y, rot) the decision is calculated r times with s samples. This script runs
for a long time. It is recommended to run each instance with a seperate screen. The results are written
in a data subfolder (make sure it exists before) as pickle file. An example for analyzing this data is
located in evaluation/evaluate_every_pos.py


For killing all screens: 
    screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill

Example:
    for using the 12 samples and 100 repetitions run:

        $ python simulate_every_pos.py -s 12 -r 100
"""


def main(num_samples, num_reps, x_step, y_step, rotation_step):
    # This takes hours
    state = State()

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    whole_decisions = []

    for rot in range(0, 360, rotation_step):
        for x in range(int(-field.x_field_length*0.5)+x_step, int(field.x_field_length*0.5), x_step):
            for y in range(int(-field.y_field_length*0.5)+y_step, int(field.y_field_length*0.5), y_step):
                state.update_pos(m2d.Vector2(x, y), rotation=rot)
                # Do this multiple times and write the decisions as a histogram
                decision_histogramm = [0, 0, 0, 0]  # ordinal scale -> define own metric in evaluation script
                for num_simulations in range(0, num_reps):
                    actions_consequences = []
                    # Simulate Consequences
                    for action in action_list:
                        single_consequence = a.ActionResults([])
                        actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, num_samples))

                    # Decide best action
                    best_action = Sim.decide_smart(actions_consequences, state)
                    decision_histogramm[best_action] += 1

                # write the position and decision in on list
                whole_decisions.append([x, y, rot, decision_histogramm])

    pickle.dump(whole_decisions, open('data/decision-simulate_every_pos-' + str(num_samples) + '-' + str(num_reps) + '.pickle', "wb"))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Calculates a histogram of decisions for each position on the field',
    )
    parser.add_argument("-s", "--num_samples", help="input the number of samples used for on action", type=int, default=30)
    parser.add_argument("-r", "--num_reps", help="input the number of repeats per position", type=int, default=1)
    parser.add_argument("-x", "--res_x", help="input the step size for rotation", type=int, default=200)
    parser.add_argument("-y", "--res_y", help="input the step size for rotation", type=int, default=200)
    parser.add_argument("-rot", "--res_rot", help="input the step size for rotation", type=int, default=30)

    args = parser.parse_args()

    main(args.num_samples, args.num_reps, args.res_x, args.res_y, args.res_rot)
