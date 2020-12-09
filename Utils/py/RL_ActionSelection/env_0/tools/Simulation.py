import numpy as np
from action import Category
from action import ActionResults

import field_info as field
import potential_field as pf
from naoth import math2d as m2d

good_threshold_percentage = 0.85
minGoalLikelihood = 0.3


def simulate_consequences(action, categorized_ball_positions, state, num_particles):
    categorized_ball_positions.reset()

    # calculate the own goal line
    own_goal_dir = (field.own_goalpost_right - field.own_goalpost_left).normalize()

    own_left_endpoint = field.own_goalpost_left + own_goal_dir * (
                field.goalpost_radius + field.ball_radius)
    own_right_endpoint = field.own_goalpost_right - own_goal_dir * (
                field.goalpost_radius + field.ball_radius)

    own_goal_line_global = m2d.LineSegment(own_left_endpoint, own_right_endpoint)

    # calculate opponent goal lines and box
    opp_goal_back_left = m2d.Vector2(field.opponent_goalpost_left.x + field.goal_depth,
                                     field.opponent_goalpost_left.y)
    opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth,
                                      field.opponent_goalpost_right.y)

    # Maybe add list of goal backsides here
    goal_backsides = ([])
    goal_backsides.append(m2d.LineSegment(opp_goal_back_left, opp_goal_back_right))
    goal_backsides.append(m2d.LineSegment(field.opponent_goalpost_left, opp_goal_back_left))
    goal_backsides.append(m2d.LineSegment(field.opponent_goalpost_right, opp_goal_back_right))

    opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

    # current ball position
    global_ball_start_position = state.pose * state.ball_position

    # virtual ultrasound obstacle line
    obstacle_line = m2d.LineSegment(state.pose * m2d.Vector2(400, 200),
                                    state.pose * m2d.Vector2(400, -200))

    mean_test_list_x = []
    mean_test_list_y = []

    # now generate predictions and categorize
    for i in range(0, num_particles):
        # predict and calculate shoot line
        global_ball_end_position = state.pose * action.predict(state.ball_position, True)

        shootline = m2d.LineSegment(global_ball_start_position, global_ball_end_position)

        # check if collision detection with goal has to be performed
        # if the ball start and end positions are inside of the field, you don't need to check
        collision_with_goal = False
        t_min = 0  # dummy value
        if not field.field_rect.inside(global_ball_end_position) or not field.field_rect.inside(
                global_ball_start_position):
            t_min = shootline.length
            for side in goal_backsides:
                t = shootline.line_intersection(side)
                if 0 <= t < t_min and side.intersect(shootline):
                    t_min = t
                    collision_with_goal = True

        # if there are collisions with the back goal lines, calculate where the ball will stop
        if collision_with_goal:
            global_ball_end_position = shootline.point(t_min - field.ball_radius)
            shootline = m2d.LineSegment(global_ball_start_position, global_ball_end_position)

        # Obstacle Detection
        obstacle_collision = False
        # TODO: fix obstacle collision somehow
        '''
        for obstacle in state.obstacle_list:
            dist = math.sqrt((state.pose.translation.x-obstacle.x)**2 + (state.pose.translation.y-obstacle.y)**2)
            # check for distance and rotation
            # Todo it's wrong: Now if obstacle is near, then obstacle is in front of the robot
            if dist < 400 and shootline.intersect(obstacle_line):
                obstacle_collision = True
        '''
        if opp_goal_box.inside(global_ball_end_position):
            category = Category.OPPGOAL
        elif obstacle_collision and obstacle_line.intersect(shootline) and shootline.intersect(
                obstacle_line):
            category = Category.COLLISION
        elif (field.field_rect.inside(global_ball_end_position) or
              (global_ball_end_position.x <= field.opponent_goalpost_right.x and
               field.opponent_goalpost_left.y > global_ball_end_position.y > field.opponent_goalpost_right.y)):
            category = Category.INFIELD
        elif shootline.intersect(own_goal_line_global) and own_goal_line_global.intersect(
                shootline):
            category = Category.OWNGOAL
        elif global_ball_end_position.x > field.x_opponent_groundline:
            category = Category.OPPOUT
        elif global_ball_end_position.x < field.x_opponent_groundline:
            category = Category.OWNOUT
        elif global_ball_end_position.y > field.y_left_sideline:
            category = Category.LEFTOUT
        elif global_ball_end_position.y < field.y_right_sideline:
            category = Category.RIGHTOUT
        else:
            category = Category.INFIELD

        local_test_pos = state.pose / global_ball_end_position
        mean_test_list_x.append(local_test_pos.x)
        mean_test_list_y.append(local_test_pos.y)

        categorized_ball_positions.add(state.pose / global_ball_end_position, category)

    # calculate the most likely ball position in a separate simulation run
    categorized_ball_positions.expected_ball_pos_mean = m2d.Vector2(np.mean(mean_test_list_x),
                                                                    np.mean(mean_test_list_y))
    categorized_ball_positions.expected_ball_pos_median = m2d.Vector2(np.median(mean_test_list_x),
                                                                      np.median(mean_test_list_y))
    return categorized_ball_positions


def simulateAction(action, state, num_particles):
    result = ActionResults([])

    # current ball position
    global_ball_start = state.pose * state.ball_position

    opp_goal_backsides = [
        m2d.LineSegment(field.opp_goal_back_left, field.opp_goal_back_right),
        m2d.LineSegment(field.opponent_goalpost_left, field.opp_goal_back_left),
        m2d.LineSegment(field.opponent_goalpost_right, field.opp_goal_back_right)
    ]

    own_goal_backsides = [
        m2d.LineSegment(field.own_goal_back_left, field.own_goal_back_right),
        m2d.LineSegment(field.own_goalpost_left, field.own_goal_back_left),
        m2d.LineSegment(field.own_goalpost_right, field.own_goal_back_right)
    ]

    # now generate predictions and categorize
    for i in range(0, num_particles):

        # predict and calculate shoot line
        global_ball_end = state.pose * action.predict(state.ball_position, True)

        # ball is crossing a field line
        if not field.field_rect.inside(global_ball_end) or not field.field_rect.inside(
                global_ball_start):
            global_ball_end, _ = calculateCollision(opp_goal_backsides, global_ball_start,
                                                    global_ball_end)
            global_ball_end, _ = calculateCollision(own_goal_backsides, global_ball_start,
                                                    global_ball_end)

        category = classifyBallPosition(global_ball_end)

        result.add(state.pose / global_ball_end, category)

    # calculate the most likely ball position in a separate simulation run
    ball_pos_array = [[p.pos().x, p.pos().y] for p in result.positions()]
    mean = np.mean(ball_pos_array, axis=0)
    median = np.median(ball_pos_array, axis=0)
    result.expected_ball_pos_mean = m2d.Vector2(mean[0], mean[1])
    result.expected_ball_pos_median = m2d.Vector2(median[0], median[1])

    return result


def calculateCollision(lines, start, end):
    motionLine = m2d.LineSegment(start, end)
    t_min = motionLine.length

    collision = False
    for segment in lines:
        t = motionLine.line_intersection(segment)
        if 0 <= t < t_min and segment.intersect(motionLine):
            t_min = t
            collision = True

    # if there are collisions with the back goal lines, calculate where the ball will stop
    if collision:
        return motionLine.point(t_min - field.ball_radius), True
    else:
        return end, False


def classifyBallPosition(global_ball_position):
    if field.opp_goal_box.inside(global_ball_position):
        category = Category.OPPGOAL
    elif field.own_goal_box.inside(global_ball_position):
        category = Category.OWNGOAL
    elif field.field_rect.inside(global_ball_position):
        category = Category.INFIELD
    elif global_ball_position.x > field.x_opponent_groundline:
        category = Category.OPPOUT
    elif global_ball_position.x < field.x_opponent_groundline:
        category = Category.OWNOUT
    elif global_ball_position.y > field.y_left_sideline:
        category = Category.LEFTOUT
    elif global_ball_position.y < field.y_right_sideline:
        category = Category.RIGHTOUT
    else:
        category = Category.INFIELD

    return category


def decide_minimal(actions_consequences, state):
    # choose potentialfield function
    if state.potential_field_function == "influence_01":
        evaluate = pf.evaluate_action_with_robots
    elif state.potential_field_function == "normal":
        # normal potentialfield without influence regions
        evaluate = pf.evaluate_action
    elif state.potential_field_function == "generated":
        # use generated potential field values
        evaluate = pf.evaluate_action_gen_field
    else:
        # normal potentialfield without influence regions
        evaluate = pf.evaluate_action

    best_goal_idx = None
    best_goal_value = None
    best_goal_likelihood = None

    for i, results in enumerate(actions_consequences):

        score = results.likelihood(Category.INFIELD) + results.likelihood(Category.OPPGOAL)
        if score < max(0.0, good_threshold_percentage):
            continue

        value = -evaluate(results, state)
        goal_likelihood = results.likelihood(Category.OPPGOAL)

        if best_goal_idx is None:
            best_goal_idx, best_goal_value = i, value

        if ((goal_likelihood > best_goal_likelihood or
             (goal_likelihood == best_goal_likelihood and value > best_goal_value))):
            best_goal_idx, best_goal_value, best_goal_likelihood = i, value, goal_likelihood

    if best_goal_idx is not None:
        return best_goal_idx
    else:
        # HACK
        return 0


def decide_smart(actions_consequences, state):
    # choose potentialfield function
    if state.potential_field_function == "influence_01":
        evaluate = pf.evaluate_action_with_robots
    elif state.potential_field_function == "normal":
        # normal potentialfield without influence regions
        evaluate = pf.evaluate_action
    # TODO fix generated field in potential_field.py
    # elif state.potential_field_function == "generated":
    #    # use generated potential field values
    #    evaluate = pf.evaluate_action_gen_field
    else:
        # normal potentialfield without influence regions
        evaluate = pf.evaluate_action

    acceptable_actions = []
    # select acceptable actions
    for i, results in enumerate(actions_consequences):
        # if an own-goal is detected, ignore the action
        if results.category(Category.OWNGOAL) > 0:
            continue

        # ignore actions with too high chance of kicking out
        score = results.likelihood(Category.INFIELD) + results.likelihood(Category.OPPGOAL)
        if score < max(0.0, good_threshold_percentage):
            # print("Threshold is too low for action: " + str(i) + "with score: " + str(score) )
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
        # if results.category(Category.OPPGOAL) < a.minGoalParticles:
        if results.likelihood(Category.OPPGOAL) < minGoalLikelihood:
            continue

        # there is no other action to compare yet
        if len(goal_actions) == 0:
            goal_actions.append(index)
            continue

        # the action with the highest chance of scoring the goal is the best
        if actions_consequences[goal_actions[0]].category(Category.OPPGOAL) < results.category(
                Category.OPPGOAL):
            goal_actions = ([])
            goal_actions.append(index)
            continue

        if actions_consequences[goal_actions[0]].category(Category.OPPGOAL) == results.category(
                Category.OPPGOAL):
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
            potential = evaluate(actions_consequences[index], state)
            if potential < best_value:
                best_action = index
                best_value = potential
        return best_action

    # find min of goalActions
    best_action = 0
    best_value = float("inf")  # assuming potential is [0.0, inf]
    for index in goal_actions:
        potential = evaluate(actions_consequences[index], state)
        if potential < best_value:
            best_action = index
            best_value = potential

    return best_action
