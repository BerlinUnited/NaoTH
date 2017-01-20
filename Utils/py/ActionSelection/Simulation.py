from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
from fieldInfo import *  # Maybe bad
import helperFunctions as h

import Action as A
import potentialField as pf


def simulate_consequences(action, pose, ballPosition):  # Todo Check for Collisions with opp goal and if ball is out

    categorizedBallPosList = []

    catHist = [0]*len(A.Categories)
    goalBackSide1 = m2d.LineSegment(oppGoalBackLeft.x,oppGoalBackLeft.y,oppGoalBackRight.x,oppGoalBackRight.y)
    goalBackSide2 = m2d.LineSegment(opponentGoalPostLeft.x,opponentGoalPostLeft.y,oppGoalBackLeft.x,oppGoalBackLeft.y)
    goalBackSide3 = m2d.LineSegment(opponentGoalPostRight.x,opponentGoalPostRight.y,oppGoalBackRight.x,oppGoalBackRight.y)

    for i in range(0, A.numParticles):

        newBallPos = action.predict(ballPosition)
        newBallPosGlobal = pose * newBallPos
        globalBallPos = pose * ballPosition
        shootline = m2d.LineSegment(globalBallPos.x,globalBallPos.y,newBallPosGlobal.x,newBallPosGlobal.y)
        shootlineLength = h.distance(shootline)

        # if ball is not in field check for collisions with oppGoal and shorten the ball
        # Todo use those checks to shorten the ball later
        # Check if ball hits the goal contruction for each particle
        # Todo compile error
        # intersection1 = h.line_intersection(shootline, goalBackSide1)
        # intersection2 = h.line_intersection(shootline, goalBackSide2)
        # intersection3 = h.line_intersection(shootline, goalBackSide3)

        # Obstacle currently not used

        # Check if particle scores a goal
        intersectionOppGoal = h.intersect(ballPosition,ballPosition, opponentGoalPostLeft,opponentGoalPostRight)
        # Check if ball hits the own goal
        intersectionOwnGoal = h.intersect(ballPosition,ballPosition , ownGoalPostLeft,ownGoalPostRight)
        category = "INFIELD"
        if intersectionOppGoal and newBallPosGlobal.x < oppGoalBackLeft and oppGoalBackRight <newBallPosGlobal.y < oppGoalBackLeft and newBallPosGlobal.y:
            category = "OPPGOAL"
        elif -4500 < newBallPosGlobal.x < 4500  and -3000 < newBallPosGlobal.y < 3000:
            category = "INFIELD"
        elif intersectionOwnGoal:
            category = "OWNGOAL"
        else:
            category = "OUT"
        catHist[A.Categories.index(category)] += 1
        categorizedBallPosList.append(A.CategorizedBallPosition(newBallPosGlobal,category))

    return categorizedBallPosList,catHist


def decide_smart(ActionConsequences,catHist):

    numberOfInfieldPos = 0
    acceptableActions = []
    goalActions = []
    score = 0
    for action in ActionConsequences:
        for i in range(0, A.numParticles):
            if action[i].category == "OWNGOAL":
                continue
            elif action[i].category == "INFIELD" or action[i].category == "OPPGOAL":
              numberOfInfieldPos += 1

        score = numberOfInfieldPos/30  # 30=number particles
        if score <= max(0.0,A.good_threshold_percentage):
            continue
        acceptableActions.append(action)

        for action in acceptableActions:
            if goalActions == []:
                goalActions.append(action)
                continue

    # Evaluate by potential field
    sumPotentialList = []
    for action in ActionConsequences:
        sumPotential = 0
        for i in range(0, A.numParticles):
            sumPotential += pf.evaluateAction(action[i].ballPos.x, action[i].ballPos.y)

        sumPotentialList.append(sumPotential)

    min = 10000
    minIndex = 0
    for i in range(0, len(sumPotentialList)):
        if(sumPotentialList[i] <  min):
            min = sumPotentialList[i]
            minIndex = i
    return minIndex


def draw_actions(ActionConsequences):

    plt.clf()
    h.drawField()
    ax = plt.gca()
    ax.add_artist(Circle(xy=(1000, 2000), radius=100, fill=False, edgecolor='white'))  # should be robotpos * ballpos
    plot_color = ['ro', 'go', 'bo']
    for idx,action in enumerate(ActionConsequences):
        for i in range(0, A.numParticles):
            plt.plot(action[i].ballPos.x, action[i].ballPos.y, plot_color[idx])

    plt.pause(0.001)
    # plt.show()

if __name__ == "__main__":

    plt.show(block=False)

    # Robot Position
    pose = m2d.Pose2D(1000.0, 2000.0, math.radians(0))  # unexpected argument?? why

    # Ball Position
    ballPosition = m2d.Vector2()
    ballPosition.x = 0.0
    ballPosition.y = 0.0

    sidekick_right = A.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    sidekick_left = A.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    kick_short = A.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

    ActionList = [sidekick_right, sidekick_left, kick_short]

    while True:
        categorizedBallPosList = []  # Each Entry holds the information per particle for one action
        Action_consequences = []  # results for all actions and particles
        cat_hist = []  # histogram of results for all actions

        # Simulate Consequences
        for action in ActionList:
            categorizedBallPosList, hist = simulate_consequences(action, pose, ballPosition)

            Action_consequences.append(categorizedBallPosList)
            cat_hist.append(hist)  # Todo use that somewhere

        # Decide best action
        bestAction = decide_smart(Action_consequences, cat_hist)
        print ActionList[bestAction].name

        draw_actions(Action_consequences)
        # break
