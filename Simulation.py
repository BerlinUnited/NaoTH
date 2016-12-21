from matplotlib import pyplot as plt
import math2d as m2d
import numpy as np
import math
from fieldInfo import * #Maybe bad
import helperFunctions as h

import Action as A
import potentialField as pf

def simulateConsequences(action, pose,ballPosition, ActionResults): #Todo Check for Collisions with opp goal and if ball is out

  goal = owngoal = infield = out = 0
  goalBackSide1 = ((oppGoalBackLeft.x,oppGoalBackLeft.y),(oppGoalBackRight.x,oppGoalBackRight.y))
  goalBackSide2 = ((opponentGoalPostLeft.x,opponentGoalPostLeft.y),(oppGoalBackLeft.x,oppGoalBackLeft.y))
  goalBackSide3 = ((opponentGoalPostRight.x,opponentGoalPostRight.y),(oppGoalBackRight.x,oppGoalBackRight.y))

  for i in range(0, A.numParticles):
    newBallPos = pose * action.predict(ballPosition)
    newBallPosGlobal = pose * newBallPos
    globalBallPos = pose * ballPosition
    shootline = ((globalBallPos.x,globalBallPos.y),(newBallPosGlobal.x,newBallPosGlobal.y))
    shootlineLength = h.distance(shootline)

    #if ball is not in field check for collisions with oppGoal and shorten the ball
    #Todo use those checks to shorten the ball later
    #Check if ball hits the goal contruction for each particle
    intersection1 = h.line_intersection(shootline, goalBackSide1)
    intersection2 = h.line_intersection(shootline, goalBackSide2)
    intersection3 = h.line_intersection(shootline, goalBackSide3)

    #Obstacle currently not used


    #Check if particle scores a goal
    intersectionOppGoal = h.intersect(ballPosition,ballPosition, opponentGoalPostLeft,opponentGoalPostRight)
    #Check if ball hits the own goal
    intersectionOwnGoal = h.intersect(ballPosition,ballPosition , ownGoalPostLeft,ownGoalPostRight)
    category = "INFIELD"
    if intersectionOppGoal and newBallPosGlobal.x < oppGoalBackLeft and oppGoalBackRight <newBallPosGlobal.y < oppGoalBackLeft and newBallPosGlobal.y:
      category = "OPPGOAL"
    elif -4500 < newBallPosGlobal.x < 4500  and -3000 < newBallPosGlobal.y < 3000:
      infield +=1
    elif intersectionOwnGoal:
      category = "OWNGOAL"
    else:
      category = "OUT"
    ActionResults.append(A.CategorizedBallPosition(newBallPos,category))

  #ActionConsequences.append(action_consequencesI)
  return ActionResults

def decide_smart(ActionConsequences):

  numberOfInfieldPos = 0
  acceptableActions = []
  score = 0
  for action in ActionConsequences:
    for i in range(0, A.numParticles):
      if action[i].category == "OWNGOAL":
        continue
      elif action[i].category == "INFIELD" or action[i].category == "OPPGOAL":
        numberOfInfieldPos += 1

    score = numberOfInfieldPos/30 #30=number particles
    if score <= max(0.0,A.good_threshold_percentage):
      continue
    acceptableActions.append(action)



  #Evaluate by potential field
  sumPotentialList = []
  for action in ActionConsequences:
    sumPotential = 0
    for i in range(0, A.numParticles):
      sumPotential += pf.evaluateAction(action[i].x, action[i].y)

    sumPotentialList.append(sumPotential)

  min = 10000
  minIndex = 0
  for i in range(0, len(sumPotentialList)):
    if(sumPotentialList[i] <  min):
      min = sumPotentialList[i]
      minIndex = i
  return minIndex

def drawActions(): #Todo draw good looking field

  plt.clf()
  h.drawField()

  plotColor = ['ro','go','bo']
  for idx,action in enumerate(ActionConsequences):
    for i in range(0, A.numParticles):
      plt.plot(action[i].x, action[i].y, plotColor[idx])

  plt.pause(0.001)
  #plt.show()

if __name__ == "__main__":

  plt.show(block=False)

  # Robot Position
  pose = m2d.Pose2D(1000.0,2000.0,math.radians(0))

  # Ball Position
  ballPosition = m2d.Vector2()
  ballPosition.x = 0.0
  ballPosition.y = 0.0

  sidekick_right = A.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
  sidekick_left = A.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
  kick_short = A.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

  ActionList = [sidekick_right,sidekick_left, kick_short]

  while True:
    ActionConsequences = []#results for all actions and particles
    ActionResults = []#ActionResults holds the information per particle for one action

    #Simulate Consequences
    for action in ActionList:
      ActionResults = simulateConsequences(action,pose,ballPosition, ActionResults)
      ActionConsequences.append(ActionResults)


    #Decide best action
    #bestAction = decide_smart(ActionConsequences)

    #drawActions()
