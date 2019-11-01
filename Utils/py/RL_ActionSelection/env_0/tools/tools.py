from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib.patches as patches
import field_info as f
import matplotlib as mpl


def draw_field(axis=plt.gca()):
    mpl.rcParams['lines.linewidth'] = 2.33513514  # was 2 before

    ax = axis

    # draw field background
    ax.add_patch(
        patches.Rectangle(
            (-f.x_field_length / 2, -f.y_field_length / 2), f.x_field_length, f.y_field_length,
            facecolor="green", zorder=0
        )
    )

    ax.plot([0, 0], [-f.y_length * 0.5, f.y_length * 0.5], 'white')  # Middle line

    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline],
            [f.y_left_sideline, f.y_right_sideline], 'white')  # opponent ground line
    ax.plot([f.x_own_groundline, f.x_own_groundline], [f.y_right_sideline, f.y_left_sideline],
            'white')  # own ground line

    ax.plot([f.x_own_groundline, f.x_opponent_groundline], [f.y_left_sideline, f.y_left_sideline],
            'white')
    ax.plot([f.x_own_groundline, f.x_opponent_groundline],
            [f.y_right_sideline, f.y_right_sideline], 'white')

    ax.plot([f.x_opponent_groundline - f.x_penalty_area_length,
             f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'white')  # opp penalty

    ax.plot([f.x_own_groundline + f.x_penalty_area_length,
             f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'white')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=f.center_circle_radius, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(f.x_opponent_groundline - f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='white'))
    ax.add_artist(Circle(xy=(f.x_own_groundline + f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='white'))

    # Own goal box
    ax.add_artist(
        Circle(xy=(f.own_goalpost_right.x, f.own_goalpost_right.y), radius=f.goalpost_radius,
               color='white'))  # GoalPostRight
    ax.add_artist(
        Circle(xy=(f.own_goalpost_left.x, f.own_goalpost_left.y), radius=f.goalpost_radius,
               color='white'))  # GoalPostLeft
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline - f.goal_depth, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # own goal box

    # Opp GoalBox
    ax.add_artist(Circle(xy=(f.opponent_goalpost_right.x, f.opponent_goalpost_right.y),
                         radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.opponent_goalpost_left.x, f.opponent_goalpost_left.y),
                         radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline + f.goal_depth, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # Opp goal box

    ax.set_xlim([-f.x_field_length * 0.5, f.x_field_length * 0.5])
    ax.set_ylim([-f.y_field_length * 0.5, f.y_field_length * 0.5])
    # ax.set_facecolor('green')
    ax.set_aspect("equal")
