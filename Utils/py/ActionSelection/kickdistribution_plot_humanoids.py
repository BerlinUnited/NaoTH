import numpy as np
import matplotlib as mpl
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from tools import tools
from state import State
import math

"""
JUST A SOME THROAWAY SCRIPT for the paper

Example:
    run without any parameters

        $ python run_simulation.py
"""


from matplotlib.backends.backend_pgf import FigureCanvasPgf
mpl.backend_bases.register_backend('pgf', FigureCanvasPgf)

pgf_params = {                          # setup matplotlib to use latex for output
    'pgf.texsystem': 'pdflatex',        # change this if using xetex or lautex
    'text.usetex': True,                # use LaTeX to write all text
    'font.family': 'serif',
    'font.serif': [],                   # blank entries should cause plots to inherit fonts from the document
    'font.sans-serif': [],
    'font.monospace': [],
    # 'font.size': 10,
    # 'axes.labelsize': 10,               # LaTeX default is 10pt font.
    # 'legend.fontsize': 8,               # Make the legend/label fonts a little smaller
    # 'xtick.labelsize': 8,
    # 'ytick.labelsize': 8,
    # 'figure.figsize': figsize(0.8), #[6, 4],     # default fig size of 0.9 textwidth
    'figure.autolayout': True,
    'pgf.preamble': [
        r'\usepackage[utf8x]{inputenc}',    # use utf8 fonts becasue your computer can handle it :)
        r'\usepackage[T1]{fontenc}',        # plots will be generated using this preamble
        ]
    }
mpl.rcParams.update(pgf_params)

# mpl.rcParams['xtick.direction'] = 'in'
# mpl.rcParams['ytick.direction'] = 'in'

      
def draw_actions(actions_consequences, state, best_action):
    plt.clf()
    plt.axis('off')
    tools.draw_field(plt.gca())

    axes = plt.gca()
    axes.set_xlim([2300, 5800])
    axes.set_ylim([-3000, 3000])
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=70, fill=True, color='red'))
    # axes.text(-4500, 3150, best_action, fontsize=12)

    colors = ['cyan', 'yellow', 'red']

    for i, consequence in enumerate(actions_consequences):
        # expected_ball_pos_mean = state.pose * consequence.expected_ball_pos_mean
        # expected_ball_pos_median = state.pose * consequence.expected_ball_pos_median
        # axes.add_artist(Circle(xy=(expected_ball_pos_mean.x, expected_ball_pos_mean.y), radius=100, fill=False, edgecolor='yellow'))
        # axes.add_artist(Circle(xy=(expected_ball_pos_median.x, expected_ball_pos_median.y), radius=100, fill=False, edgecolor='blue'))
        x = np.array([])
        y = np.array([])
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

        plt.scatter(x, y, color=colors[i], edgecolor='black', alpha=1, zorder=100)
    plt.tight_layout()
    # plt.show()
    plt.savefig('{}.pgf'.format("kickdistribution_sampling"))


def main():
    state = State(4050, -750)
    state.pose.rotation = math.radians(70)
    # no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1080, 150, 0, 7)
    sidekick_left = a.Action("sidekick_left", 750, 90, 90, 10)
    sidekick_right = a.Action("sidekick_right", 750, 90, -90, 10)

    action_list = [kick_short, sidekick_left, sidekick_right]

    # while plt.get_fignums():
    actions_consequences = []
    # Simulate Consequences
    for action in action_list:
        single_consequence = a.ActionResults([])
        actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, 30))

    # actions_consequences is now a list of ActionResults

    # Decide best action
    best_action = Sim.decide_smart(actions_consequences, state)

    draw_actions(actions_consequences, state, action_list[best_action].name)


if __name__ == "__main__":
    main()
