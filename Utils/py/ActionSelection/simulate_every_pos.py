import math
from tools import action as a
from tools import Simulation as S
from tools import math2d as m2d
from tools import field_info as field


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(4200, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def main():
    state = State()

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    pos_list = []
    pos_list_labeled = []

    # xrange is only slightly faster in python 2.7
    for x in range(int(-field.x_field_length*0.5), int(field.x_field_length*0.5), 50):
        for y in range(int(-field.y_field_length*0.5), int(field.y_field_length*0.5), 50):
            pos_list.append(m2d.Vector2(x, y))

    for pos in pos_list:
        # the rotation should be fixed for all pos
        state.update_pos(pos, rotation=0)
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(S.simulate_consequences(action, single_consequence, state))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = S.decide_smart(actions_consequences, state)
        pos_list_labeled.append([state.pose.translation, action_list[best_action].name])
        # print(str(state.pose.translation) + " - " + str(action_list[best_action].name))

    # Todo do some statistics
    # Todo find a good solution for not always recomputing the whole list or faster compuation
    # print(pos_list_labeled)

if __name__ == "__main__":
    main()
