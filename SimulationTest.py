import Simulation
import math2d as m2d
import math
for x in range(-4500,4500,100):
  for y in range(-3000,3000,100):
    pose = m2d.Pose2D()
    rotation = 0
    pose.x = x
    pose.y = y
    pose.rotation = math.radians(rotation)

    # Ball Position
    ballPosition = m2d.Vector2()
    ballPosition.x = 0.0
    ballPosition.y = 0.0
