import math
from naoth import math2d as m2d

# represents the robots view on the situation


class State:
    def __init__(self, x=1000, y=100):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(x, y)
        self.pose.rotation = math.radians(0)

        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec

        self.ball_position = m2d.Vector2(0.0, 0.0)

        # Possible options: normal, influence_01, generated
        self.potential_field_function = "normal"

        self.opp_robots = ([])  # is in global coordinates
        self.own_robots = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)
