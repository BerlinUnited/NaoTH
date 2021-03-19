from naoth.math import *
import math
import field_info as field

'''
This class calculates the raw attack direction the way it's implemented in the PotentialFieldProvider.cpp

'''


def compact_exponential_repeller(target_point, point):
    player_repeller_strenth = 1500
    player_repeller_radius = 2000
    a = player_repeller_strenth
    d = player_repeller_radius

    v = target_point - point
    t = v.abs()  # should be double precision
    if t >= d-100:
        return Vector2(0, 0)

    return v.normalize() * math.exp(a / d - a / (d - t))


def global_exponential_attractor(target_point, point):
    goal_attractor_strength = -0.001
    alpha = goal_attractor_strength
    v = target_point - point
    d = v.abs()

    return v.normalize() * math.exp(alpha * d)


def calculate_potential_field(point, target_point):
    # we are attracted to the target point

    field_f = global_exponential_attractor(target_point, point)
    # we are repelled by the opponents
    player_f = Vector2(0, 0)
    player_f.abs()
    # NOT Implemented yet

    # my self - NOTE this differs from the cpp Implementation in order to factor in the robots own position
    player_f -= compact_exponential_repeller(Vector2(0, 0), point)

    # field_f is of type Vector2
    ff = field_f.abs() * 0.8

    if player_f.abs() > ff:
        player_f = player_f.normalize_length(ff)

    return field_f + player_f
    # return field_f


def get_goal_target(state, point):
    goal_post_offset = 200.0

    # normalized vector from left post to the right
    left_to_right = (state.pose / field.opponent_goalpost_right - state.pose / field.opponent_goalpost_left).normalize()
    # a normal vector pointing from the goal towards the field
    goal_normal = left_to_right
    goal_normal.rotate_right()

    # the endpoints of our line are a shortened version of the goal line

    left_endpoint = (state.pose / field.opponent_goalpost_left) + left_to_right * goal_post_offset
    right_endpoint = (state.pose / field.opponent_goalpost_right) - left_to_right * goal_post_offset

    # this is the goal line we are shooting for
    goal_line = LineSegment(left_endpoint, right_endpoint)

    # project the point on the goal line
    target = goal_line.projection(point)

    # this is the cos of the angle between the vectors (leftEndpoint-point) and (rightEndpoint-point)
    # simple linear algebra: <l-p,r-p>/(||l-p||*||r-p||)
    goal_angle_cos = (state.pose / field.opponent_goalpost_left-point).normalize()*(state.pose / field.opponent_goalpost_right - point).normalize()

    goal_line_offset_front = 100.0
    goal_line_offset_back = 100.0
    # asymetric quadratic scale
    # goalAngleCos = -1 => t = -goalLineOffsetBack
    # goalAngleCos =  1 => t =  goalLineOffsetFront;
    c = (goal_line_offset_front + goal_line_offset_back)*0.5
    v = (goal_line_offset_front - goal_line_offset_back)*0.5
    t = goal_angle_cos*(goal_angle_cos*c + v)

    # move the target depending on the goal opening angle
    target = target + goal_normal.normalize_length(t)

    return target


# Execute:
def get_attack_direction(state):

    ball_relative = state.ball_position  # This is in local coordinates

    target_point = get_goal_target(state, ball_relative)

    attack_direction = calculate_potential_field(ball_relative, target_point)  # ignore obstacle for now
    return attack_direction
