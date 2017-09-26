from __future__ import division
import sys
import math as m
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir


"""
TODO introduce DEBUG LEVEL -> maybe use logging module
     need to easily enable/disable output but also drawing
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-1000, -1000)
        self.pose.rotation = m.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = m.radians(rotation)


def draw_robot_walk(actions_consequences, s, expected_ball_pos, best_action):
    plt.clf()
    axes = plt.gca()
    origin = s.pose.translation
    arrow_head = m2d.Vector2(500, 0).rotate(s.pose.rotation)
    x = np.array([])
    y = np.array([])

    tools.draw_field(axes)
    axes.add_artist(Circle(xy=(s.pose.translation.x, s.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.add_artist(Circle(xy=(expected_ball_pos.x, expected_ball_pos.y), radius=120, fill=True, edgecolor='blue'))
    axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
    axes.text(-4500, 3150, best_action, fontsize=12)
    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = s.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.5)


def main(x, y, rot, s, num_iter):
    s.update_pos(m2d.Vector2(x, y), rotation=rot)

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1280, 0, 0, 0)
    sidekick_left = a.Action("sidekick_left", 750, 0, 90, 0)
    sidekick_right = a.Action("sidekick_right", 750, 0, -90, 0)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    # Do several decision cycles not just one to get rid of accidental decisions
    timings = []
    n_kicks = []
    n_turns = []
    for idx in range(num_iter):
        num_kicks = 0
        num_turn_degrees = 0
        goal_scored = False
        total_time = 0
        choosen_rotation = 'none'  # This is used for deciding the rotation direction once per none decision
        s.update_pos(m2d.Vector2(x, y), rotation=rot)
        while not goal_scored:
            actions_consequences = []
            # Simulate Consequences
            for action in action_list:
                single_consequence = a.ActionResults([])
                actions_consequences.append(Sim.simulate_consequences(action, single_consequence, s, a.num_particles))

            # Decide best action
            best_action = Sim.decide_smart(actions_consequences, s)

            # expected_ball_pos should be in local coordinates for rotation calculations
            expected_ball_pos = actions_consequences[best_action].expected_ball_pos_mean

            # Check if expected_ball_pos inside opponent goal
            opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
            opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

            goal_scored = opp_goal_box.inside(s.pose * expected_ball_pos)
            inside_field = field.field_rect.inside(s.pose * expected_ball_pos)
            if goal_scored:
                num_kicks += 1
                # print("Goal " + str(total_time) + " " + str(math.degrees(s.pose.rotation)))
                break

            elif not inside_field and not goal_scored:
                # Asserts that expected_ball_pos is inside field or inside opp goal
                # print("Error: This position doesn't manage a goal")
                total_time = float('nan')
                break

            elif not action_list[best_action].name == "none":

                # draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)

                # calculate the time needed
                rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
                rotation_time = np.abs(m.degrees(rotation) / s.rotation_vel)
                distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
                distance_time = distance / s.walking_vel
                total_time += distance_time + rotation_time

                # update total turns
                num_turn_degrees += np.abs(m.degrees(rotation))

                # reset the rotation direction
                choosen_rotation = 'none'

                # update the robots position
                s.update_pos(s.pose * expected_ball_pos, m.degrees(s.pose.rotation + rotation))
                num_kicks += 1

            elif action_list[best_action].name == "none":
                # print(str(state.pose * expected_ball_pos) + " Decision: " + str(action_list[best_action].name))
                # draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)
                turn_rotation_step = 5
                # Calculate rotation time
                total_time += np.abs(turn_rotation_step / s.rotation_vel)

                attack_direction = attack_dir.get_attack_direction(s)

                if (attack_direction > 0 and choosen_rotation) is 'none' or choosen_rotation is 'left':
                    s.update_pos(s.pose.translation, m.degrees(s.pose.rotation) + turn_rotation_step)  # Should turn right
                    choosen_rotation = 'left'
                elif (attack_direction <= 0 and choosen_rotation is 'none') or choosen_rotation is 'right':
                    s.update_pos(s.pose.translation, m.degrees(s.pose.rotation) - turn_rotation_step)  # Should turn left
                    choosen_rotation = 'right'

                num_turn_degrees += turn_rotation_step
            else:
                sys.exit("There should not be other actions")
        # print("Total time to goal: " + str(total_time))
        # print("Num Kicks: " + str(num_kicks))
        # print("Num Turns: " + str(num_turn_degrees))

        timings.append(total_time)
        n_kicks.append(num_kicks)
        n_turns.append(num_turn_degrees)

    return np.nanmean(timings), np.nanmean(n_kicks), np.nanmean(n_turns)


if __name__ == "__main__":
    # repeat it multiple times to see how reliable the result is
    for i in range(30):
        state = State()
        time, kicks, turns = main(state.pose.translation.x, state.pose.translation.y, m.degrees(state.pose.rotation), state, num_iter=10)
        print("Total time to goal: " + str(time) + " #Kicks: " + str(kicks) + " #Turns: " + str(turns))
