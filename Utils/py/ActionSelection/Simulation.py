from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
import field_info as field
import tools as tools
import action as a
import potential_field as pf
import math2d as m2d
import numpy as np


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(4200, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates
        self.obstacle_list.append(m2d.Vector2(2000, 2000))


def simulate_consequences(action, categorized_ball_positions, state):

    categorized_ball_positions.reset()

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
    global_ball_start_position = state.pose * state.ball_position

    # virtual ultrasound obstacle line
    obstacle_line = m2d.LineSegment(state.pose * m2d.Vector2(400, 200), state.pose * m2d.Vector2(400, -200))

    # now generate predictions and categorize
    for i in range(0, a.num_particles):
        # predict and calculate shoot line
        global_ball_end_position = state.pose * action.predict(state.ball_position, False)

        shootline = m2d.LineSegment(global_ball_start_position, global_ball_end_position)

        # check if collision detection with goal has to be performed
        # if the ball start and end positions are inside of the field, you don't need to check
        collision_with_goal = False
        t_min = 0  # dummy value
        if not field.field_rect.inside(global_ball_end_position) or not field.field_rect.inside(global_ball_start_position):
            t_min = shootline.length
            for side in goal_backsides:
                t = shootline.line_intersection(side)
                if 0 <= t < t_min:
                    t_min = t
                    collision_with_goal = True

        # if there are collisions with the back goal lines, calulate where the ball will stop
        if collision_with_goal:
            shootline = m2d.LineSegment(global_ball_start_position, shootline.point(t_min-field.ball_radius))
            global_ball_end_position = shootline.end()

        # Obstacle Detection
        obstacle_collision = False
        for obstacle in state.obstacle_list:
            dist = math.sqrt((state.pose.translation.x-obstacle.x)**2 - (state.pose.translation.y-obstacle.y)**2)
            # check for distance and rotation
            if dist < 400 and shootline.intersect(obstacle_line):
                obstacle_collision = False

        category = "INFIELD"
        if opp_goal_box.inside(global_ball_end_position):
            category = "OPPGOAL"
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

        categorized_ball_positions.add(state.pose / global_ball_end_position, category)

    return categorized_ball_positions


def decide_smart(actions_consequences, state):

    acceptable_actions = []
    # select acceptable actions
    for i, results in enumerate(actions_consequences):
        # if an own-goal is detected, ignore the action
        if results.category("OWNGOAL") > 0:
            continue

        # ignore actions with too high chance of kicking out
        score = results.likelihood("INFIELD") + results.likelihood("OPPGOAL")
        if score <= max(0.0, a.good_threshold_percentage):
            continue

        # all actions which are not too bad
        acceptable_actions.append(i)

    # no acceptable actions
    if len(acceptable_actions) == 0:
        return 0  # assumes 0 is the turn action

    # there is a clear decision
    if len(acceptable_actions) == 1:
        return acceptable_actions[0]

    # select the subset of actions with highest goal chance
    goal_actions = []
    for index in acceptable_actions:
        results = actions_consequences[index]

        # chance of scoring a goal must be significant
        if results.category("OPPGOAL") < a.minGoalParticles:
            continue

        # there is no other action to compare yet
        if len(goal_actions) == 0:
            goal_actions.append(index)
            continue

        # the action with the highest chance of scoring the goal is the best
        if actions_consequences[goal_actions[0]].categorie("OPPGOAL") < results.categorie("OPPGOAL"):
            goal_actions = ([])
            goal_actions.append(index)
            continue

        if actions_consequences[goal_actions[0]].categorie("OPPGOAL") == results.categorie("OPPGOAL"):
            goal_actions.append(index)
            continue

    # go for the goal action!
    if len(goal_actions) == 1:
        return goal_actions[0]

    # no goal actions => select one of the acceptable actions based on strategic value
    if len(goal_actions) == 0:
        best_action = 0
        best_value = float("inf")  # assuming potential is [0.0, inf]
        for index in acceptable_actions:
            potential = pf.evaluate_action2(actions_consequences[index], state)
            if potential < best_value:
                best_action = index
                best_value = potential
        return best_action

    # find min of goalActions
    best_action = 0
    best_value = float("inf")  # assuming potential is [0.0, inf]
    for index in goal_actions:
        potential = pf.evaluate_action2(actions_consequences[index], state)
        if potential < best_value:
            best_action = index
            best_value = potential
    return best_action


def draw_actions(actions_consequences, state):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    x = np.array([])
    y = np.array([])

    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.0001)


def main():
    state = State()

    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

    action_list = [sidekick_right, sidekick_left, kick_short]


    while True:
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(simulate_consequences(action, single_consequence, state))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = decide_smart(actions_consequences, state)
        # print(action_list[best_action].name)

        draw_actions(actions_consequences, state)


if __name__ == "__main__":
    main()
