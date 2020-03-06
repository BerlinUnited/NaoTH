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

from naoth.math import Vector2 as Vec
from naoth.math import LineSegment

from actions import Actions
from state import State


class Field():
    """
    the field class gives represents the soccer field

    this class is going to be the component on which world and referee operate

    thus we include functions to check weather the ball in in the field, in one goal,
    outside the field etc.

    """

    def __init__(self):
        self.width = 9000
        self.height = 6000
        self.center_point = Vec(0, 0)
        self.opp_goal_line = LineSegment(Vec(4500, 750), Vec(4500, 750))
        self.own_goal_line = LineSegment(Vec(-4500, 750), Vec(-4500, -750))

    def in_onw_goal(self, object1, object2=None):
        """
        detects weather the is / went trough in the own goal
        :param object: m2d Vector2
        :return: boolean
        """
        ball_trajectory = self.get_ball_trajectory(object1, object2)

        if object2 is None or ball_trajectory is None:
            # ball trajectory is none, if the two points are less than 1 cm apart
            return (object1.x < -4500 and (-750 < object1.y < 750))
        in_goal = self.own_goal_line.intersection(ball_trajectory) and \
                  ball_trajectory.intersection(self.own_goal_line)
        if in_goal == float('inf'):
            in_goal = 0
        return in_goal

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
        in_goal = self.opp_goal_line.intersection(ball_trajectory) and \
                  ball_trajectory.intersection(self.opp_goal_line)
        if in_goal == float('inf'):
            in_goal = 0
        return in_goal  # TODO: VERY IMPORTANT !!!! CHECK IF BOTH LINES HAVE INTERSECTION ON THEIR LINE SEGMENT

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
        if (ball - player).abs() <= 100:
            return None  # if distance of the player and ball are to small, return nothing
        return LineSegment(player, ball)


class Rules(Field):
    """
    the rules class checks ball positions and manipulates the ball according to the
    RoboCup Standard Platform League - rules

    this implementation is going to be rather naive and not complete
    """

    def apply_rules(self, ball, player=None):
        """
        applies all robo cup rules

        :param ball: m2d Vector2 object
        :param player: m2d Vector2 object - player who most recently touched the ball
        :return: m2d Vector2 object, 'new' ball position
        """

    def ball_in_goal(self, ball, player):
        if self.in_own_goal(ball) or self.in_opp_goal(ball):
            ball = self.center_point

    def ball_out(self, ball, player=None):
        if not (self.in_field(ball) or self.ball_in_goal(ball)):
            self.throw_in(ball)

    def throw_in(self, ball, player=None):
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
        sides = ['lower', 'upper']
        new_ball = Vec()
        if not (self.in_field(ball) or self.in_onw_goal(ball, player) or self.in_opp_goal(ball,
                                                                                          player)):
            if player is not None:
                side = sides[ball.y > 0]
                ## here we always asume that there is only one player thus we know who touched the ball at last
                ### the opponent goal is on the right, the own goal on the left side
                # TODO: add team / side recognition
                # TODO: could be done with trajectories as in the goal methods
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
                    new_ball.x = -3500

                if side == 'upper':  # set on upper or lower throw-in line
                    new_ball.y = 2600
                else:
                    new_ball.y = -2600

            # TODO: if player is None fall einfuegen // Noetig??

            ball = new_ball  # set new_ball as ball otherwise just return the ball position
        return ball


class World:
    """
    wrapper class with which the simulator interacts
    """

    def __init__(self):
        # self.current_state = initial_state
        self.last_action = None
        self.actions = Actions(add_noise=True)
        self.Rules = Rules()

    def do_action(self, given_state, chosen_action):
        action_list = self.actions.get_actions()
        action_list[chosen_action](given_state)
        self.last_action = chosen_action

    def set_robot_position_to_ball_position(self, given_state):
        if given_state.position != given_state.ball_position:  # just set if position don't not equal
            # (otherwise rotation action could get lost
            given_state.direction = (given_state.ball_position - given_state.position).normalize()
            # robot orientation (direction) is the direction of the shortest path to the ball
            given_state.position = given_state.ball_position
            # new position is equal to ball position


if __name__ == "__main__":
    """
    GameRules = Rules()
    new_ball = GameRules.throw_in(Vec(-2000,2000),Vec(-1000,2000))
    print(new_ball)
    GameActions = Actions()
    new_state = State()
    print new_state.direction
    GameActions.turn_right_45(new_state)
    print new_state.direction
    print new_state.ball_position
    GameActions.sidekick_right(new_state)
    #GameActions.sidekick_right(new_state)
    print new_state.ball_position
    GameActions.sidekick_left(new_state)
    print new_state.ball_position
    GameActions.kick_short(new_state)
    print new_state.ball_position
    """

    simWorld = World()
    for i in range(0, 5):
        print
        "Action: " + str(i)
        print
        "before:"
        new_state = State()
        print
        "Ball Position:"
        print
        new_state.ball_position
        print
        "Position:"
        print
        new_state.position
        print
        "Direction"
        print
        new_state.direction
        print
        "++++++++"
        simWorld.do_action(new_state, i)
        print
        "after kick"
        print
        "Ball Position:"
        print
        new_state.ball_position
        print
        "Position:"
        print
        new_state.position
        print
        "Direction"
        print
        new_state.direction
        print
        "++++++++"
        simWorld.set_robot_position_to_ball_position(new_state)
        print
        "after resetting position"
        print
        "Ball Position:"
        print
        new_state.ball_position
        print
        "Position:"
        print
        new_state.position
        print
        "Direction"
        print
        new_state.direction
        print
        "------------------"
