import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
import copy


def main():
    plt.clf()
    tools.draw_field(plt.gca())
    axes = plt.gca()

    # Create a list of line segments
    linesegment_list = []
    leftpoint01 = m2d.Vector2(2000, 3000)
    rightpoint01 = m2d.Vector2(2000, -3000)
    bouncing_border01 = m2d.LineSegment(leftpoint01, rightpoint01)
    linesegment_list.append(bouncing_border01)

    leftpoint02 = m2d.Vector2(-1000, 2000)
    rightpoint02 = m2d.Vector2(-1000, -2000)
    bouncing_border02 = m2d.LineSegment(leftpoint02, rightpoint02)
    #linesegment_list.append(bouncing_border02)

    # Draw borders
    axes.plot([leftpoint01.x, rightpoint01.x], [leftpoint01.y, rightpoint01.y], 'black')
    axes.plot([leftpoint02.x, rightpoint02.x], [leftpoint02.y, rightpoint02.y], 'black')

    ball_start = m2d.Vector2(0, 0)
    ball_end = m2d.Vector2(3000, 1000)  # Hack set length of shootline here

    # draw start position
    axes.add_artist(Circle(xy=(ball_start.x, ball_start.y), radius=100, fill=False, edgecolor='white'))

    collision_assumed = True
    while collision_assumed:
        shootline = m2d.LineSegment(ball_start, ball_end)
        shootline_length = math.hypot(ball_end.x - ball_start.x,
                                      ball_end.y - ball_start.y)
        collision_flag = False
        t_min = shootline.length
        for i, border in enumerate(linesegment_list):
            # Check collision with each border
            t = shootline.line_intersection(border)
            if 0 <= t < t_min and border.intersect(shootline):
                t_min = t
                collision_flag = True
                print("Collides with border : ", i, "at pos: ", shootline.point(t_min), t_min)

        if collision_flag:
            # TODO: check if it is always okay to subtract ball radius???
            # should always shorten the shootline by the ball_radius
            intersection_point = shootline.point(t_min - field.ball_radius)

            axes.add_artist(Circle(xy=(intersection_point.x, intersection_point.y), radius=100, fill=False, edgecolor='pink'))
            axes.plot([ball_start.x, intersection_point.x], [ball_start.y, intersection_point.y], 'red')
            # calculate new endpoint
            new_shootline_length = math.hypot(ball_end.x - intersection_point.x,
                                              ball_end.y - intersection_point.y)

            shootline_angle = math.atan2(intersection_point.y - ball_start.y,
                                         intersection_point.x - ball_start.x)
            print(math.degrees(shootline_angle), math.degrees(math.pi - 2*(0.5 * math.pi - shootline_angle)))
            bounce = intersection_point + (m2d.Vector2(new_shootline_length, 0)).rotate(math.pi - 2*shootline_angle)

            # rotate new vector by the amount of the original shootline
            bounce = bounce.rotate(shootline_angle)

            # set ball_start and ball_end
            ball_start = intersection_point
            ball_end = bounce
            collision_flag = False
        else:
            # print("collision_assumed")
            # print("Ball-Start: ", ball_start, "Ball-End: ", ball_end)

            axes.plot([ball_start.x, ball_end.x], [ball_start.y, ball_end.y], 'blue')
            collision_assumed = False

    plt.show()


if __name__ == "__main__":
    main()
