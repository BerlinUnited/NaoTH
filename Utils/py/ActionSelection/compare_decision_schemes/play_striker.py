from __future__ import division
import os
import sys
import inspect
import math
import copy
import numpy as np
from matplotlib import pyplot as plt
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir
from state import State

from run_simulation_with_particleFilter import calculate_best_direction as heinrich_test

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile(inspect.currentframe()))[0], "..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)


def direct_kick_strategy(state, action_list):
  
    turn_speed = math.radians(5.0)
    action_dir = 0
    turn_direction = 0

    while True:
        # turn towards the direction of the action
        state.pose.rotate(action_dir)

        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, num_particles=30))

        # Decide best action
        selected_action_idx = Sim.decide_smart(actions_consequences, state)

        # restore the previous orientation
        state.pose.rotate(-action_dir)

        if selected_action_idx != 0:
            break
        else:
            attack_direction = attack_dir.get_attack_direction(state)

            if turn_direction == 0:
                turn_direction = np.sign(attack_direction)  # "> 0" => left, "< 0" => right

            # set motion request
            action_dir += turn_direction*turn_speed

    return selected_action_idx, action_dir


def optimal_kick_strategy(state, action_list):
  
    action_dir = None
    selected_action_idx = 0
    for ix, action in enumerate(action_list):
        if action.name is "none":
            continue

        rotation, _ = heinrich_test(state, action_list[ix], False, iterations=20)

        if action_dir is None or np.abs(rotation) < np.abs(action_dir):
            action_dir = rotation
            selected_action_idx = ix

    return selected_action_idx, action_dir
  

class Simulator:
    def __init__(self, state, strategy, action_list):
        self.state = state
        self.strategy = strategy
        self.action_list = action_list

        #
        self.goal_scored = None
        self.inside_field = None

        # actions
        self.selected_action_idx = None
        self.turn_around_ball = 0
        self.rotate = 0
        self.walk_dist = 0

    def step(self):

        # reset stuff
        self.turn_around_ball = 0
        self.rotate = 0
        self.walk_dist = 0

        # ---------------
        #  make a decision
        # ---------------

        self.selected_action_idx, self.turn_around_ball = self.strategy(self.state, self.action_list)
        selected_action = self.action_list[self.selected_action_idx]

        # ---------------
        #  execute the action
        # ---------------

        if selected_action.name == "none":
            print("WARNING: action is NONE, what should we do here? " + str(self.selected_action_idx))
        else:
            # rotate around ball if necessary
            self.state.pose.rotate(self.turn_around_ball)

            # create a new action which will actually be executed
            new_action = a.Action("new_action", selected_action.speed, selected_action.speed_std, selected_action.angle, 0)

            single_consequence = a.ActionResults([])
            single_consequence = Sim.simulate_consequences(new_action, single_consequence, self.state, num_particles=30)

            # expected_ball_pos should be in local coordinates for rotation calculations
            new_ball_position = single_consequence.expected_ball_pos_mean

            # walk to ball now
            # set motion request
            self.rotate = new_ball_position.angle()
            self.walk_dist = new_ball_position.abs()

            # self.state.update_pos(self.state.pose * new_ball_position, math.degrees(self.state.pose.rotation + self.rotate))
            self.state.pose.rotate(self.rotate)
            self.state.pose.translate(self.walk_dist, 0)

        # ---------------
        #  evaluate the real situation
        # ---------------

        # Check if expected_ball_pos inside opponent goal
        opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
        opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

        ball_pos_global = self.state.pose * self.state.ball_position

        # update the internal state
        self.goal_scored = opp_goal_box.inside(ball_pos_global)
        self.inside_field = field.field_rect.inside(ball_pos_global)
    
    
def run(state, strategy, action_list):
  
    simulator = Simulator(state, strategy, action_list)

    history = [[state.pose.translation.x, state.pose.translation.y]]

    while True:

        simulator.step()

        history += [[simulator.state.pose.translation.x, simulator.state.pose.translation.y]]

        if simulator.goal_scored:
            # print ("success")
            break
        elif not simulator.inside_field:
            # print ("failure")
            break
        else:
            # continue
            pass

    return history

  
def select(actions, ids):
    return [actions[x] for x in ids]


if __name__ == "__main__":
    
    # set up the available actions
    actions = {
      "none": a.Action("none", 0, 0, 0, 0),
      "kick_short": a.Action("kick_short", 1080, 150, 0, 7),
      "sidekick_left": a.Action("sidekick_left", 750, 150, 90, 10),
      "sidekick_right": a.Action("sidekick_right", 750, 50, -90, 10)
    }

    all_actions = select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])
    # print ([a.name for a in all_actions])
    
    # repeat it multiple times to see how reliable the result is
    for i in range(10):
    
        origin = State()
        origin.pose.rotation = np.radians(70)
        origin.pose.translation.x = 3000
        origin.pose.translation.y = 2500
        
        state = copy.deepcopy(origin)
        print("run optimal_kick_strategy")
        history1 = run(state, optimal_kick_strategy, select(actions, ["none", "kick_short"]))
        
        state = copy.deepcopy(origin)
        print("run optimal_kick_strategy")
        history2 = run(state, optimal_kick_strategy, all_actions)
        
        state = copy.deepcopy(origin)
        print("run direct_kick_strategy")
        history3 = run(state, direct_kick_strategy, all_actions)
        
        plt.clf()
        axes = plt.gca()
        tools.draw_field(axes)
        
        h1 = np.array(history1)
        h2 = np.array(history2)
        h3 = np.array(history3)
        
        plt.plot(h1[:, 0], h1[:, 1], '-ob')
        plt.plot(h2[:, 0], h2[:, 1], '-ok')
        plt.plot(h3[:, 0], h3[:, 1], '-or')
        
        plt.pause(1)
