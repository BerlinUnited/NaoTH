from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
from fieldInfo import *  # Maybe bad
import helperFunctions as h
import Action as A
import potentialField as pf


def simulate_consequences(action, pose, ball_position):  # Todo Check for Collisions with opp goal and if ball is out

    categorized_ball_pos_list = []

    cat_hist = [0]*len(A.Categories)
    goal_back_side1 = m2d.LineSegment(oppGoalBackLeft.x,oppGoalBackLeft.y,oppGoalBackRight.x,oppGoalBackRight.y)
    goal_back_side2 = m2d.LineSegment(opponentGoalPostLeft.x,opponentGoalPostLeft.y,oppGoalBackLeft.x,oppGoalBackLeft.y)
    goal_back_side3 = m2d.LineSegment(opponentGoalPostRight.x,opponentGoalPostRight.y,oppGoalBackRight.x,oppGoalBackRight.y)

    for i in range(0, A.numParticles):

        new_ball_pos = action.predict(ball_position)
        new_ball_pos_global = pose * new_ball_pos
        global_ball_pos = pose * ball_position
        shootline = m2d.LineSegment(global_ball_pos.x, global_ball_pos.y, new_ball_pos_global.x, new_ball_pos_global.y)
        shootline_length = h.distance(shootline)

        # if ball is not in field check for collisions with oppGoal and shorten the ball
        # Todo use those checks to shorten the ball later
        # Check if ball hits the goal contruction for each particle
        # Todo compile error
        # intersection1 = h.line_intersection(shootline, goalBackSide1)
        # intersection2 = h.line_intersection(shootline, goalBackSide2)
        # intersection3 = h.line_intersection(shootline, goalBackSide3)

        # Obstacle currently not used

        # Check if particle scores a goal
        intersectionOppGoal = h.intersect(ball_position,ball_position, opponentGoalPostLeft,opponentGoalPostRight)
        # Check if ball hits the own goal
        intersectionOwnGoal = h.intersect(ball_position,ball_position , ownGoalPostLeft,ownGoalPostRight)
        category = "INFIELD"
        if intersectionOppGoal and new_ball_pos_global.x < oppGoalBackLeft and oppGoalBackRight <new_ball_pos_global.y < oppGoalBackLeft and new_ball_pos_global.y:
            category = "OPPGOAL"
        elif -4500 < new_ball_pos_global.x < 4500  and -3000 < new_ball_pos_global.y < 3000:
            category = "INFIELD"
        elif intersectionOwnGoal:
            category = "OWNGOAL"
        else:
            category = "OUT"
        cat_hist[A.Categories.index(category)] += 1
        categorized_ball_pos_list.append(A.CategorizedBallPosition(new_ball_pos_global,category))

    return categorized_ball_pos_list,cat_hist


def decide_smart(action_consequences, cat_hist):

    numberOfInfieldPos = 0
    acceptableActions = []
    goalActions = []
    score = 0
    for action in action_consequences:
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
    for action in action_consequences:
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


def draw_actions(action_consequences):

    plt.clf()
    h.drawField()
    ax = plt.gca()
    ax.add_artist(Circle(xy=(1000, 2000), radius=100, fill=False, edgecolor='white'))  # should be robot pos * ball pos
    plot_color = ['ro', 'go', 'bo']
    for idx,action in enumerate(action_consequences):
        for i in range(0, A.numParticles):
            plt.plot(action[i].ballPos.x, action[i].ballPos.y, plot_color[idx])

    plt.pause(0.001)


def main():
    plt.show(block=False)

    # Robot Position
    pose = m2d.Pose2D(1000.0, 2000.0, math.radians(0))  # unexpected argument?? why

    # Ball Position
    ball_position = m2d.Vector2()
    ball_position.x = 0.0
    ball_position.y = 0.0

    sidekick_right = A.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    sidekick_left = A.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    kick_short = A.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

    action_list = [sidekick_right, sidekick_left, kick_short]

    while True:
        action_consequences = []  # results for all actions and particles
        cat_hist = []  # histogram of results for all actions

        # Simulate Consequences
        for action in action_list:
            categorized_ball_pos_list, hist = simulate_consequences(action, pose, ball_position)

            action_consequences.append(categorized_ball_pos_list)
            cat_hist.append(hist)  # Todo use that somewhere

        # Decide best action
        best_action = decide_smart(action_consequences, cat_hist)
        print action_list[best_action].name

        draw_actions(action_consequences)
        # break


if __name__ == "__main__":
    main()


