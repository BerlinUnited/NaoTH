import math
import sys
import numpy as np
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir
"""
    This file simulates the best angle for a given robot position on the field by simulation all the steps necessary to 
    score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(2000, 0)
        self.pose.rotation = math.radians(0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)


def main(x, y, s, rotation_step):

    start_x = x
    start_y = y
    s.update_pos(m2d.Vector2(start_x, start_y), rotation=0)

    # Only simulate kick_short -> robot_rotation == kick direction and we want to simulate the kick direction - how a kick in this direction is
    # executed doesnt matter.
    kick_short = a.Action("kick_short", 1080, 150, 0, 0)

    action_list = [kick_short]

    repetitions = 10
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
            max_turn_credit = 360  # FIXME

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

                if max_turn_credit <= 0:
                    print("x: " + str(x) + " y: " + str(y) + " - Credit: " + str(max_turn_credit))
                    a.histogram[50-1] += 1  # means oscillation or that 36 times turning by rot degrees wasn't enough
                    break

                # Assert that expected_ball_pos is inside field or inside opp goal
                # HACK: the real robot would shoot out
                elif not inside_field and not goal_scored:

                    # print("Ball out at x: " + str(s.pose.translation.x) + " y: " + str(s.pose.translation.y) + " Rotation: " + str(s.pose.rotation))
                    total_time += 10000  # TODO: set to Nan and dont use for mean calculation
                    a.histogram[0] += 1
                    # TODO indicate in time 2D Plot that this position leads to a ball out
                    break

                elif not action_list[best_action].name == "none":
                    max_turn_credit = 360

                    # calculate the time needed
                    rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
                    rotation_time = np.abs(rotation / s.rotation_vel)
                    distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
                    distance_time = distance / s.walking_vel
                    total_time += distance_time + rotation_time

                    # update the robots position
                    s.update_pos(s.pose * expected_ball_pos, s.pose.rotation + rotation)

                    num_kicks += 1

                elif action_list[best_action].name == "none":

                    # Calculate rotation time
                    total_time += np.abs(math.radians(10) / s.rotation_vel)

                    attack_direction = attack_dir.get_attack_direction(s)
                    attack_direction = math.degrees((attack_direction.angle()))
                    # Can run in a deadlock for some reason - Hack: abort if max_turn_credit is 0
                    if attack_direction > 0:
                        s.update_pos(s.pose.translation, s.pose.rotation + math.radians(15))  # Should be turn right
                        max_turn_credit -= 10
                    else:
                        s.update_pos(s.pose.translation, s.pose.rotation - math.radians(15))  # Should be turn left
                        max_turn_credit -= 10

                    num_turn_degrees += 1
                else:
                    print("HUGE ERROR")
                    break

            # print(total_time)
            if pos_total_time > total_time:
                pos_total_time = total_time
                best_rotation = rot

        best_times.append(pos_total_time)
        best_rotations.append(best_rotation)

    print("Shortest Time: " + str(np.mean(best_times)) + " with global Rotation of robot: " + str(np.mean(best_rotations)) + " StartX " + str(start_x))

    return np.mean(best_times), np.mean(best_rotations)


if __name__ == "__main__":
    state = State()
    rot_step = 10
    main(state.pose.translation.x, state.pose.translation.y, state, rot_step)
