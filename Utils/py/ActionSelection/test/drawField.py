import field_info as field
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import numpy as np

ax = plt.gca()
ax.plot([0, 0], [-field.y_length*0.5, field.y_length*0.5], 'white')  # Middle line

ax.plot([field.x_opponent_groundline, field.x_opponent_groundline], [-3000, 3000], 'white')  # opponent groundline
ax.plot([-4500, -4500], [-3000, 3000], 'white')  # own groundline

ax.plot([field.x_own_groundline, field.x_opponent_groundline], [3000, 3000], 'white')
ax.plot([field.x_own_groundline, field.x_opponent_groundline], [-3000, -3000], 'white')

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
ax.add_artist(Circle(xy=(field.x_own_groundline+1300, 0), radius=50, color='white'))

# Own GoalBox -not quite correct - goalpost radius is not considered
ax.add_artist(Circle(xy=(field.own_goalpost_right.x, field.own_goalpost_right.y), radius=50, color='white'))  # GoalPostRight
ax.add_artist(Circle(xy=(field.own_goalpost_left.x, field.own_goalpost_left.y), radius=50, color='white'))  # GoalPostLeft
ax.plot([-4500, -5000], [-750, -750], 'white', linewidth=field.field_lines_width)  # own GoalBox
ax.plot([-4500, -5000], [750, 750], 'white')  # own GoalBox
ax.plot([-5000, -5000], [-750, 750], 'white')  # own GoalBox

# Opp GoalBox not quite correct - goalpost radius is not considered
ax.plot([4500, 5000], [-750, -750], 'white')  # Opp GoalBox
ax.plot([4500, 5000], [750, 750], 'white')  # Opp GoalBox
ax.plot([5000, 5000], [-750, 750], 'white')  # Opp GoalBox

ax.set_xlim([-field.x_field_length*0.5, field.x_field_length*0.5])
ax.set_ylim([-field.y_field_length*0.5, field.y_field_length*0.5])
ax.set_axis_bgcolor('green')
ax.set_aspect("equal")
# plt.show()


def linewidth_from_data_units(linewidth, axis, reference='y'):
    """
    Convert a linewidth in data units to linewidth in points.

    Parameters
    ----------
    linewidth: float
        Linewidth in data units of the respective reference-axis
    axis: matplotlib axis
        The axis which is used to extract the relevant transformation
        data (data limits and size must not change afterwards)
    reference: string
        The axis that is taken as a reference for the data width.
        Possible values: 'x' and 'y'. Defaults to 'y'.

    Returns
    -------
    linewidth: float
        Linewidth in points
    """
    fig = axis.get_figure()
    if reference == 'x':
        length = fig.bbox_inches.width * axis.get_position().width
        value_range = np.diff(axis.get_xlim())
    elif reference == 'y':
        length = fig.bbox_inches.height * axis.get_position().height
        value_range = np.diff(axis.get_ylim())
    # Convert length to points
    length *= 72
    # Scale linewidth to value range
    return linewidth * (length / value_range)

print linewidth_from_data_units(50, ax)
