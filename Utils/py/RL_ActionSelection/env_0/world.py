##############
#
# This skript specifies the rules of the RoboCup Standard Platform League (2017) [regarding the ball position]
#
# The world.py skript uses these rules to check ball positions and/or to move
# the ball according to the given rules
#
#############

#############
#
# in general the ball is represented as two dimensional vector (x,y) with real value entries
# the field center point represents the position (0,0), the coordinate origin
#
# we refer to the player who's goal is at the end of the field with negative x - coordinates as player 1
# and to the other player as player 2
#
#############


#############
#
# rules:
#  * if the the ball went trough one of the goals referring to a player we grant the opposite player a point and reset
#    the ball position to (0,0)
#  * if the ball is in the field, nothing gets changed
#  * if the ball crosses sidelines of the field the ball position gets manipulated as follows:
#      - Done: read rules and encode as formulars
#
#############

#############
#
# technicalities:
#  goal:
#   to calculate goal situations we need at least the last 2 ball position to generate a line, from which we can derive
#   the ball trajectory from which we can conclude weather the ball is got went trough the goal
#
#
#############

import math

from naoth.math2d import Vector2 as Vec
from naoth.math2d import LineSegment

from actions import Actions, Actions_2, Actions_3
from state import State

from copy import deepcopy
import numpy as np


class Field():
    """
    the field class represents the soccer field

    this class is going to be the subclass on which world and referee operate

    thus we include functions to check weather the ball in in the field, in one goal,
    outside the field etc.

    """

    width = 9000
    height = 6000
    center_point = Vec(0, 0)
    opp_goal_line = LineSegment(Vec(4500, 750), Vec(4500, -750))
    own_goal_line = LineSegment(Vec(-4500, 750), Vec(-4500, -750))

    objects = []

    def in_own_goal(self, object1, object2=None):
        """
        detects weather the is / went trough in the own goal
        :param object: m2d Vector2
        :return: boolean
        """

        ball_trajectory = self.get_ball_trajectory(object1, object2)

        if object2 is None or ball_trajectory is None:
            # ball trajectory is none, if the two points are less than 1 cm apart
            return (object1.x < -4500 and (-750 < object1.y < 750))
        in_goal = self.own_goal_line.intersect(ball_trajectory) and \
                  ball_trajectory.intersect(self.own_goal_line)
        if in_goal == float('inf'):
            in_goal = 0
        return in_goal

        # return ((object1.x < -4500) and (-750 < object1.y < 750))

    def in_opp_goal(self, object1, object2=None):
        """
        detects weather the ball is / went trough in the opponents goal
        :param object: m2d Vector2
        :return: boolean
        """

        ball_trajectory = self.get_ball_trajectory(object1, object2)

        if object2 is None or ball_trajectory is None:
            # ball trajectory is none, if the two points are less than 1 cm apart
            return (object1.x > 4500 and (-750 < object1.y < 750))

        in_goal = self.opp_goal_line.intersect(ball_trajectory) and \
                  ball_trajectory.intersect(self.opp_goal_line)
        if in_goal == float('inf'):
            in_goal = 0
        return in_goal

    def add_object(self, object):
        self.objects.append(object)

    def object_collision(self, state):
        """
        ball_trajectory = LineSegment(state.position, state.ball_position)

        collision = object.intersect(ball_trajectory) and \
                  ball_trajectory.intersect(object)

        if collision == float('inf'):
            collision = 0
        return collision
        """
        ball_trajectory = self.get_ball_trajectory(state.position, state.ball_position)

        for object in self.objects:
            if object.collision(ball_trajectory):
                return True
        return False

    def in_field(self, object):
        """
        detects weather the object is in the field
        :param object: m2d Vector2
        :return: boolean
        """
        return ((-4500 <= object.x <= 4500) and (-3000 <= object.y <= 3000))

    def get_ball_trajectory(self, player, ball):
        """

        :param ball: math2d Vector2 object
        :param player: math2d Vector2 object
        :return: return line segment
        """
        return LineSegment(player, ball)


class Rules(Field):
    """
    the rules class checks ball positions and manipulates the ball according to the
    RoboCup Standard Platform League - rules

    this implementation is going to be rather naive and not complete
    """

    def ball_in_goal(self, state):
        ball = state.ball_position
        player = state.position

        if self.in_own_goal(ball, player) or self.in_opp_goal(ball, player):
            ball = self.center_point

    def ball_out(self, state):
        ball = state.ball_position
        player = state.position

        if not (self.in_field(ball, player) or self.ball_in_goal(ball, player)):
            self.throw_in(state)

    def throw_in(self, state):
        """
        Throw-in rules:
            Throw-in line:  400mm into the field from side lines
                            7000mm long (from (-3500,-2600) to (3500,-2600) &
                                            (-3500, 2600) to (3500, 2600) )

                - ball placed on throw-in line on the same side
                - paces farthest back of one of these options:
                    * 1000mm back form the robot
                    * 1000mm back from the point where it went out
                    * always placed on the throw-in line

            End-line:
                - if last touched by defensive team
                    -> placed at closed endpoint of throw-in line
                - if last touched by offensive team
                    -> placed on the throw-in line on the farthes back of these positions:
                        * 1000mm back from last touched robot
                        * at the intersection of halfway and throw-in line (0, -2600) or (0, 2600)


        :param ball: ball position (x,y) coordinates
        :param player: player position (x,y) coordinates
        :return: new ball position (x,y) coordinates
        """

        ball = state.ball_position
        player = state.position

        new_ball = Vec()
        if (not self.in_field(ball)) and (not self.in_own_goal(ball, player)) and (
        not self.in_opp_goal(ball, player)):
            if player is not None:
                ## here we always asume that there is only one player thus we know who touched the ball at last
                ### the opponent goal is on the right, the own goal on the left side
                # first case: ball went out crossing a sideline
                if -4500 < ball.x < 4500:
                    if ball.x <= player.x:
                        # take option which places the ball the furthest back
                        new_ball.x = (ball.x - 1000)
                    else:
                        new_ball.x = (player.x - 1000)
                    if new_ball.x > 3500:
                        new_ball.x = 3500
                    if new_ball.x < -3500:
                        new_ball.x = -3500
                # second case: ball went out crossing the end-line
                if ball.x > 4500:
                    if player.x - 1000 < 0:
                        new_ball.x = 0
                    else:
                        new_ball.x = player.x - 1000
                if ball.x < -4500:
                    new_ball.x = -3500  # adding random noise

                if ball.y > 0:  # set on upper or lower throw-in line
                    new_ball.y = 2600
                else:
                    new_ball.y = -2600

            ball = new_ball  # set new_ball as ball otherwise just return the ball position
        state.ball_position = ball


class World(Rules, Actions_3):
    """
    This class models the World in which the Robot interacts

    With this class all actual actions get calculated and then passed on, in simpler form ( given trough features ),
    to the environment on which the agent learns it behaviour.

    For further information see the documentation.

    TODO: Write Documentation!

    """

    reset_x_range = [-4490, 4490]
    reset_y_range = [-2990, 2990]
    reset_rad_range = [0, 2 * np.pi]

    history = []  # add max size??

    state = None

    def __init__(self, reward_function, feature_function):
        Actions_3.__init__(self, add_noise=True)
        if feature_function is None:
            self.feature = lambda state=None, return_output_state=None: None
        else:
            self.feature = feature_function  # feature function which gives a feature vector

        if reward_function is None:
            self.reward = lambda state=None, old_state=None: None
        else:
            self.reward = lambda state, old_state: reward_function(self, state,
                                                                   old_state)  # reward function which derives reward from state ( inout must be State object )

        self.state = self.reset_state()  # initialize state

    def current_state(self):
        return self.feature(self.state)

    def perform_action(self, state, chosen_action):
        """
        simulates a chosen action (with noise)

        :param state: current state of the robot
        :param chosen_action: integer between 0,...,(nr of actions - 1)
        :return: None
        """

        action_list, _ = self.get_actions_from_state(state)

        action_list[chosen_action](state)  # take action
        self.throw_in(state)  # check throw in rules

    def move_robot_to_ball(self, state):
        """
        move the robot to ball position

        function manipulates input object -> no return
        """

        # just set if position are not equal
        # (otherwise rotation action could get lost)
        if state.position.x != state.ball_position.x or \
                state.position.y != state.ball_position.y:
            # robot orientation (direction) is the direction of the shortest path to the ball
            state.direction = (state.ball_position - state.position).normalize()
            # new position is equal to ball position
            state.position = state.ball_position

    def step_old(self, state, action):
        self.perform_action(state, action)
        self.move_robot_to_ball(state)

    def reset_state(self, range_x=reset_x_range, range_y=reset_y_range, range_rad=reset_rad_range):
        new_state = State()

        new_state.position.x = np.random.randint(range_x[0],
                                                 range_x[1])  # get random position in field
        new_state.position.y = np.random.randint(range_y[0], range_y[1])
        new_state.ball_position = deepcopy(new_state.position)

        rad = range_rad[1] * np.random.rand() - range_rad[0] * np.random.rand()
        new_state.direction = Vec(1, 0).rotate(rad)
        self.state = new_state

    def terminated(self, state):
        # position and ball position should not be equal for this check to be usefull!!
        if self.in_opp_goal(state.position, state.ball_position):
            return True  # reward = 100
        elif self.in_own_goal(state.position, state.ball_position):
            return True  # reward = -100

        for obstacle in self.obstacles:
            pass  # check collision

        return False

    def reset_history(self):
        self.history = []

    def remember(self, state, action, reward, next_state, done):
        self.history.append((state, action, reward, next_state, done))

    def step(self, action):
        # watch out for correct evaluation process:
        #   state -> predict action -> do action -> receive reward -> get termination state-> move to new position
        #       -> add to history

        old_state = deepcopy(self.state)

        self.perform_action(self.state, action)  # do action

        reward, done = self.reward(self.state, old_state)  # get reward for action

        # done = self.terminated(self.state)  # get termination state (bool)

        self.move_robot_to_ball(self.state)  # move robot to ball

        new_state = deepcopy(self.state)

        self.remember(old_state, action, reward, new_state, done)  # add to history

        observation = self.feature(
            self.state)  # output the observation for environment (simplified position)

        info = {}

        return observation, reward, done, info

    def reset(self):
        self.reset_state()
        self.reset_history()
        return self.feature(self.state)

    def close(self):
        self.reset()

    def render(self):
        from matplotlib import pyplot as plt
        from naoth import math2d as m2d
        from tools.tools import draw_field

        state_history = [None] * len(self.history)
        action_history = [None] * len(self.history)
        for i, entry in enumerate(self.history):
            state_history[i] = entry[0]
            action_history[i] = entry[1]
            if i == len(self.history) - 1:
                state_history.append(entry[3])

        print(action_history)
        print(str(len(self.history)))

        plt.clf()
        axes = plt.gca()
        draw_field(axes)

        # h1 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in run])
        positions = np.array([[state.position.x, state.position.y] for state in state_history])
        # plt.plot(positions[:,0], positions[:,1], '-or')

        _, action_names = self.get_actions_degree(0)

        for i, state in enumerate(state_history):
            # plt.scatter(positions[:i+1, 0], positions[:i+1, 1], '-or')

            plt.plot(positions[:i + 1, 0], positions[:i + 1, 1], '-or')

            arrow_head = m2d.Vector2(500, 0).rotate(state.direction.angle())
            axes.arrow(state.position.x, state.position.y, arrow_head.x, arrow_head.y,
                       head_width=100,
                       head_length=100, fc='k', ec='k')
            if i < len(state_history) - 1:
                axes.text(x=positions[i, 0], y=positions[i, 1] + 30,
                          s=action_names[action_history[i]])
            plt.pause(0.2)
        plt.show()

        print("#" * 20)


if __name__ == "__main__":
    import features
    import reward
    from naoth import math2d

    simWorld = World(reward.simple_reward, features.no_features_normalized)
    simWorld.state = State(position=math2d.Vector2(4000, 2800), direction=math2d.Vector2(0, 1))
    # print simWorld.feature(simWorld.state).shape
    simWorld.step(0)
    simWorld.step(0)
    simWorld.render()
