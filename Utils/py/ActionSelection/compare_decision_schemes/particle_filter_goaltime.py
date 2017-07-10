import math
import copy
import numpy as np
from tools import action as a
from tools import Simulation as Sim
from tools import potential_field as pf
from naoth import math2d as m2d
from tools import tools
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import field_info as field
"""
    This file calculates the shooting direction for given position on the field, then turns the robot accordingly
    and executes a short kick and then continues until a goal is scored - the required time is returned
        
    The shooting direction is calculated according to a particle filter
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(0, 0)
        self.pose.rotation = math.radians(0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)


def draw_actions(actions_consequences, state, best_action, normal_angle, better_angle, angle_list):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    axes.add_artist(
        Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.text(0, 0, best_action, fontsize=12)

    origin = state.pose.translation
    # arrow_head = m2d.Vector2(500, 0).rotate(math.radians(normal_angle + better_angle))

    for angle in angle_list:
        arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + angle))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

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
        return l/s


def resample(samples, likelihoods, n, sigma):
    new_samples = np.zeros(n)
    likelihoods = normalize(likelihoods)
    likelihoods = normalize(np.power(likelihoods, 2))

    likelihood_step = 1.0 / float(n)
    target_sum = np.random.random(1) * likelihood_step
    current_sum = 0.0

    j = 0
    for (s, v) in zip(samples, likelihoods):
        current_sum += v

        while target_sum < current_sum and j < len(new_samples):
            new_samples[j] = s + (np.random.random(1) - 0.5) * 2 * sigma
            target_sum += likelihood_step
            j += 1

    return new_samples


def update(samples, likelihoods, state, action, m_min, m_max):
    # likelihoods = np.zeros(samples.shape)
    test_action = copy.deepcopy(action)
    simulation_consequences = []

    simulation_num_particles = 1
    for i in range(0, len(samples)):
        # modify the action with the sample
        test_action.angle = action.angle + samples[i]

        test_action_consequence = a.ActionResults([])
        simulation_consequences.append(
            Sim.simulate_consequences(test_action, test_action_consequence, state, simulation_num_particles))

        if test_action_consequence.category("INFIELD") > 0:
            potential = -pf.evaluate_action2(test_action_consequence, state)
            likelihoods[i] = potential
            m_min = min(m_min, potential)
            m_max = max(m_max, potential)
        elif test_action_consequence.category("OPPGOAL") > 0:
            likelihoods[i] = m_max
        else:
            likelihoods[i] = m_min

    return likelihoods, simulation_consequences, m_min, m_max


def calculate_best_direction(x, y, iterations, show, state, action):
    # state.update_pos(m2d.Vector2(x, y), rotation=0)

    # particles
    num_angle_particle = 30
    n_random = 0

    samples = (np.random.random(num_angle_particle) - 0.5) * 2 * 180.0
    likelihoods = np.ones(samples.shape) * (1 / float(num_angle_particle))

    m_min = 0
    m_max = 0
    mean_angle = None
    for iteration in range(1, iterations):

        # evaluate the particles
        likelihoods, simulation_consequences, m_min, m_max = update(samples, likelihoods, state, action, m_min, m_max)

        # resample
        samples = resample(samples, likelihoods, num_angle_particle, 5.0)

        # add random samples
        if n_random > 0:
            samples[(num_angle_particle - n_random):num_angle_particle] = np.random.random(n_random) * 360.0

        mean_angle = np.mean(samples)

        if show:
            draw_actions(simulation_consequences, state, action.name, action.angle, mean_angle, samples)

    return np.radians(mean_angle), np.radians(np.std(samples))


def main(x, y, rot, state):

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 980, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    state.update_pos(m2d.Vector2(x, y), rotation=rot)  # rot is in degrees

    # TODO do it multiple times
    num_kicks = 0
    num_turn_degrees = 0
    goal_scored = False
    total_time = 0
    while not goal_scored:
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, a.num_particles))

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        # Change Angle of best action according to the particle filter
        # best_dir is the global rotation for that kick
        best_dir, _ = calculate_best_direction(state.pose.translation.x, state.pose.translation.y, 50, False, state, action_list[best_action])

        # Rotate the robot so that the shooting angle == best_dir
        state.pose.rotation = state.pose.rotation + best_dir

        total_time += np.abs(math.degrees(best_dir) / state.rotation_vel)

        #
        single_consequence = a.ActionResults([])
        actions_consequences.append(Sim.simulate_consequences(action_list[best_action], single_consequence, state, a.num_particles))

        # expected_ball_pos should be in local coordinates for rotation calculations
        expected_ball_pos = actions_consequences[best_action].expected_ball_pos

        # Check if expected_ball_pos inside opponent goal
        opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth,
                                          field.opponent_goalpost_right.y)
        opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

        goal_scored = opp_goal_box.inside(state.pose * expected_ball_pos)
        inside_field = field.field_rect.inside(state.pose * expected_ball_pos)

        # Assert that expected_ball_pos is inside field or inside opp goal
        if not inside_field and not goal_scored:
            # print("Error: This position doesn't manage a goal")
            total_time = float('nan')
            break

        # calculate the time needed
        rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
        rotation_time = np.abs(math.degrees(rotation) / state.rotation_vel)
        distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
        distance_time = distance / state.walking_vel

        total_time += distance_time + rotation_time

        # update the robots position
        state.update_pos(state.pose * expected_ball_pos, math.degrees(state.pose.rotation + rotation))
        num_kicks += 1

    return total_time

if __name__ == "__main__":
    state = State()
    rotation_step = 10
    total_time = main(state.pose.translation.x, state.pose.translation.y, math.degrees(state.pose.rotation), state)
