from __future__ import division
import math
import sys
import numpy as np
from tools import action as a
from matplotlib.patches import Circle
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from matplotlib import pyplot as plt
from tools import raw_attack_direction_provider as attack_dir

"""
    This file simulates the best angle for a given robot position on the field by simulation all the steps necessary to 
    score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-100, 1800)
        self.pose.rotation = math.radians(0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)


def draw_robot_walk(s, expected_ball_pos, best_action):
    plt.clf()
    axes = plt.gca()
    origin = s.pose.translation
    arrow_head = m2d.Vector2(500, 0).rotate(s.pose.rotation)

    tools.draw_field()
    axes.add_artist(Circle(xy=(s.pose.translation.x, s.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.add_artist(Circle(xy=(expected_ball_pos.x, expected_ball_pos.y), radius=120, fill=True, edgecolor='blue'))
    axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
    axes.text(0, 0, best_action, fontsize=12)

    plt.pause(0.1)


def main(x, y, s, rotation_step):

    start_x = x
    start_y = y
    s.update_pos(m2d.Vector2(start_x, start_y), rotation=0)

    # Only simulate kick_short -> robot_rotation == kick direction and we want to simulate the kick direction - how a kick in this direction is
    # executed doesnt matter.
    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1080, 0, 0, 0)  # Maybe set std again to 150mm

    action_list = [no_action, kick_short]

    repetitions = 1  # TODO make it an argument
    best_times = []
    best_rotations = []

    for reps in range(repetitions):
        pos_total_time = sys.float_info.max
        best_rotation = 0

        for rot in range(0, 360, rotation_step):
            # print("Start Rotation: " + str(rot))
            s.update_pos(m2d.Vector2(x, y), rotation=rot)
            num_kicks = 0
            num_turn_degrees = 0
            goal_scored = False
            choosen_rotation = 'none'  # This is used for deciding the rotation direction once per none decision
            total_time = 0

            while not goal_scored:

                actions_consequences = []
                # Simulate Consequences
                for action in action_list:
                    single_consequence = a.ActionResults([])
                    actions_consequences.append(Sim.simulate_consequences(action, single_consequence, s, a.num_particles))

                # Decide best action
                best_action = Sim.decide_smart(actions_consequences, s)

                # expected_ball_pos should be in local coordinates for rotation calculations
                expected_ball_pos = actions_consequences[best_action].expected_ball_pos

                # Check if expected_ball_pos inside opponent goal
                opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
                opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

                goal_scored = opp_goal_box.inside(s.pose * expected_ball_pos)
                inside_field = field.field_rect.inside(s.pose * expected_ball_pos)
                if goal_scored:
                    # print("Goal " + str(total_time) + " " + str(math.degrees(s.pose.rotation)))
                    break

                elif not inside_field and not goal_scored:
                    # Asserts that expected_ball_pos is inside field or inside opp goal
                    # print("Ball out at x: " + str(s.pose.translation.x) + " y: " + str(s.pose.translation.y) +
                    #      " Rotation: " + str(math.degrees(s.pose.rotation)))
                    total_time = float('nan')  # HACK: the real robot would shoot out
                    break

                elif not action_list[best_action].name == "none":

                    # calculate the time needed
                    rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
                    rotation_time = np.abs(rotation / s.rotation_vel)
                    distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
                    distance_time = distance / s.walking_vel
                    total_time += distance_time + rotation_time

                    # reset the rotation direction
                    choosen_rotation = 'none'

                    # update the robots position
                    s.update_pos(s.pose * expected_ball_pos, math.degrees(s.pose.rotation + rotation))

                    num_kicks += 1

                elif action_list[best_action].name == "none":

                    # Calculate rotation time
                    total_time += np.abs(10 / s.rotation_vel)

                    attack_direction = attack_dir.get_attack_direction(s)
                    attack_direction = math.degrees((attack_direction.angle()))

                    if (attack_direction > 0 and choosen_rotation is 'none') or choosen_rotation is 'left':
                        s.update_pos(s.pose.translation, math.degrees(s.pose.rotation) + 10)  # Should turn right
                        choosen_rotation = 'left'
                    elif (attack_direction <= 0 and choosen_rotation is 'none') or choosen_rotation is 'right':
                        s.update_pos(s.pose.translation, math.degrees(s.pose.rotation) - 10)  # Should turn left
                        choosen_rotation = 'right'
                    else:
                        print("Error at: " + str(s.pose.translation.x) + " - " + str(s.pose.translation.y) + " - " + str(math.degrees(s.pose.rotation)))
                        break

                    num_turn_degrees += 1

                else:
                    sys.exit("There should not be other actions")

                draw_robot_walk(s, s.pose * expected_ball_pos, action_list[best_action].name)

            if pos_total_time > total_time and not np.isnan(total_time):
                pos_total_time = total_time
                best_rotation = rot

        best_times.append(pos_total_time)
        best_rotations.append(best_rotation)

    print("Shortest Time: " + str(np.nanmean(best_times)) + " with global Rotation of robot: " +
          str(np.mean(best_rotations)) + " StartX " + str(start_x) + " Y: " + str(start_y))

    return np.mean(best_times), np.mean(best_rotations)


if __name__ == "__main__":
    state = State()
    rot_step = 30
    main(state.pose.translation.x, state.pose.translation.y, state, rot_step)
