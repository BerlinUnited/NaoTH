import math
import copy
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools.action import Category

from tools import Simulation as Sim
from tools import potential_field as pf
import naoth.math as naoth_magic
from tools import tools
from state import State
from tools import field_info as field

import timeit

'''
    Computes the best kick direction based on particle filter sampling over possible angles. 
    Uses manually crafted potential field for evaluation.
    
    TODO: fix problems with particle filter
'''


def draw_actions(actions_consequences, likelihoods, state, best_action, normal_angle, better_angle,
                 angle_list):
    plt.clf()
    tools.draw_field(plt.gca())

    axes = plt.gca()
    axes.add_artist(
        Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False,
               edgecolor='white'))
    axes.text(-4500, 3150, str(best_action) + " with rotation: " + str(better_angle), fontsize=12)

    origin = state.pose.translation
    likelihoods = normalize(likelihoods)

    for angle in angle_list:
        arrow_head = naoth_magic.Vector2(500, 0).rotate(
            state.pose.rotation + math.radians(normal_angle + angle))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100,
                   fc='k', ec='k')

    # arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + better_angle))
    # axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

    x = np.array([])
    y = np.array([])

    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.0001)


def normalize(likelihood):
    m = np.min(likelihood)
    # M = np.max(likelihood)
    l = likelihood - m
    s = np.sum(l)
    if s == 0:
        return likelihood
    else:
        return l / s


def resample(samples, likelihoods, n, sigma):
    likelihoods = normalize(likelihoods)
    # likelihoods = normalize(np.power(likelihoods, 2))

    new_samples = np.zeros(n)
    likelihood_step = 1.0 / float(n)
    target_sum = np.random.random(1) * likelihood_step
    current_sum = 0.0

    best = likelihoods[0]
    best_s = samples[0]

    j = 0
    for (s, v) in zip(samples, likelihoods):
        current_sum += v

        if v > best:
            best = v
            best_s = s

        while target_sum < current_sum and j < len(new_samples):
            new_samples[j] = s + (np.random.random(1) - 0.5) * 2 * sigma
            target_sum += likelihood_step
            j += 1

    new_samples[0] = best_s + (np.random.random(1) - 0.5) * 2 * sigma
    # for j in range(0,new_samples.shape[0]/2):
    #  new_samples[j] = best_s + (np.random.random(1) - 0.5)*2*sigma

    return new_samples


def update(samples, likelihoods, state, action, m_min, m_max):
    # likelihoods = np.zeros(samples.shape)
    test_action = copy.deepcopy(action)
    simulation_consequences = []

    simulation_num_particles = 1

    stats = np.zeros((len(samples), 3))
    number = 0.0

    for i in range(0, len(samples)):
        # modify the action with the sample
        test_action.angle = action.angle + samples[i]

        results = a.ActionResults([])
        simulation_consequences.append(
            Sim.simulate_consequences(test_action, results, state, simulation_num_particles))

        stats[i, 0] = results.likelihood(Category.OPPGOAL)
        stats[i, 1] = results.likelihood(Category.INFIELD)
        number = results.likelihood(Category.NUMBER)

        if stats[i, 0] + stats[i, 1] > 0:
            potential = -pf.evaluate_action(results, state)
            stats[i, 2] = potential

    max_goal = np.max(stats[:, 0])
    min_value = np.min(stats[:, 2])
    max_value = np.max(stats[:, 2])
    diff = max_value - min_value
    if diff == 0:
        diff = 1.0

    for i in range(0, len(samples)):
        if stats[i, 1] + stats[i, 0] >= max(0.0, Sim.good_threshold_percentage):
            likelihoods[i] = (stats[i, 1] + stats[i, 0]) * ((stats[i, 2] - min_value) / diff) + \
                             stats[i, 0]
        else:
            likelihoods[i] = 0

    return likelihoods, simulation_consequences, m_min, m_max


def calculate_best_direction(state, action, show, iterations):
    # particles
    # using at least 60 particles seems to always generate the correct kick direction
    num_angle_particle = 30
    n_random = 0

    # samples = (np.random.random(num_angle_particle)-0.5)*2 * 180.0
    # test uniform distribution for samples(angles)
    samples = np.arange(-180, 180, int(360 / num_angle_particle))
    likelihoods = np.ones(samples.shape) * (1 / float(num_angle_particle))

    ####################################################################################################################
    m_min = 0
    m_max = 0
    mean_angle = None
    noise = 30.0
    min_noise = 3.0
    for iteration in range(0, iterations):

        # evaluate the particles

        likelihoods, simulation_consequences, m_min, m_max = update(samples, likelihoods, state,
                                                                    action, m_min, m_max)
        # resample
        samples = resample(samples, likelihoods, num_angle_particle, noise)
        noise = max(min_noise, noise * 0.5)

        # add random samples 
        if n_random > 0:
            samples[(num_angle_particle - n_random):num_angle_particle] = np.random.random(
                n_random) * 360.0

        x = np.sum(np.sin(np.radians(samples)))
        y = np.sum(np.cos(np.radians(samples)))
        mean_angle = np.arctan2(x, y)
        # mean_angle = np.mean(samples)

        if show:
            if plt.get_fignums():
                draw_actions(simulation_consequences, likelihoods, state, action.name,
                             action.angle, mean_angle, samples)
            else:
                break

    return mean_angle, np.radians(np.std(samples))


if __name__ == "__main__":
    single_run = True

    if single_run:
        # run a single direction
        start_time = timeit.default_timer()

        state = State(-4000, 1000)
        action = a.Action("KickForward", 1000, 150, 0, 8)

        calculate_best_direction(state, action, True, 10)
        print(timeit.default_timer() - start_time)
    else:
        # run for the whole field
        # add 100 to make sure field.x_field_length/2 and field.y_field_length/2 are included
        step = 250
        x_pos = range(-field.x_field_length / 2, field.x_field_length / 2 + 100, step)
        y_pos = range(-field.y_field_length / 2, field.y_field_length / 2 + 100, step)

        xx, yy = np.meshgrid(x_pos, y_pos)
        vx = np.ones(xx.shape)
        vy = np.zeros(xx.shape)
        f = np.zeros(xx.shape)

        tools.draw_field(plt.gca())
        Q = plt.quiver(xx, yy, vx, vy, np.degrees(f), angles='uv', scale_units='xy', scale=1)
        plt.pause(0.001)

        # we simulate the same action for different positions
        action = a.Action("KickForward", 1000, 150, 0, 8)

        # print(xx.shape, len(x_pos), len(y_pos))
        start_time = timeit.default_timer()
        for ix in range(0, len(x_pos)):
            for iy in range(0, len(y_pos)):

                state = State(float(x_pos[ix]), float(y_pos[iy]))
                direction, direction_std = calculate_best_direction(state, action, False, 10)

                v = m2d.Vector2(step / 4.0 * 3.0, 0.0).rotate(direction)
                vx[iy, ix] = v.x
                vy[iy, ix] = v.y
                f[iy, ix] = direction_std

                if not plt.get_fignums():
                    quit()

                Q.set_UVC(vx, vy, np.degrees(f))
                plt.pause(0.001)

            print("progress: {0}% ({1}s)".format(float(ix + 1) / (float(len(x_pos))) * 100.0,
                                                 timeit.default_timer() - start_time))

        plt.show()
