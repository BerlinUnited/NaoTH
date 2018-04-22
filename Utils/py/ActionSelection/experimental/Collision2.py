from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from naoth import math2d as m2d
from tools import tools

"""
    Create a test for bouncing a ball from a wall. Calculate the angle based on a normal vector. Works if everything is in the
    coordinate frame of the collision point. Implementation based on http://www.3dkingdoms.com/weekly/weekly.php?a=2
"""


def main():
    plt.clf()
    tools.draw_field(plt.gca())
    axes = plt.gca()

    # Create a list of line segments
    test_line = m2d.LineSegment(m2d.Vector2(-2000, 0),
                                m2d.Vector2(2000, 0))

    axes.plot([test_line.begin().x, test_line.end().x], [test_line.begin().y, test_line.end().y], 'black')

    # calculate normal of border line
    dx = test_line.end().x - test_line.begin().x
    dy = test_line.end().y - test_line.begin().y

    axes.add_artist(Circle(xy=(-dy, dx), radius=100, fill=True, color='magenta'))
    axes.add_artist(Circle(xy=(dy, -dx), radius=100, fill=True, color='magenta'))

    # create normal line
    normal_line = m2d.LineSegment(m2d.Vector2(dy, dx), m2d.Vector2(dy, -dx))
    axes.plot([-dy, dy], [dx, -dx], 'magenta')

    # test project a vector onto normal line
    shooting_vector = m2d.Vector2(-1000, 1000)  # der vector muss in den coordinaten des collisionspunktes liegen
    projected_vector = normal_line.projection(shooting_vector)

    print("Projection onto normal:", projected_vector)

    reflection = 2*projected_vector - shooting_vector
    print("Reflection:", reflection)

    axes.add_artist(Circle(xy=(shooting_vector.x, shooting_vector.y), radius=100, fill=True, color='red'))
    axes.add_artist(Circle(xy=(reflection.x, reflection.y), radius=100, fill=True, color='blue'))

    plt.show()


if __name__ == "__main__":
    main()
