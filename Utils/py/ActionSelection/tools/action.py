from __future__ import division
import numpy as np
import math
from naoth import math2d as m2d

num_particles = 30
friction = 0.0275
good_threshold_percentage = 0.85
minGoalParticles = 9


class Action:

    def __init__(self, name, speed=0.0, speed_std=0.0, angle=0.0, angle_std=0.0):
        self.speed = speed
        self.speed_std = speed_std
        self.angle = angle
        self.angle_std = angle_std
        self.name = name

    def predict(self, ball, noise):
        gforce = 9.80620 * 1e3  # mm/s^2
        if self.speed == 0:  # means action is none
            return ball
        if noise:
            if self.speed_std > 0:
              speed = np.random.normal(self.speed, self.speed_std)
            else:
              speed = self.speed
              
            if self.angle_std > 0:
              angle = np.random.normal(math.radians(self.angle), math.radians(self.angle_std))
            else:
              angle = math.radians(self.angle)
              
            distance = speed * speed / friction / gforce / 2.0  # friction*mass*gforce*distance = 1/2*mass*speed*speed
        else:
            distance = self.speed*self.speed / friction / gforce / 2.0
            angle = math.radians(self.angle)

        noisy_action = m2d.Vector2(distance, 0.0)
        noisy_action = noisy_action.rotate(angle)

        return ball + noisy_action

Categories = ["INFIELD", "OPPOUT", "OWNOUT", "LEFTOUT", "RIGHTOUT", "OPPGOAL", "OWNGOAL", "COLLISION", "NUMBER_OF_BallPositionCategory"]
histogram = [0]*50  # Assumes that 50 is the max number of kicks till goal


# This class is a container for a particlePos and its corresponding category
class CategorizedBallPosition:
    def __init__(self, ball_pos, category):
        self.ball_pos = ball_pos
        self.category = category

    def cat(self):
        return self.category

    def pos(self):
        return self.ball_pos


class ActionResults:
    # Todo: first argument is actually a list of CategorizedBallPosition
    # Todo: Check if init has to have parameters
    def __init__(self, categorized_ball_position_list):
        self.ball_positions = categorized_ball_position_list  # type is list of CategorizedBallPosition
        self.cat_histogram = [0]*len(Categories)  # type is list
        self.expected_ball_pos = m2d.Vector2()  # should be in local coordinates

    def positions(self):
        # returns a list of CategorizedBallPosition
        return self.ball_positions

    def category(self, cat):
        # returns a number of category for this list
        return self.cat_histogram[Categories.index(cat)]

    def likelihood(self, cat):
        assert self.cat_histogram[Categories.index("NUMBER_OF_BallPositionCategory")] > 0
        return (self.cat_histogram[Categories.index(cat)]) / (self.cat_histogram[Categories.index("NUMBER_OF_BallPositionCategory")])

    def reset(self):
        self.ball_positions = []
        self.cat_histogram = [0]*len(Categories)  # type is list

    def add(self, position, ball_position_category):
        self.ball_positions.append(CategorizedBallPosition(position, ball_position_category))
        self.cat_histogram[Categories.index(ball_position_category)] += 1
        self.cat_histogram[Categories.index("NUMBER_OF_BallPositionCategory")] += 1
