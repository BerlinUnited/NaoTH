#################################
#                               #
#    /\/\/\/\/\/\/\/\/\/\/\/\   #
#    \/\/\/\/\/\/\/\/\/\/\/\/   #
#                               #
#       Environment wrapper     #
#                               #
#    /\/\/\/\/\/\/\/\/\/\/\/\   #
#    \/\/\/\/\/\/\/\/\/\/\/\/   #
#                               #
#################################

import gym  # import gym for action spaces
from rl import core  # import Keras reinforcement learning

import numpy as np

import world


class Env(core.Env):
    # Environment class
    # build as needed for keras-rl (https://github.com/keras-rl/keras-rl/blob/master/rl/core.py)

    reward_range = (-np.inf, np.inf)
    action_space = gym.spaces.Discrete(5)
    observation_space = gym.spaces.MultiBinary(872)  # type: MultiBinary TODO: make derivable

    def __init__(self, reward, features):
        # reward and features are function, for further details see world class
        self.world = world.World(reward, features)

    def step(self, action=None):
        next_state, reward, done, info = self.world.step(action)

        return next_state, reward, done, info

    def render(self):
        #  implement maybe in the futur
        #  visualize traces and agent movement on field
        self.world.render()

    def close(self):
        self.world.close()

    def reset(self):
        observation = self.world.reset()
        return observation
