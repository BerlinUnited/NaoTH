import numpy as np
import math
import math2d as m2d

numParticles = 30
friction = 0.0275
good_threshold_percentage = 0.85

class Action:
  def __init__(self,name, speed=0.0, speed_std=0.0, angle=0.0, angle_std=0.0):
    self.speed = speed
    self.speed_std = speed_std
    self.angle = angle
    self.angle_std = angle_std
    self.name = name

  def predict(self, ball):
    gforce = 9.80620 * 1e3  # mm/s^2

    speed = np.random.normal(self.speed, self.speed_std)
    distance = speed * speed / friction / gforce / 2.0  # friction*mass*gforce*distance = 1/2*mass*speed*speed
    angle = np.random.normal(math.radians(self.angle), math.radians(self.angle_std))
    noisyAction = m2d.Vector2(distance, 0.0)
    noisyAction = noisyAction.rotate(angle) # vector2 is const

    return ball + noisyAction

class CategorizedBallPosition:
  def __init__(self,ballPos,category):
    self.ballPos = ballPos
    self.category = category
