from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
import field_info as field
import tools as tools
import action as a
import potentialfield as pf
import math2d as m2d


def simulate_consequences(action, pose, ball_position):  # Todo Check for Collisions with opp goal and if ball is out

    categorized_ball_pos_list = []

    cat_hist = [0]*len(a.Categories)
    # goal_back_side1 = m2d.LineSegment(oppGoalBackLeft.x, oppGoalBackLeft.y, oppGoalBackRight.x, oppGoalBackRight.y)
    # goal_back_side2 = m2d.LineSegment(opponentGoalPostLeft.x, opponentGoalPostLeft.y, oppGoalBackLeft.x, oppGoalBackLeft.y)
    # goal_back_side3 = m2d.LineSegment(opponentGoalPostRight.x, opponentGoalPostRight.y, oppGoalBackRight.x, oppGoalBackRight.y)

    for i in range(0, a.num_particles):

        new_ball_pos = action.predict(ball_position)
        new_ball_pos_global = pose * new_ball_pos
        # global_ball_pos = pose * ball_position
        # shootline = m2d.LineSegment(global_ball_pos.x, global_ball_pos.y, new_ball_pos_global.x, new_ball_pos_global.y)
        # shootline_length = tools.distance(shootline)

        # if ball is not in field check for collisions with oppGoal and shorten the ball
        # Todo use those checks to shorten the ball later
        # Check if ball hits the goal contruction for each particle
        # Todo compile error
        # intersection1 = h.line_intersection(shootline, goalBackSide1)
        # intersection2 = h.line_intersection(shootline, goalBackSide2)
        # intersection3 = h.line_intersection(shootline, goalBackSide3)

        # Obstacle currently not used

        # Check if particle scores a goal
        intersection_opp_goal = tools.intersect(ball_position, ball_position, field.opponent_goalpost_left, field.opponent_goalpost_right)
        # Check if ball hits the own goal
        intersection_own_goal = tools.intersect(ball_position, ball_position, field.own_goalpost_left, field.own_goalpost_right)
        if intersection_opp_goal and new_ball_pos_global.x < field.opp_goal_back_left and field.opp_goal_back_right < new_ball_pos_global.y < field.opp_goal_back_left and new_ball_pos_global.y:
            category = "OPPGOAL"
        elif -4500 < new_ball_pos_global.x < 4500 and -3000 < new_ball_pos_global.y < 3000:
            category = "INFIELD"
        elif intersection_own_goal:
            category = "OWNGOAL"
        else:
            category = "OUT"
        cat_hist[a.Categories.index(category)] += 1
        categorized_ball_pos_list.append(a.CategorizedBallPosition(new_ball_pos_global, category))

    return categorized_ball_pos_list, cat_hist


def decide_smart(action_consequences, cat_hist):  # does not behave like cpp function

    number_infield_pos = 0
    acceptable_actions = []
    goal_actions = []
    # score = 0
    for action in action_consequences:
        for i in range(0, a.num_particles):
            if action[i].category == "OWNGOAL":
                continue
            elif action[i].category == "INFIELD" or action[i].category == "OPPGOAL":
                number_infield_pos += 1

        score = number_infield_pos/30  # 30=number particles
        if score <= max(0.0, a.good_threshold_percentage):
            continue
        acceptable_actions.append(action)

    for action in acceptable_actions:
        if not goal_actions:
            goal_actions.append(action)
            continue

    # Evaluate by potential field
    sum_potential_list = []
    for action in action_consequences:
        sum_potential = 0
        for i in range(0, a.num_particles):
            sum_potential += pf.evaluate_action(action[i].ball_pos.x, action[i].ball_pos.y)

        sum_potential_list.append(sum_potential)

    min_potential = 10000
    min_index = 0
    for i in range(0, len(sum_potential_list)):
        if sum_potential_list[i] < min_potential:
            min_potential = sum_potential_list[i]
            min_index = i
    return min_index


def draw_actions(action_consequences):

    plt.clf()
    tools.draw_field()
    ax = plt.gca()
    ax.add_artist(Circle(xy=(1000, 2000), radius=100, fill=False, edgecolor='white'))  # should be robot pos * ball pos
    plot_color = ['ro', 'go', 'bo']
    for idx, action in enumerate(action_consequences):
        for i in range(0, a.num_particles):
            plt.plot(action[i].ball_pos.x, action[i].ball_pos.y, plot_color[idx])

    plt.pause(0.001)


def main():
    plt.show(block=False)

    # Robot Position
    pose = m2d.Pose2D()
    pose.translation = m2d.Vector2(1000, 2000)
    pose.rotation = math.radians(0)
    # Ball Position
    ball_position = m2d.Vector2()
    ball_position.x = 0.0
    ball_position.y = 0.0

    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

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
        # best_action = decide_smart(action_consequences, cat_hist)
        # print action_list[best_action].name

        draw_actions(action_consequences)
        # break


if __name__ == "__main__":
    main()
