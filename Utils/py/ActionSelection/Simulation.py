from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
import field_info as field
import tools as tools
import action as a
import potential_field as pf
import math2d as m2d


def simulate_consequences(action, pose, ball_position):

    categorized_ball_pos_list = []
    cat_hist = [0]*len(a.Categories)

    # calculate the own goal line
    own_goal_dir = (field.own_goalpost_right - field.own_goalpost_left).normalize()

    own_left_endpoint = field.own_goalpost_left + own_goal_dir*(field.goalpost_radius + field.ball_radius)
    own_right_endpoint = field.own_goalpost_right - own_goal_dir*(field.goalpost_radius + field.ball_radius)

    own_goal_line_global = m2d.LineSegment(own_left_endpoint, own_right_endpoint)

    # calculate opponent goal lines and box
    opp_goal_back_left = m2d.Vector2(field.opponent_goalpost_left.x + field.goal_depth, field.opponent_goalpost_left.y)
    opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)

    # Maybe add list of goal backsides here
    goal_backsides = ([])
    goal_backsides.append(m2d.LineSegment(opp_goal_back_left, opp_goal_back_right))
    goal_backsides.append(m2d.LineSegment(field.opponent_goalpost_left, opp_goal_back_left))
    goal_backsides.append(m2d.LineSegment(field.opponent_goalpost_right, opp_goal_back_right))

    opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

    # current ball position
    global_ball_start_position = pose * ball_position

    # virtual ultrasound obstacle line
    obstacle_line = m2d.LineSegment(pose * m2d.Vector2(400, 200), pose * m2d.Vector2(400, -200))

    # now generate predictions and categorize
    for i in range(0, a.num_particles):
        # predict and calculate shoot line
        global_ball_end_position = pose * action.predict(ball_position)

        shootline = m2d.LineSegment(global_ball_start_position, global_ball_end_position)

        # check if collision detection with goal has to be performed
        # if the ball start and end positions are inside of the field, you don't need to check
        collision_with_goal = False
        t_min = 0  # dummy value
        if not field.field_rect.inside(global_ball_end_position) or not field.field_rect.inside(global_ball_start_position):
            t_min = shootline.length
            for side in goal_backsides:
                t = shootline.intersection(side)
                if 0 <= t < t_min and side.intersect(shootline):
                    t_min = t
                    collision_with_goal = True

            print(collision_with_goal)  # Todo Test if its working correctly - It's not

        # if there are collisions with the back goal lines, calulate where the ball will stop
        if collision_with_goal:
            shootline = m2d.LineSegment(global_ball_start_position, shootline.point(t_min-field.ball_radius))
            global_ball_end_position = shootline.end()

        # Obstacle Detection
        obstacle_collision = False

        if opp_goal_box.inside(global_ball_end_position):
            category = "OPPGOAL"
        # Todo add Collision
        elif obstacle_collision and obstacle_line.intersect(shootline) and shootline.intersect(obstacle_line):
            category = "COLLISION"
        elif (field.field_rect.inside(global_ball_end_position) or
                global_ball_end_position.x <= field.opponent_goalpost_right.x and
                field.opponent_goalpost_left.y < global_ball_end_position.y > field.opponent_goalpost_right.y):
            category = "INFIELD"
        elif shootline.intersect(own_goal_line_global) and own_goal_line_global.intersect(shootline):
            category = "OWNGOAL"
        elif global_ball_end_position.x > field.x_opponent_groundline:
            category = "OPPOUT"
        elif global_ball_end_position.x < field.x_opponent_groundline:
            category = "OWNOUT"
        elif global_ball_end_position.y > field.y_left_sideline:
            category = "LEFTOUT"
        elif global_ball_end_position.y < field.y_right_sideline:
            category = "RIGHTOUT"
        cat_hist[a.Categories.index(category)] += 1
        # Todo: write ball end pos in local coordinates
        categorized_ball_pos_list.append(a.CategorizedBallPosition(global_ball_end_position, category))

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
