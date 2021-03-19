from __future__ import division
import os
import sys
import inspect
import pickle
import numpy as np
import matplotlib as mpl
from matplotlib import pyplot as plt
from matplotlib.patches import Circle

# TODO make a tools script in root folder which imports all the scripts in tools folder
cmd_subfolder = os.path.realpath(
    os.path.abspath(os.path.join(os.path.split(inspect.getfile(inspect.currentframe()))[0], "..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

from tools import tools
from tools.action import Category
from tools import field_info as field

from matplotlib.backends.backend_pgf import FigureCanvasPgf

mpl.backend_bases.register_backend('pgf', FigureCanvasPgf)

"""
pgf_params = {                          # setup matplotlib to use latex for output
    'pgf.texsystem': 'pdflatex',        # change this if using xetex or luatex
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
        r'\usepackage[utf8x]{inputenc}',    # use utf8 fonts because your computer can handle it :)
        r'\usepackage[T1]{fontenc}',        # plots will be generated using this preamble
        ]
    }
mpl.rcParams.update(pgf_params)

mpl.rcParams['xtick.direction'] = 'in'
mpl.rcParams['ytick.direction'] = 'in'
"""


def plot_start_positions(exp):
    # test for iterating over all frames
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    for i in range(len(exp['frames'])):
        start_x = exp['frames'][i]['sim']['optimal_one'][0].state.pose.translation.x
        start_y = exp['frames'][i]['sim']['optimal_one'][0].state.pose.translation.y
        axes.add_artist(Circle(xy=(start_x, start_y), radius=100, fill=False, edgecolor='white'))

    plt.show()


def extract_values(exp, strategy, get_value):
    # NOTE: we ignore the last element here
    # for extraction of 'rotation' and 'walk_dist' the last element should included.
    return [get_value(e) for frame in exp['frames'] for e in frame['sim'][strategy][1:-1] if
            getValue(e) != 0]  # if np.abs(get_value(e)) > np.radians(10)


def plot_histogram(exp):
    names = {'optimal_one': 'optimal one', 'optimal_all': 'optimal all', 'fast': 'fast',
             'optimal_value': 'optimal value', 'fast_best': 'fast best'}

    kick_values = []
    labels = []
    values = []

    for i, strategy in enumerate(exp['frames'][0]['sim']):
        print(strategy)
        values += [np.abs(np.degrees(extract_values(exp, strategy, lambda x: x.turn_around_ball)))]

        kick_values += [[len(frame['sim'][strategy]) - 1 for frame in exp['frames']]]
        labels += [names[strategy]]

        max_value = max(
            np.abs(np.degrees(extract_values(exp, strategy, lambda x: x.turn_around_ball))))

        print(max_value)

        # ax[i].hist(np.abs(np.degrees(values)), range=[0, 180], rwidth=1, bins=18)
        # ax[i].hist(values, range=[0, 15], bins=15, cumulative=True, histtype='step')
        # ax[i].set_title(strategy)

    #
    # plt.xkcd()
    f, ax = plt.subplots(2, 1)
    ax[0].hist(values, range=[0, 180], rwidth=1, bins=16, label=labels, align='mid')
    ax[0].legend()
    ax[0].set_yscale("log")
    ax[0].set_ylabel('Number of occurrences.')
    ax[0].set_xlabel('Turn around ball in deg.')

    ax[1].boxplot(np.transpose(np.array(kick_values)), labels=labels)
    ax[1].set_ylabel('Number of kicks until goal.')
    ax[1].grid()

    plt.tight_layout()
    plt.savefig('{}.pgf'.format("kick_box"))
    plt.show()


def plot_matrix(exp):
    all_rotations = []
    all_ball_turns = []
    all_walking = []

    num_frames = len(exp['frames'])

    for frame in range(num_frames):
        e = exp['frames'][frame]['sim']['optimal_one']
        trace_length = len(e)
        start_x = e[0].state.pose.translation.x
        start_y = e[0].state.pose.translation.y
        rotation = 0
        turn_around_ball = 0
        walk_distance = 0

        for i in range(trace_length):
            rotation += np.abs(e[i].rotate)
            turn_around_ball += np.abs(e[i].turn_around_ball)
            walk_distance += np.abs(e[i].walk_dist)

        all_rotations.append([start_x, start_y, rotation])
        all_ball_turns.append([start_x, start_y, turn_around_ball])
        all_walking.append([start_x, start_y, walk_distance])

    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    data = all_rotations

    nx = {}
    ny = {}
    for pos in data:
        x, y, value = pos
        nx[x] = x
        ny[y] = y

    nxi = np.array(sorted(nx.keys()))
    nyi = np.array(sorted(ny.keys()))

    for i, v in enumerate(nxi):
        nx[v] = i

    for i, v in enumerate(nyi):
        ny[v] = i

    f = np.zeros((len(ny), len(nx)))

    for pos in data:
        x, y, value = pos

        value = value

        if np.isnan(value):
            f[ny[y], nx[x]] = 0
        else:
            f[ny[y], nx[x]] = value

    x_step = y_step = 300

    x_range = range(int(-field.x_length / 2), int(field.x_length / 2) + x_step, x_step)
    y_range = range(int(-field.y_length / 2), int(field.y_length / 2) + y_step, y_step)

    axes.pcolor(x_range, y_range, f, cmap="jet", alpha=0.8)
    plt.show()


def show_run(run, ):
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    # h1 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in run])
    h1 = np.array([[h.position.x, h.position.y] for h in run])

    # plt.plot(h1[:, 0], h1[:, 1], '-or')
    for i in range(len(h1[:, 0])):
        plt.plot(h1[:i, 0], h1[:i, 1], '-or')
        plt.pause(0.1)
    plt.show()


def extract_invalid_runs(exp, strategy):
    return [frame['sim'][strategy] for frame in exp['frames'] if
            frame['sim'][strategy][-2].state_category != Category.OPPGOAL]


def test(exp):
    bad_cases = {}
    for i, strategy in enumerate(exp['frames'][0]['sim']):
        bad_cases[strategy] = extract_invalid_runs(exp, strategy)
        print("{0}: {1}%".format(strategy, len(bad_cases[strategy]) / len(exp['frames']) * 100))

    for strategy, cases in bad_cases.iteritems():
        if strategy == 'fast':
            for run in cases:

                if np.max(np.abs(np.degrees([e.turn_around_ball for e in run[0:-1]]))) >= 110:
                    print(np.degrees(run[0].state.pose.rotation))
                    print([(np.degrees(e.turn_around_ball), e.selected_action_idx) for e in
                           run[0:-1]])
                    show_run(run)


if __name__ == "__main__":
    data_prefix = os.path.realpath(os.path.abspath(os.path.join(cmd_subfolder, "../data")))
    data_prefix = "./data/"
    # file = data_prefix + "simulation_6.pickle"
    # print("read file: " + file)

    experiment = pickle.load(open(data_prefix + 'simulation_1.pickle', "rb"))
    '''
    experiment = pickle.load(open(data_prefix + "simulation_6.pickle", "rb"))
    experiment2 = pickle.load(open(data_prefix + "simulation_7.pickle", "rb"))
    experiment3 = pickle.load(open(data_prefix + "simulation_8.pickle", "rb"))
    experiment['frames'] += experiment2['frames']
    experiment['frames'] += experiment3['frames']
    '''
    print
    len(experiment['frames'])

    # plot_start_positions(experiment)

    # plot_matrix(experiment)

    # test(experiment)

    plot_histogram(experiment)
