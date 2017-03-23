import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-4000, -700)
        self.pose.rotation = math.radians(-40)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    # Todo use this everywhere
    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def draw_robot_walk(actions_consequences, state, expected_ball_pos, best_action):
    plt.clf()
    axes = plt.gca()
    origin = state.pose.translation
    arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation)
    x = np.array([])
    y = np.array([])

    tools.draw_field()
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.add_artist(Circle(xy=(expected_ball_pos.x, expected_ball_pos.y), radius=120, fill=True, edgecolor='blue'))
    axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
    axes.text(0, 0, best_action, fontsize=12)
    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.5)


def main():
    state = State()

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    # Todo: Maybe do several decision cycles(histogram of decisions) not just one to get rid of accidental decisions
    num_kicks = 0
    num_turn_degrees = 0
    goal_scored = False
    while not goal_scored:

        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state))

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        # expected_ball_pos should be in local coordinates for rotation calculations
        expected_ball_pos = actions_consequences[best_action].expected_ball_pos

        # Check if expected_ball_pos inside opponent goal
        opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
        opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

        goal_scored = opp_goal_box.inside(state.pose * expected_ball_pos)
        inside_field = field.field_rect.inside(state.pose * expected_ball_pos)

        # Assert that expected_ball_pos is inside field or inside opp goal
        if not inside_field and not goal_scored:
            print("Error")
            # For histogram -> note the this position doesnt manage a goal
            break

        if not action_list[best_action].name == "none":

            print(str(state.pose * expected_ball_pos) + " Decision: " + str(action_list[best_action].name))
            draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)

            # update the robots position
            rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
            print(math.degrees(rotation))
            state.pose.translation = state.pose * expected_ball_pos
            state.pose.rotation += rotation

            num_kicks += 1

        elif action_list[best_action].name == "none":
            print(str(state.pose * expected_ball_pos) + " Decision: " + str(action_list[best_action].name))
            draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)

            attack_direction = attack_dir.get_attack_direction(state)
            # Todo: can run in a deadlock for some reason
            if attack_direction > 0:
                state.pose.rotation += math.radians(10)  # Should be turn right
                print("Robot turns right - global rotation turns left")
            else:
                state.pose.rotation -= math.radians(10)  # Should be turn left
                print("Robot turns left - global rotation turns right")

            num_turn_degrees += 1

    print("Num Kicks: " + str(num_kicks))
    print("Num Turns: " + str(num_turn_degrees))

if __name__ == "__main__":
    '''
    Should Simulate the real robot behavior, that means turn incrementally
    -> i dont account for difference in speed of cognition and motion
    Todo: make a histogram

    Histogram:
    Pos.x | Pos.y | Action None | kick short | kick left | kick right
    '''
    main()
