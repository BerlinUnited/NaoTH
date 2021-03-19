##########################
#
# Actions used by world (with noise) and agent (without noise)
#
##########################

import math
import copy
import numpy as np

from tools import action as act
from state import State

from naoth.math2d import Vector2 as Vec


class Actions:
    # TODO: cleanup
    """
    all available actions

    position and rotation are m2d Vector2 object ( 2-d objects )

    all actions manipulate the state object thus there is no return
    """
    action_space = range(0, 5)
    add_noise = True

    short = act.Action("kick_short", 1080, 150, 0, 7)
    left = act.Action("sidekick_left", 750, 150, 90, 10)
    right = act.Action("sidekick_right", 750, 150, -90, 10)

    def short_with_degree(self, degree):
        return act.Action("kick_short", 1080, 150, degree, 7)  # degree in degrees

    def left_with_degree(self, degree):
        return act.Action("sidekick_left", 750, 150, degree + 90, 10)

    def right_with_degree(self, degree):
        return act.Action("sidekick_right", 750, 150, degree - 90, 10)

    def __init__(self, add_noise=True):
        self.add_noise = add_noise
        self.short = act.Action("kick_short", 1080, 150, 0, 7)
        self.left = act.Action("sidekick_left", 750, 150, 90, 10)
        self.right = act.Action("sidekick_right", 750, 150, -90, 10)

        self.short_with_degree = lambda degree: act.Action("kick_short", 1080, 150, degree,
                                                           7)  # degree in degrees
        self.left_with_degree = lambda degree: act.Action("sidekick_left", 750, 150, degree + 90,
                                                          10)
        self.right_with_degree = lambda degree: act.Action("sidekick_right", 750, 150, degree - 90,
                                                           10)

    def kick_short(self, state):
        state.ball_position = self.short.predict(state.ball_position, self.add_noise)

    def sidekick_left(self, state):
        state.ball_position = self.left.predict(state.ball_position, self.add_noise)

    def sidekick_right(self, state):
        state.ball_position = self.right.predict(state.ball_position, self.add_noise)

    def get_actions_degree(self,
                           rad):  # actions with current degree TODO: implement / add in simulation
        #########
        #
        # all actions are functions which expect a state input ( State object )
        #
        #########

        degree = np.degrees(rad) % 360  # get degrees from radian in range 0 to 359.9

        actions = \
            [lambda eval_state: self.kick_short_degree(eval_state, degree),
             lambda eval_state: self.sidekick_left_degree(eval_state, degree),
             lambda eval_state: self.sidekick_right_degree(eval_state, degree),
             self.turn_left_45,
             self.turn_right_45]

        names = ["kick_short,",
                 "sidekick_left",
                 "sidekick_right",
                 "turn_left_45",
                 "turn_right_45"]

        return actions, names

    def get_actions_degree_dict(self, rad):
        """
        How to use:

        use the get_actions_degree_dict to get an dictionary with the available actions of the agent with a rotation of
        'rad' radians.

        the dictionary includes functions, which expect a state object ( see state.py ) to evaluate the action

        """

        degree = np.degrees(rad) % 360  # get degrees from radian in range 0 to 359.9

        return {
            "kick_short_degree": lambda eval_state: self.kick_short_degree(eval_state, degree),
            "sidekick_left_degree": lambda eval_state: self.sidekick_left_degree(eval_state,
                                                                                 degree),
            "sidekick_right_degree": lambda eval_state: self.sidekick_right_degree(eval_state,
                                                                                   degree),
            "turn_left_45": self.turn_left_45,
            "turn_right_45": self.turn_right_45
        }

    def kick_short_degree(self, eval_state, degree):
        eval_state.ball_position = self.short_with_degree(degree).predict(eval_state.ball_position,
                                                                          self.add_noise)

    def sidekick_left_degree(self, eval_state, degree):
        eval_state.ball_position = self.left_with_degree(degree).predict(eval_state.ball_position,
                                                                         self.add_noise)

    def sidekick_right_degree(self, eval_state, degree):
        eval_state.ball_position = self.right_with_degree(degree).predict(eval_state.ball_position,
                                                                          self.add_noise)

    def turn_left_45(self, eval_state):
        # TODO: noise could be added here
        # update state rotation, increment by 45 degrees
        new_direction = eval_state.direction.rotate(math.pi / 4)
        eval_state.update_pos(direction=new_direction)
        # print math.degrees(state.pose.rotation)

    def turn_right_45(self, eval_state):
        # update state rotation, decrement by 45 degrees
        new_direction = eval_state.direction.rotate(-math.pi / 4)
        eval_state.update_pos(direction=new_direction)
        # print math.degrees(state.pose.rotation)

    def turn_right_angle(self, eval_state, angle):
        pass

    def turn_left_angle(self, eval_state, angle):
        pass

    def get_actions_from_state(self, eval_state):
        return self.get_actions_degree(eval_state.direction.angle())


class Actions_2:
    action_space = range(0, 5)

    def __init__(self, add_noise=True):
        self.add_noise = add_noise
        self.short_with_degree = lambda degree: act.Action("kick_short", 1080, 150, degree,
                                                           7)  # degree in degrees

    def kick_short_degree(self, eval_state, degree):
        kick = self.short_with_degree(degree)
        eval_state.ball_position = kick.predict(eval_state.ball_position, self.add_noise)

    def get_actions_degree(self, rad):  # actions with current degree

        #########
        #
        #   all actions are functions which expect a state input ( State object )
        #
        #########

        degree = np.degrees(rad) % 360  # get degrees from radian in range 0 to 359.9

        actions = \
            [lambda eval_state: self.kick_short_degree(eval_state, degree),
             lambda eval_state: self.kick_short_degree(eval_state, degree + 30),
             lambda eval_state: self.kick_short_degree(eval_state, degree - 30),
             lambda eval_state: self.kick_short_degree(eval_state, degree + 135),
             lambda eval_state: self.kick_short_degree(eval_state, degree - 135)]

        names = ["kick_short",
                 "kick_short_plus_30",
                 "kick_short_minus_30",
                 "kick_short_plus_60",
                 "kick_short_minus_60"]
        return actions, names

    def get_actions_from_state(self, eval_state):
        return self.get_actions_degree(eval_state.direction.angle())


class Actions_3:
    # class inhabits only
    action_space = range(0, 12)

    def __init__(self, add_noise=True):
        self.add_noise = add_noise
        self.short_with_degree = lambda degree: act.Action("kick_short", 1080, 150, degree,
                                                           7)  # degree in degrees

    def kick_short_degree(self, eval_state, degree):
        kick = self.short_with_degree(degree)
        eval_state.ball_position = kick.predict(eval_state.ball_position, self.add_noise)

    def get_actions_degree(self, rad):  # actions with current degree

        #########
        #
        #   all actions are functions which expect a state input ( State object )
        #
        #########

        degree = np.degrees(rad) % 360  # get degrees from radian in range 0 to 359.9

        actions = \
            [lambda eval_state: self.kick_short_degree(eval_state, 0),
             lambda eval_state: self.kick_short_degree(eval_state, +30),
             lambda eval_state: self.kick_short_degree(eval_state, -30),
             lambda eval_state: self.kick_short_degree(eval_state, +60),
             lambda eval_state: self.kick_short_degree(eval_state, -60),
             lambda eval_state: self.kick_short_degree(eval_state, +90),
             lambda eval_state: self.kick_short_degree(eval_state, -90),
             lambda eval_state: self.kick_short_degree(eval_state, +120),
             lambda eval_state: self.kick_short_degree(eval_state, -120),
             lambda eval_state: self.kick_short_degree(eval_state, +150),
             lambda eval_state: self.kick_short_degree(eval_state, -150),
             lambda eval_state: self.kick_short_degree(eval_state, +180),
             ]

        names = ["short_0"]
        for degree_name in range(30, 180, 30):
            names += ["short_pl_" + str(degree_name), "short_min_" + str(degree_name)]
        names += ["short_180"]

        return actions, names

    def get_actions_from_state(self, eval_state):
        return self.get_actions_degree(eval_state.direction.angle())


if __name__ == "__main__":
    import visualisation
    import world
    from copy import deepcopy

    # actions = Actions_2(add_noise=False)
    state = State()
    state.direction = Vec(1, 0)

    state_history = []
    key_history = []
    SimWorld = world.World(None, None)
    SimWorld.reset()

    for i in SimWorld.action_space:
        SimWorld.reset()
        SimWorld.state.position = Vec(0, 0)
        SimWorld.state.ball_position = Vec(0, 0)
        SimWorld.state.direction = Vec(1, 0)

        SimWorld.step(i)

        SimWorld.render()

    """
    for key, value in actions.get_actions_degree_dict(direction_in_degrees).items():
        print key
        value(state)
        print state.ball_position
        print state.position
        print state.direction
        state_history += [copy.copy(state)]
    """

    """
    state_history = []
    key_history = []
    
    key_history += ["initial state"]
    state_history += [copy.copy(state)]

    # random actions, visualisation test
    for i in range(0,10):

        key = actions.get_actions_from_state(state).keys()[random.randint(0,4)]
        #key = "turn_right_45"
        actions.get_actions_from_state(state)[key](state)
        if state.position.x != state.ball_position.x:
            state.direction = (state.ball_position - state.position).normalize()
        state.position = state.ball_position
        state_history += [copy.copy(state)]
        key_history += [key]

    """

    visualisation.draw_field_and_sequence(state_history)

    print
    "done"
