import math2d as m2d
xLength = 9000
yLength = 6000

xFieldLength = xLength + 2*700
yFieldLength = yLength + 2*700

xPenaltyAreaLength = 600
yPenaltyAreaLength = 2200

centerCircleRadius = 600
fieldLinesWidth = 50

goalWidth = 1400
goalHeight = 800
goalDepth = 500
goalpostRadius = 50
xPenaltyMarkDistance = 1300

xPosOpponentGroundline = xLength / 2
xPosOwnGroundline = -xPosOpponentGroundline

xPosOpponentGoal = xPosOpponentGroundline
xPosOwnGoal = -xPosOpponentGoal

xPosOpponentPenaltyArea =  xPosOpponentGroundline - xPenaltyAreaLength
xPosOwnPenaltyArea =      -xPosOpponentPenaltyArea

yPosLeftGoalpost =         goalWidth / 2.0
yPosRightGoalpost =       -yPosLeftGoalpost

yPosLeftPenaltyArea =      yPenaltyAreaLength / 2.0
yPosRightPenaltyArea =    -yPosLeftPenaltyArea

yPosLeftSideline =         yLength / 2.0
yPosRightSideline =       -yPosLeftSideline


opponentGoalPostLeft = m2d.Vector2(xPosOpponentGoal+25,yPosLeftGoalpost)
opponentGoalPostRight = m2d.Vector2(xPosOpponentGoal+25,yPosRightGoalpost)

ownGoalPostLeft = m2d.Vector2(xPosOwnGoal+25,yPosLeftGoalpost)
ownGoalPostRight = m2d.Vector2(xPosOwnGoal+25,yPosRightGoalpost)

#From Simulation.cpp
oppGoalBackLeft = m2d.Vector2(opponentGoalPostLeft.x + goalDepth, opponentGoalPostLeft.y)
oppGoalBackRight= m2d.Vector2(opponentGoalPostRight.x + goalDepth,opponentGoalPostRight.y)


