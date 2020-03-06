##########################
#
# Actions used by world (with noise) and agent (without noise)
#
#
#
##########################

import math

from tools import action as act
from state import State


class Actions:
    """
    all available actions

    position and rotation are m2d Vector2 object ( 2-d objects )
    """
    number_of_actions = 5

    def __init__(self, add_noise=True):
        self.add_noise = add_noise
        self.short = act.Action("kick_short", 1080, 150, 0, 7)
        self.left = act.Action("sidekick_left", 750, 150, 90, 10)
        self.right = act.Action("sidekick_right", 750, 150, -90, 10)

    def get_actions(self):
        return [self.kick_short, self.sidekick_left, self.sidekick_right, self.turn_left_45,
                self.turn_right_45]

    def kick_short(self, state):
        state.ball_position = self.short.predict(state.ball_position,
                                                 self.add_noise)  # noise included

    def sidekick_left(self, state):
        state.ball_position = self.left.predict(state.ball_position,
                                                self.add_noise)  # noise included

    def sidekick_right(self, state):
        state.ball_position = self.right.predict(state.ball_position,
                                                 self.add_noise)  # noise included

    def turn_left_45(self, state):
        # TODO: noise could be added here
        # update state rotation, increment by 45 degrees
        new_direction = state.direction.rotate(math.pi / 4)
        state.update_pos(direction=new_direction)
        # print math.degrees(state.pose.rotation)

    def turn_right_45(self, state):
        # update state rotation, decrement by 45 degrees
        new_direction = state.direction.rotate(-math.pi / 4)
        state.update_pos(direction=new_direction)
        # print math.degrees(state.pose.rotation)


if __name__ == "__main__":
    actions = Actions(add_noise=False)
    state = State()
    for action in actions.get_actions():
        action(state)
        print
        action
        print
        state.ball_position
        print
        state.position
        print
        state.direction
