import math
from tools import action as a
from tools import Simulation as S
from tools import math2d as m2d


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(4200, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates


def main():
    state = State()

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    while True:
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(S.simulate_consequences(action, single_consequence, state))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = S.decide_smart(actions_consequences, state)
        # print(action_list[best_action].name)

        S.draw_actions(actions_consequences, state)


if __name__ == "__main__":
    main()
