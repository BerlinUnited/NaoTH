import math
import pickle
from naoth import math2d as m2d
from tools import field_info as field
from current_impl_goaltime import main as current_impl
# from particle_filter_goaltime import main as particle_filter

'''
    using the best angle by goaltime is a terrible idea - since it only differs from our normal implementation in the
    beginning
    TODO: only compare our current implementation with the particle filter one 
'''


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-4000, -700)
        self.pose.rotation = math.radians(-40)

        self.ball_position = m2d.Vector2(100.0, 0.0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def main():
    state = State()
    cell_width = 200
    rotation_step = 30
    num_iter = 1
    blabla = []

    # run for the whole field
    for x in range(int(-field.x_length * 0.5) + cell_width, int(field.x_length * 0.5), 2 * cell_width):
        for y in range(int(-field.y_length * 0.5) + cell_width, int(field.y_length * 0.5), 2 * cell_width):
            for rot in range(0, 360, rotation_step):
                # Todo compare the times for given robot state for different schemes
                # time_particle_filter = particle_filter(x, y, state)
                time_current_impl = current_impl(x, y, state, num_iter)

                blabla.append([x, y, rot, time_current_impl])

    pickle.dump(blabla, open('../data/compare_decision_schemes.pickle', "wb"))

if __name__ == "__main__":
    main()
