import math
import sys
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
    The best direction for each kick is calculated via a particle filter and a potential field. The kick with the maximum strategic value after the applying the best rotation
    is chosen
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


def draw_actions(actions_consequences, s, best_action, normal_angle, better_angle, angle_list):
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    axes.add_artist(
        Circle(xy=(s.pose.translation.x, s.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.text(0, 0, best_action, fontsize=12)

    origin = s.pose.translation
    # arrow_head = m2d.Vector2(500, 0).rotate(math.radians(normal_angle + better_angle))

    for angle in angle_list:
        arrow_head = m2d.Vector2(500, 0).rotate(s.pose.rotation + math.radians(normal_angle + angle))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

    # arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + better_angle))
    # axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

    x = np.array([])
    y = np.array([])

    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = s.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.000001)


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


def update(samples, likelihoods, s, action, m_min, m_max):
    # likelihoods = np.zeros(samples.shape)
    test_action = copy.deepcopy(action)
    simulation_consequences = []

    simulation_num_particles = 1
    for idx in range(0, len(samples)):
        # modify the action with the sample
        test_action.angle = action.angle + samples[idx]

        test_action_consequence = a.ActionResults([])
        simulation_consequences.append(
            Sim.simulate_consequences(test_action, test_action_consequence, s, simulation_num_particles))

        if test_action_consequence.category("INFIELD") > 0:
            potential = -pf.evaluate_action2(test_action_consequence, s)
            likelihoods[idx] = potential
            m_min = min(m_min, potential)
            m_max = max(m_max, potential)
        elif test_action_consequence.category("OPPGOAL") > 0:
            likelihoods[idx] = m_max
        else:
            likelihoods[idx] = m_min

    return likelihoods, simulation_consequences, m_min, m_max


def calculate_best_direction(s, action, show, iterations):
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
        likelihoods, simulation_consequences, m_min, m_max = update(samples, likelihoods, s, action, m_min, m_max)

        # resample
        samples = resample(samples, likelihoods, num_angle_particle, 5.0)

        # add random samples
        if n_random > 0:
            samples[(num_angle_particle - n_random):num_angle_particle] = np.random.random(n_random) * 360.0

        mean_angle = np.mean(samples)

        if show:
            draw_actions(simulation_consequences, s, action.name, action.angle, mean_angle, samples)

    return np.radians(mean_angle), np.radians(np.std(samples)), simulation_consequences


def main(x, y, rot, s, num_iter):
    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1280, 0, 0, 0)
    sidekick_left = a.Action("sidekick_left", 750, 0, 90, 0)
    sidekick_right = a.Action("sidekick_right", 750, 0, -90, 0)
    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    s.update_pos(m2d.Vector2(x, y), rotation=rot)  # rot is in degrees

    # Do several decision cycles not just one to get rid of accidental decisions
    timings = []
    n_kicks = []
    n_turns = []
    for idx in range(num_iter):
        num_kicks = 0
        num_turn_degrees = 0
        goal_scored = False
        total_time = 0
        s.update_pos(m2d.Vector2(x, y), rotation=rot)
        while not goal_scored:
            # Test:
            print(state.pose.translation.x, state.pose.translation.y, math.degrees(state.pose.rotation) % 360)


            # Change Angle of all actions according to the particle filter
            # best_dir is the global rotation for that kick
            best_action = 0
            actions_consequences = []
            best_dirs = [0]
            old_rotation = s.pose.rotation
            for ix, action in enumerate(action_list):
                if action.name is "none":
                    continue

                tmp, _, consequences = calculate_best_direction(s, action_list[ix], False, iterations=20)

                # TODO simulate again normally for each rotation
                s.pose.rotation = old_rotation + tmp  # TODO change it back
                best_dirs.append(tmp)

                single_consequence = a.ActionResults([])
                actions_consequences.append(Sim.simulate_consequences(action, single_consequence, s, a.num_particles))

                print("Best dir: " + str(math.degrees(tmp)) + " for action: " + action_list[ix].name)

            best_action = Sim.decide_smart(actions_consequences, s)
            # Rotate the robot so that the shooting angle == best_dir
            best_dir = best_dirs[best_action]
            print("Total Best dir: " + str(math.degrees(best_dir)) + " for action: " + action_list[best_action].name)
            s.pose.rotation = old_rotation + best_dir
            print(state.pose.translation.x, state.pose.translation.y, math.degrees(state.pose.rotation) % 360)
            # sys.exit()
            # only model turning when it's significant
            if np.abs(best_dir) > 5:
                total_time += np.abs(math.degrees(best_dir) / s.rotation_vel)
                num_turn_degrees += np.abs(math.degrees(best_dir))

            # after turning evaluate the best action again to calculate the expected ball position
            single_consequence = a.ActionResults([])
            actions_consequences.append(
                Sim.simulate_consequences(action_list[best_action], single_consequence, s, a.num_particles))

            # expected_ball_pos should be in local coordinates for rotation calculations
            expected_ball_pos = actions_consequences[best_action].expected_ball_pos

            # Check if expected_ball_pos inside opponent goal
            opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth,
                                              field.opponent_goalpost_right.y)
            opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

            goal_scored = opp_goal_box.inside(s.pose * expected_ball_pos)
            inside_field = field.field_rect.inside(s.pose * expected_ball_pos)

            # Assert that expected_ball_pos is inside field or inside opp goal
            if not inside_field and not goal_scored:
                # print("Error: This position doesn't manage a goal")
                # total_time = float('nan')
                # Maybe still treat it as goal since it's some weird issue with particle not inside the goal which screws up the mean
                break

            # calculate the time needed
            rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
            rotation_time = np.abs(math.degrees(rotation) / s.rotation_vel)
            distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
            distance_time = distance / s.walking_vel

            total_time += distance_time + rotation_time

            # update the robots position
            s.update_pos(s.pose * expected_ball_pos, math.degrees(s.pose.rotation + rotation))
            num_turn_degrees += np.abs(math.degrees(rotation))
            num_kicks += 1

        timings.append(total_time)
        n_kicks.append(num_kicks)
        n_turns.append(num_turn_degrees)

    return np.nanmin(timings), np.nanmean(n_kicks), np.nanmean(n_turns)


if __name__ == "__main__":
    for i in range(1):
        state = State()
        rotation_step = 10
        time, kicks, turns = main(state.pose.translation.x, state.pose.translation.y, math.degrees(state.pose.rotation), state, num_iter=10)

        print("Total time to goal: " + str(time) + " #Kicks: " + str(kicks) + " #Turns: " + str(turns))
