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

import numpy as np

import world


class Env():
    # Environment class

    def __init__(self, rewards, features):
        # reward and features are function, for further details see world class
        self.world = world.World(rewards, features)

        self.action_space = self.world.action_space

    def step(self, action=None):
        next_state, reward, done, info = self.world.step(action)

        return next_state, reward, done, info

    def render(self, mode):
        #  implement maybe in the futur
        #  visualize traces and agent movement on field
        self.world.render()

    def close(self):
        self.world.close()

    def current_state(self):
        return self.world.current_state()

    def reset(self):
        observation = self.world.reset()
        return observation
