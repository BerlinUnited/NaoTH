import math
import os
import pickle
from naoth import math2d as m2d
from tools import field_info as field
from compare_decision_schemes.current_impl_goaltime import main as current_impl
from compare_decision_schemes.particle_filter_goaltime import main as particle_filter

'''
    TODO: only compare our current implementation with the particle filter one 
'''


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(0, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def main():
    state = State()
    file_idx = 0

    cell_width = 200
    rotation_step = 30

    num_iter = 10  # repeats for current implementation
    timing_container = []
    kick_container = []

    x_range = range(int(-field.x_length * 0.5) + cell_width, int(field.x_length * 0.5), 2 * cell_width)
    y_range = range(int(-field.y_length * 0.5) + cell_width, int(field.y_length * 0.5), 2 * cell_width)
    rot_range = range(0, 360, rotation_step)

    x_range = range(-800, 800, 200)
    y_range = range(-800, 800, 200)
    rot_range = range(0, 360, 360)

    # run for the whole field
    for x in x_range:
        for y in y_range:
            for rot in rot_range:
                c_time, c_kicks, c_turns = current_impl(x, y, rot, state, num_iter)
                p_time, p_kicks, p_turns = particle_filter(x, y, rot, state, num_iter)
                print("CurrentImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(rot) + " " + str(c_time))
                print("ParticleImpl: " + " X: " + str(x) + " Y: " + str(y) + " rot: " + str(rot) + " " + str(p_time))

                timing_container.append([x, y, rot, c_time, p_time])
                kick_container.append([x, y, rot, c_kicks, c_turns, p_kicks, p_turns])

    # make sure not to overwrite anything
    while (os.path.exists('{}{:d}.pickle'.format('data/strategy_times', file_idx)) or
           os.path.exists('{}{:d}.pickle'.format('data/strategy_actions', file_idx))):
        file_idx += 1
    pickle.dump(timing_container, open('data/strategy_times' + str(file_idx) + '.pickle', "wb"))
    pickle.dump(kick_container, open('data/strategy_actions' + str(file_idx) + '.pickle', "wb"))


if __name__ == "__main__":
    main()
