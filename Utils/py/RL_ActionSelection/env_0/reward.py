# script for reward function
from naoth.math2d import Vector2 as Vec
from naoth.math2d import LineSegment
from objects import Rectangle


def simple_reward_normalized(world, state, old_state=None):
    """
    simple reward function which checks whether the ball is in one of the goal
        - this returns a final reward and sets the termination state to True
        - else the reward is -1 and termination state is False

    :param state: state object as in state.py
    :return: reward (float)
    """

    if world.in_opp_goal(state.position, state.ball_position):
        return 1., True  # reward = 100
    elif world.in_own_goal(state.position, state.ball_position):
        return -1., True  # reward = -100

    return 0., True  # -0.1  # reward = -1


def simple_reward(world, state, old_state=None):
    """
    simple reward function which checks whether the ball is in one of the goal
        - this returns a final reward and sets the termination state to True
        - else the reward is -1 and termination state is False

    :param state: state object as in state.py
    :return: reward (float)
    """

    if world.in_opp_goal(state.position, state.ball_position):
        return 10., True  # reward = 100
    elif world.in_own_goal(state.position, state.ball_position):
        return -10., True  # reward = -100

    return 0., True  # -0.1  # reward = -1


def simple_reward_obstacle(world, state, old_state=None):
    """
    simple reward function which checks whether the ball is in one of the goal
        - this returns a final reward and sets the termination state to True
        - else the reward is -1 and termination state is False

    :param state: state object as in state.py
    :return: reward (float)
    """
    if world.in_opp_goal(state.position, state.ball_position):
        done = True
        return 1., True  # reward = 100
    elif world.in_own_goal(state.position, state.ball_position):
        return -1., True  # reward = -100

    """
    obstacle_upper = Vec(3000, 1000)
    obstacle_lower = Vec(3000, 500)
    object = LineSegment(obstacle_upper, obstacle_lower)
    """

    if world.object_collision(state=state):
        return -1, True
    # add object collision from world

    return 0., False


def distance_motivated_reward(world, state, old_state):
    # state has performed an action, robot was not moved to ball
    opp_goal_center = Vec(4500., 0.)
    old_position = state.position
    new_position = state.ball_position

    done = False

    old_distance = (opp_goal_center - old_position).abs()
    new_distance = (opp_goal_center - new_position).abs()

    reward = old_distance - new_distance / 9.486  # normalized by max distance from opp goal

    if world.in_opp_goal(state.position, state.ball_position):
        done = True
    elif world.in_own_goal(state.position, state.ball_position):
        done = True

    return reward, done
