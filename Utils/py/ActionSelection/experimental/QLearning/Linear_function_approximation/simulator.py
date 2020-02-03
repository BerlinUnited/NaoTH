from state import State
from world import *
from actions import Actions
from agent import Agent, features
from copy import copy, deepcopy
import numpy as np
import pickle

from naoth.math import Vector2 as Vec

# classes camel case, functions lower case

simWorld = World()
simRules = Rules()


def derive_reward_termination(state):
    """
    opp_goal -> r = 1000
    own_goal -> r = -1000
    t := time needed to move to ball
    r = -t (if ball not in goal)

    velRot = 60 # dregrees per second
    velWalk = 200 # mm per second

    r = -( dist(robot_pos, ball_pos)/200 + |robot_direction - ball. angle|/60 )
    rotation time + walk time

    :param ball_position: Vector2 object
    :return: float -- reward
    """
    if simRules.in_opp_goal(state.position, state.ball_position):
        return -1000., True  # episode has terminated
    elif simRules.in_onw_goal(state.position, state.ball_position):
        return 1000., True  # episode has terminated

    """
    velRot = 60.
    velWalk = 200.

    robot_position = state.position
    ball_position = state.ball_position

    robot_angle = state.direction.angle()
    ball_angle = state.ball_position.angle()
    # gives radian with math.atan2 function
    # need to convert to degrees from 0 to 360 for further calculations

    if robot_angle < 0:
        robot_angel = (2*math.pi + robot_angle) * 360 / (2 * math.pi)
    if ball_angle < 0:
        ball_angle = (2*math.pi + ball_angle) * 360 / (2 * math.pi)


    distance = (robot_position - ball_position).abs()
    angle_difference = abs(ball_angle - robot_angle)
    if angle_difference > abs(angle_difference - 360): # take smaller angle
        angle_difference = abs(angle_difference - 360)

    reward = - (distance / velWalk + angle_difference / velRot)

    # TODO: Add robot position = ball position after reward got calculated
    

    return reward, False  # episode is not terminal
    """
    return 0., False


def derive_new_state(state, action):
    """

    :param state: state object
    :param action: integer representing the chosen action
    :return: None, sets the state object parameters
    """
    simWorld.do_action(state, action)


def reward_state_termination_wrapper(state, action):
    """

    :param state: state object
    :param action: integer representing an action
    :return: reward (integer), new state (state object), boolean representing whether the
                episode ended (ball is in own or opposite goal)
    """
    new_state = deepcopy(state)
    derive_new_state(new_state,
                     action)  # set state to the new state which get derive from the world model
    reward, terminated = derive_reward_termination(new_state)
    simWorld.set_robot_position_to_ball_position(new_state)
    if new_state.position.x == new_state.position.y:
        pass
    return new_state, reward, terminated


def calculate_mean(old_mean, num_of_iterations, new_value):
    new_mean = old_mean * (1. - 1 / float(num_of_iterations)) + new_value * (
                1 / float(num_of_iterations))
    return new_mean


def run(save_model=True, load_model=False, save_mean=False):
    SimAgent = Agent(features)

    if load_model:
        try:
            SimAgent.load_model()
        except(IOError):
            pass

    run = 1
    run_length = 200

    mean_history = []
    save_mean_history = True

    print
    "\nrunning...\n"
    print
    "run: " + str(run) + "; length: " + str(run_length)
    print
    "   -------   "

    while True:
        mean = 0
        for i in range(1, 201):

            initial_state = State()

            # TODO: start from whole field / or do increment wise adaption
            initial_state.position.x = np.random.randint(-4500,
                                                         0)  # get random position in own field
            initial_state.position.y = np.random.randint(-3000, 3000)
            initial_state.ball_position = deepcopy(initial_state.position)
            # print "episode: " + str(i) + " start position: " + str(initial_state.position)

            overall_reward, episode_length = SimAgent.run_sarsa(initial_state,
                                                                lambda state,
                                                                       action: reward_state_termination_wrapper(
                                                                    state, action))
            mean = calculate_mean(mean, i % 50 + 1, overall_reward)
            if (i % 50) == 0:
                print
                "Mean reward on run " + str(i - 50) + " - " + str(i)
                print
                mean
                print
                "Max Norm:"
                print
                np.linalg.norm(SimAgent.theta, np.inf)
                print
                "   -------   "
                mean_history.append(mean)
                mean = 0

        if save_model:
            print
            "\nsaving model..."
            SimAgent.save_model()

        if save_mean:
            if save_mean_history:
                name = "mean_histroy_" + str(run)
                save = open(name + '.p', 'wb')
                pickle.dump({name: mean_history}, save)

        print
        "\n++++++++++++++++++++++++++++++++++++++\n"
        run += 1
        print
        "run: " + str(run)


if __name__ == "__main__":
    """
    #start_simulation()
    new_state = State()
    new_state.ball_position = Vec(1000,0)
    print new_state.ball_position.angle()
    """
    run()
