from naoth import math2d as m2d

# represents the robots view on the situation

class State(object):
    def __init__(self, position=m2d.Vector2(0.,0.), direction=m2d.Vector2(1.,0.)):
        self.position = position  # position on the field as 2 d element
        self.direction = direction  # facing direction as vector

        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec

        self.ball_position = m2d.Vector2(0.0, 0.0)

        """
        include maybe later
        self.opp_robots = ([])  # is in global coordinates
        self.own_robots = ([])  # is in global coordinates
        """

    def update_pos(self, position=None, direction=None):
        if position is not None:
            self.position = position
        if direction is not None:
            self.direction = direction
