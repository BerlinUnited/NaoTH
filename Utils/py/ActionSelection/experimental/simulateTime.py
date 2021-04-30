import math
import numpy as np
from naoth.math import *


class Robot:
    def __init__(self):
        self.pose = Pose2D()
        self.pose.translation = Vector2(0, 0)
        self.pose.rotation = math.radians(0)
        self.ball_position = Vector2(2500.0, -450.0)  # relative to robot

        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # degrees per sec


if __name__ == "__main__":
    r = Robot()
    angle = np.degrees(np.arctan2(r.ball_position.y, r.ball_position.x))
    rotation_time = np.abs(angle / r.rotation_vel)
    distance = np.hypot(r.ball_position.x, r.ball_position.y)

    distance_time = distance / r.walking_vel
    total_time = distance_time + rotation_time

    with open("times.txt", "w") as f:
        f.write("Time to Ball: " + str(total_time) + " seconds")

    # This is a fairly simplistic time estimation
    # The actual robot can make steps that are a combination of turning and forward walking
    # Also just models a straight line, no path planning is considered

    # A slightly better implementation would be to give desired rotation at the ball and stop before the actual ball position
    # and the second turning time as well. This would model the time the robot needs to kick the ball (again)
