import math2d as m2d

ball_radius = 100

x_length = 9000
y_length = 6000
x_field_length = x_length + 2*700
y_field_length = y_length + 2*700
x_penalty_area_length = 600
y_penalty_area_length = 2200
center_circle_radius = 600
x_penalty_mark_distance = 1300

field_lines_width = 50

goal_width = 1400
goal_height = 800
goal_depth = 500
goalpost_radius = 50
x_opponent_groundline = x_length / 2
x_own_groundline = -x_opponent_groundline
x_opponent_goal = x_opponent_groundline
x_own_goal = -x_opponent_goal
x_opponent_penalty_area = x_opponent_groundline - x_penalty_area_length
x_own_penalty_area = -x_opponent_penalty_area

y_left_goalpost = goal_width / 2.0
y_right_goalpost = -y_left_goalpost

y_left_penalty_area = y_penalty_area_length / 2.0
y_right_penalty_area = -y_left_penalty_area

y_left_sideline = y_length / 2.0
y_right_sideline = -y_left_sideline

opponent_goalpost_left = m2d.Vector2(x_opponent_goal+25, y_left_goalpost)
opponent_goalpost_right = m2d.Vector2(x_opponent_goal+25, y_right_goalpost)

own_goalpost_left = m2d.Vector2(x_own_goal-25, y_left_goalpost)
own_goalpost_right = m2d.Vector2(x_own_goal-25, y_right_goalpost)

# From Simulation.cpp
opp_goal_back_left = m2d.Vector2(opponent_goalpost_left.x + goal_depth, opponent_goalpost_left.y)
opp_goal_back_right = m2d.Vector2(opponent_goalpost_right.x + goal_depth, opponent_goalpost_right.y)


field_rect = m2d.Rect2d(m2d.Vector2(-x_length*0.5, -y_length*0.5), m2d.Vector2(x_length*0.5, y_length*0.5))


