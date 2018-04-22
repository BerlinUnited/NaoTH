import math
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field

"""
    Test for repeated collisions. Angle calculation does not work correctly
"""


def main():
    plt.clf()
    tools.draw_field(plt.gca())
    axes = plt.gca()

    # Create a list of line segments
    linesegment_list = [
                        m2d.LineSegment(m2d.Vector2(-field.x_length / 2.0, field.y_length / 2.0),
                                        m2d.Vector2(field.x_length / 2.0, field.y_length / 2.0)),
                        m2d.LineSegment(m2d.Vector2(-field.x_length / 2.0, -field.y_length / 2.0),
                                        m2d.Vector2(field.x_length / 2.0, -field.y_length / 2.0)),

                        m2d.LineSegment(m2d.Vector2(field.x_length / 2.0, field.y_length / 2.0),
                                        m2d.Vector2(field.x_length / 2.0, -field.y_length / 2.0)),
                        m2d.LineSegment(m2d.Vector2(-field.x_length / 2.0, field.y_length / 2.0),
                                        m2d.Vector2(-field.x_length / 2.0, -field.y_length / 2.0))
    ]
    # Draw borders
    for line in linesegment_list:
        axes.plot([line.begin().x, line.end().x], [line.begin().y, line.end().y], 'black')

    ball_start = m2d.Vector2(0, 0)
    ball_end = m2d.Vector2(6000, 2000)

    # draw start position
    axes.add_artist(Circle(xy=(ball_start.x, ball_start.y), radius=100, fill=False, edgecolor='white'))

    collision_assumed = True
    while collision_assumed:
        shootline = m2d.LineSegment(ball_start, ball_end)

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
            intersection_point = shootline.point(t_min - field.ball_radius)

            axes.add_artist(Circle(xy=(intersection_point.x, intersection_point.y), radius=100, fill=False, edgecolor='pink'))
            axes.plot([ball_start.x, intersection_point.x], [ball_start.y, intersection_point.y], 'red')

            # calculate new endpoint
            new_shootline_length = math.hypot(ball_end.x - intersection_point.x,
                                              ball_end.y - intersection_point.y)

            shootline_angle = math.atan2(intersection_point.y - ball_start.y,
                                         intersection_point.x - ball_start.x)

            if shootline_angle > math.pi/2:
                print("Original Shootline Angle: ", math.degrees(shootline_angle), "New:", math.degrees(shootline_angle - math.pi/2))
                shootline_angle = shootline_angle - math.pi/2

                print("Einfallswinkel: ", math.degrees(shootline_angle))

                bounce = intersection_point + (m2d.Vector2(new_shootline_length, 0)).rotate(math.pi - 2*shootline_angle)

                # rotate new vector by the angle of the original shootline
                bounce = bounce.rotate(-shootline_angle)
            else:
                print("Einfallswinkel: ", math.degrees(shootline_angle))

                bounce = intersection_point + (m2d.Vector2(new_shootline_length, 0)).rotate(math.pi - 2 * shootline_angle)

                # rotate new vector by the angle of the original shootline
                bounce = bounce.rotate(shootline_angle)

            # set ball_start and ball_end
            ball_start = intersection_point
            ball_end = bounce
            collision_flag = False
        else:
            axes.plot([ball_start.x, ball_end.x], [ball_start.y, ball_end.y], 'blue')
            collision_assumed = False

    plt.show()


if __name__ == "__main__":
    main()
