from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math


def draw_field():  # Todo use field_info

    ax = plt.gca()
    ax.plot([0, 0], [-3000, 3000], 'white')  # Middle line
    ax.plot([4500, 4500], [-3000, 3000], 'white')
    ax.plot([-4500, -4500], [-3000, 3000], 'white')

    ax.plot([-4500, 4500], [3000, 3000], 'white')
    ax.plot([-4500, 4500], [-3000, -3000], 'white')

    ax.plot([3900, 3900], [-1100, 1100], 'white')  # opp penalty
    ax.plot([4500, 3900], [1100, 1100], 'white')  # opp penalty
    ax.plot([4500, 3900], [-1100, -1100], 'white')  # opp penalty

    ax.plot([-3900, -3900], [-1100, 1100], 'white')  # own penalty
    ax.plot([-4500, -3900], [1100, 1100], 'white')  # own penalty
    ax.plot([-4500, -3900], [-1100, -1100], 'white')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=750, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(4500-1300, 0), radius=50, color='white'))
    ax.add_artist(Circle(xy=(-4500+1300, 0), radius=50, color='white'))

    # OwnGoalBox -not quite correct - goalpost radius is not considered
    ax.plot([-4500, -5000], [-750, -750], 'white')  # own penalty
    ax.plot([-4500, -5000], [750, 750], 'white')  # own penalty
    ax.plot([-5000, -5000], [-750, 750], 'white')  # own penalty

    # OppGoalBox not quite correct - goalpost radius is not considered
    ax.plot([4500, 5000], [-750, -750], 'white')  # own penalty
    ax.plot([4500, 5000], [750, 750], 'white')  # own penalty
    ax.plot([5000, 5000], [-750, 750], 'white')  # own penalty

    ax.set_xlim([-5200, 5200])
    ax.set_ylim([-3700, 3700])
    ax.set_axis_bgcolor('green')
    ax.set_aspect("equal")
