##############
#
# agent who interacts with the world
#
# tries to find an optimal policy
#
##############

import numpy as np
import pickle

from state import State
from world import *
from actions import Actions
from naoth.math import Vector2 as Vec
from copy import copy

game_field = Field()


# TODO: Add rotation feature or something similar
def features(position):
    """
    generates the feature vector Phi from a given field position (x,y)

    for now we use coarse coding
    :param position: math2d Vector2 Element
    :return: numpy array of size (#balls,1)
    """

    field_height = game_field.height
    field_width = game_field.width

    radius_of_balls = 50
    distance_of_balls = 50

    sub_div_x = field_width / distance_of_balls  # number of subdivisions
    sub_div_y = field_height / distance_of_balls

    number_of_balls = sub_div_x * sub_div_y
    output = np.zeros((number_of_balls, 1))  # generate the output vector

    if not game_field.in_field(position):
        return output

    position = position + Vec(4500,
                              3000)  # for more intuitive calculation transpose the position vector

    x_1 = int(position.x // distance_of_balls)  # potential ball candidates

    y_1 = int(position.y // distance_of_balls)

    # fill the output // feature vector

    for i in [x_1 - 1, x_1, x_1 + 1]:
        for j in [y_1 - 1, y_1, y_1 + 1]:
            center_ball = Vec(i * distance_of_balls,
                              j * distance_of_balls)  # get potential ball center points
            if (
                    position - center_ball).abs() <= radius_of_balls:  # check distance of position to center point
                if 0 <= i <= sub_div_x and 0 <= j <= sub_div_y:
                    output[
                        (i - 1) * sub_div_y + (j - 1)] = 1  # if position in field set feature = 1

                    # print "ball center: " + str(center_ball) + ", in ball: " \
                    #      + str(output[(i-1) * sub_div_y + (j-1)])

    return output


class Agent:
    def __init__(self, gen_features):
        # initialize theta
        self.gen_features = lambda position: gen_features(position)
        # gen_features has to generate an numpy array from a field position
        # -- the used features
        # self.theta = np.zeros(self.gen_features(Vec(0, 0)).shape)
        # self.theta = np.concatenate((self.theta, np.array([[1]])))
        self.theta = 5 * np.random.random(self.gen_features(Vec(0, 0)).shape) - 2.5

        self.actions = Actions(add_noise=False)

        # important parameters
        self.epsilon = 0.1
        self.gamma = 0.95
        self.stepsize = 0.01  # refers to alpha in the literature
        self.alg_lambda = 0.95  # refers to lambda parameter

    def reward(self, position):
        # TODO: can be deleted
        # calculate the expected reward
        # TODO: Add bias term
        return float(np.dot(self.gen_features(position).transpose(), self.theta))

    def run_sarsa(self, initial_state, action_evaluation):
        """
        this algorithm is based on the Sutton and Barto p. 212 linear, gradient-descent Sarsa(lambda)
            with binary features algorithm with epsilon greedy policy

        :param initial_state: state object
        :param action_evaluation: function with which a chosen action gets evaluated
                must return: new state, resulting reward of the action terminated status
        :return: nothing, updated theta values
        """
        # initialize Theta and gradient E
        # theta = self.theta
        grad = np.zeros(self.theta.shape)
        # get actions
        action_list = self.actions.get_actions()
        terminated = False
        state = initial_state

        # observation variables
        episode_length = 1
        overall_reward = 0

        action_features = []
        expected_rewards = []

        for act in action_list:
            # derive features for each expected position
            # for each action get new expected position
            state_copy = copy(state)  # copy 'real' state
            act(state_copy)  # get expected position after action
            action_features.append(
                self.gen_features(
                    state_copy.ball_position))  # add features of the expected ball position

        for feature in action_features:
            expected_rewards.append(float(np.dot(feature.transpose(), self.theta)))

        best_action = np.argmax(np.array(expected_rewards))
        # get best action

        # apply epsilon greedy method
        if np.random.random() <= self.epsilon:
            best_action = np.random.randint(0, len(action_list))
            # with epsilon certainty select a random action (exploration)
            # otherwise take best action as chosen before

        while not terminated:
            grad = self.gamma * self.alg_lambda * grad
            # TODO: replace trace ?? yes??

            for i in range(0, len(action_features)):
                if i != best_action:
                    # for features in non selected actions set grad entry to zero
                    grad = grad - np.multiply(grad, action_features[i])

            # for features in selected action set grad entry to one ( entries of the gradient
            # which don't get chosen by any action remain untouched
            grad = grad + action_features[best_action] - np.multiply(grad,
                                                                     action_features[best_action])

            ### get new state
            new_state, reward, terminated = action_evaluation(state, best_action)
            if new_state.position.x == new_state.position.y:
                pass

            # reward difference
            delta = reward - expected_rewards[best_action]

            # repeat selection process
            new_action_features = []
            new_expected_rewards = []

            for act in action_list:
                # derive features for each expected position
                # for each action get new expected position
                new_state_copy = copy(new_state)  # copy 'real' state
                act(new_state_copy)  # get expected position after action
                new_action_features.append(
                    self.gen_features(
                        new_state_copy.ball_position))  # add features of the expected ball position

            for feature in new_action_features:
                new_expected_rewards.append(float(np.dot(feature.transpose(), self.theta)))

            new_best_action = np.argmax(np.array(expected_rewards))
            # get best action

            # apply epsilon greedy method
            if np.random.random() <= self.epsilon:
                new_best_action = np.random.randint(0, len(action_list))
                # with epsilon certainty select a random action (exploration)
                # otherwise take best action as chosen before

            delta = delta + self.gamma * new_expected_rewards[new_best_action]
            self.theta = self.theta + self.stepsize * delta * grad
            best_action = new_best_action

            ### observation section
            overall_reward += reward
            # print episode_length
            # if (episode_length % 100) == 0:
            #    print new_state.position
            #    print best_action
            episode_length += 1

            # set gradient to zero again
            # grad = np.zeros(self.theta.shape)

        # self.theta = theta

        return overall_reward, episode_length

    def save_model(self):
        # TODO: Add name for different models / parameters
        save = open('save.p', 'wb')
        pickle.dump({'theta': self.theta}, save)

    def load_model(self):
        model = pickle.load(open('save.p', 'rb'))
        self.theta = model['theta']


if __name__ == "__main__":
    # print(features(Vec(-4500,-3000))) # test feature generation
    TestAgent = Agent(features, resume=False)
    print(TestAgent.reward(Vec(2000, 30)))
    robot_state = State(1000, 100)

    act = Actions(add_noise=False)
    new_state = State()
    act.kick_short(new_state)
    print
    new_state.ball_position

    TestAgent.run_sarsa(State())
