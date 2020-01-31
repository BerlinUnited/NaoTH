import math
import numpy as np
import random
import copy
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth.math import *
from tools import tools
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir

"""
Visualization of Action Selection Algorithm until a goal is scored, with extra variations not considered in Action Model
definition.

Example:
    run without any parameters

        $ python simulate_variate_until_goal.py
"""


# TODO update this with all the bugfixes from other scripts
# TODO make it possible to switch between variations and standard


class State:
    def __init__(self):
        self.pose = Pose2D()
        self.pose.translation = Vector2(-4000, -700)
        self.pose.rotation = math.radians(-40)

        self.ball_position = Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

        self.next_action = 0

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def var_kick(kick_type, ball_pos, shorten_kicks=True):
    """
    :param kick_type: integer -- type of kick 0 - none; 1 - kick_short ; 2 - sidekick_left ; 3 - sidekick_right
    :param shorten_kicks: boolean
    :param ball_pos: Vector2 type

    :return: new ball_pos -- Vector2 type
    """
    if kick_type == 1 and shorten_kicks is True:
        random_number = np.random.randint(0, 3, 1)  # zu 25 % kurzer Schuss
        if random_number == 0:
            direction = ball_pos.normalize()
            rotation_degrees = random.gauss(0,
                                            20)  # verschobene neue Richtung des Balls mit mittels Normalverteilung N(0,20)
            direction = direction.rotate(math.radians(rotation_degrees))
            direction *= 400
            return direction
        else:
            return ball_pos
    elif kick_type in [2, 3] and shorten_kicks is True:
        random_number = np.random.randint(0, 2, 1)  # zu 33% kurzer Seitschuss
        if random_number == 0:
            # gleicht dem verkuerzen und verschieben des short kicks
            # TODO: bessere / realistischere Verschiebenungen
            direction = ball_pos.normalize()
            direction = direction.normalize()
            direction *= 500
            return direction
        else:
            return ball_pos
    else:
        return ball_pos


def draw_robot_walk_lines(line):
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    count = 0
    action_name_list = ["none", "short", "left", "right"]
    for state in line:
        origin = state.pose.translation
        ball_pos = state.pose * state.ball_position

        arrow_head = Vector2(500, 0).rotate(state.pose.rotation)

        axes.add_artist(Circle(xy=(origin.x, origin.y), radius=100, fill=False, edgecolor='white'))
        axes.add_artist(
            Circle(xy=(ball_pos.x, ball_pos.y), radius=120, fill=True, edgecolor='blue'))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100,
                   fc='k', ec='k')
        axes.text(origin.x, origin.y - 350, count, fontsize=8)
        axes.text(origin.x, 3500 - (count % 4) * 250,
                  action_name_list[state.next_action] + " " + str(count), fontsize=8)
        count += 1
        # -- Add counter for moves
        # -- Add executed action
        # -- prove Ball position ist correct
        # -- Haeufungspunkte (z.B. rotieren um Ball) Zahlen verbessern - mehr Uebersichtlichkeit

    plt.show()


def simulate_goal_cycle(variation=False):
    state = State()
    sim_data = [copy.deepcopy(state)]

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    # Todo: Maybe do several decision cycles not just one to get rid of accidental decisions
    num_kicks = 0
    num_turn_degrees = 0
    goal_scored = False

    while not goal_scored:

        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(
                Sim.simulate_consequences(action, single_consequence, state, a.num_particles))

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        # state.next_action = best_action #not next but last action
        sim_data[len(sim_data) - 1].next_action = best_action

        # expected_ball_pos should be in local coordinates for rotation calculations
        expected_ball_pos = actions_consequences[best_action].expected_ball_pos_mean

        if variation is True:
            expected_ball_pos = var_kick(best_action, expected_ball_pos)

        # Check if expected_ball_pos inside opponent goal
        opp_goal_back_right = Vector2(field.opponent_goalpost_right.x + field.goal_depth,
                                      field.opponent_goalpost_right.y)
        opp_goal_box = Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

        goal_scored = opp_goal_box.inside(state.pose * expected_ball_pos)
        inside_field = field.field_rect.inside(state.pose * expected_ball_pos)

        # Assert that expected_ball_pos is inside field or inside opp goal
        if not inside_field and not goal_scored:
            sim_data.append(copy.deepcopy(state))
            print("Error")
            # For histogram -> note the this position doesnt manage a goal
            break

        if not action_list[best_action].name == "none":

            print(str(state.pose * expected_ball_pos) + " Decision: " + str(
                action_list[best_action].name))

            # update the robots position
            rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
            print(math.degrees(rotation))
            state.update_pos(state.pose * expected_ball_pos, state.pose.rotation + rotation)
            sim_data.append(copy.deepcopy(state))

            num_kicks += 1

        elif action_list[best_action].name == "none":
            print(str(state.pose * expected_ball_pos) + " Decision: " + str(
                action_list[best_action].name))

            attack_direction = attack_dir.get_attack_direction(state)
            # Todo: can run in a deadlock for some reason
            if attack_direction > 0:
                state.update_pos(state.pose.translation,
                                 state.pose.rotation + math.radians(10))  # Should be turn right
                # state.pose.rotation += math.radians(10)  # Should be turn right
                print("Robot turns right - global rotation turns left")

            else:
                state.update_pos(state.pose.translation,
                                 state.pose.rotation - math.radians(10))  # Should be turn left
                # state.pose.rotation -= math.radians(10)  # Should be turn left
                print("Robot turns left - global rotation turns right")

            sim_data.append(copy.deepcopy(state))

            num_turn_degrees += 1

    # print("Num Kicks: " + str(num_kicks))
    # print("Num Turns: " + str(num_turn_degrees))

    return sim_data


def main():
    variation = True
    ball_line = simulate_goal_cycle(variation)
    draw_robot_walk_lines(ball_line)


if __name__ == "__main__":
    main()
