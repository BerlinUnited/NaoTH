from __future__ import division
import os
import sys
import inspect
import math
import copy
import numpy as np
from matplotlib import pyplot as plt
from naoth import math2d as m2d

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile(inspect.currentframe()))[0], "..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

from tools import action as a
from tools import Simulation as Sim
from tools import tools
from tools import raw_attack_direction_provider as attack_dir
from state import State
from run_simulation_with_particleFilter import calculate_best_direction as heinrich_test


def minimal_rotation(state, action, turn_direction):
  
    turn_speed = math.radians(5.0)
    action_dir = 0

    none = a.Action("none", 0, 0, 0, 0)
    none_actions_consequences = Sim.simulateAction(none, state, num_particles=30)

    while True:
        # turn towards the direction of the action
        state.pose.rotate(action_dir)

        # Simulate Consequences
        actions_consequences = Sim.simulateAction(action, state, num_particles=30)

        # Decide best action
        selected_action_idx = Sim.decide_minimal([none_actions_consequences, actions_consequences], state)

        # restore the previous orientation
        state.pose.rotate(-action_dir)

        if selected_action_idx != 0:
            break
        elif np.abs(action_dir) > math.pi:
            # print("WARNING: in minimal_rotation no kick found after rotation {0}.".format(action_dir))
            break
        else:
            # decide on rotation direction once
            if turn_direction == 0:
                attack_direction = attack_dir.get_attack_direction(state)
                turn_direction = np.sign(attack_direction)  # "> 0" => left, "< 0" => right

            # set motion request
            action_dir += turn_direction*turn_speed

    return action_dir


def direct_kick_strategy_cool_best(state, action_list):
    return direct_kick_strategy_cool(state, action_list, True)


def direct_kick_strategy_cool(state, action_list, take_best=False):
  
    actions_consequences = []
    rotations = []
    
    fastest_action_dir = None
    fastest_action_idx = 0
    
    for idx, action in enumerate(action_list):
        
        if action.name is "none":
            rotation = 0
        else:
            # optimize action
            a0 = minimal_rotation(state, action, 1)
            a1 = minimal_rotation(state, action, -1)
            if np.abs(a0) < np.abs(a1):
                rotation = a0
            else:
                rotation = a1

            if np.abs(rotation) > 3:
                print("WARNING: in direct_kick_strategy_cool no kick found after rotation {0}.".format(rotation))
        
        rotations += [rotation]
        
        # apply optimized rotation
        state.pose.rotate(rotation)
        
        actions_consequences.append(Sim.simulateAction(action, state, num_particles=30))
        
        # restore previous rotation
        state.pose.rotate(-rotation)
        
        if action.name is not "none":
            if fastest_action_dir is None or np.abs(rotation) < np.abs(fastest_action_dir):
                fastest_action_dir = rotation
                fastest_action_idx = idx
        
    # Decide best action
    if take_best:
        selected_action_idx = Sim.decide_minimal(actions_consequences, state)
        return selected_action_idx, rotations[selected_action_idx]
    else:
        # print fastest_action_idx, selected_action_idx
        return fastest_action_idx, fastest_action_dir


def direct_kick_strategy(state, action_list):
  
    turn_speed = math.radians(5.0)
    action_dir = 0
    turn_direction = 0

    while True:
        # turn towards the direction of the action
        state.pose.rotate(action_dir)

        # Simulate Consequences
        actions_consequences = [Sim.simulateAction(action, state, num_particles=30) for action in action_list]

        # Decide best action
        selected_action_idx = Sim.decide_minimal(actions_consequences, state)
        
        # restore the previous orientation
        state.pose.rotate(-action_dir)

        if selected_action_idx != 0:
            break
        elif np.abs(action_dir) > math.pi:
            print ("WARNING: in direct_kick_strategy no kick found after rotation {0}.".format(action_dir))
            break
        else:
            # decide on rotation direction once
            if turn_direction == 0:
                attack_direction = attack_dir.get_attack_direction(state)
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

        rotation, _ = heinrich_test(state, action, False, iterations=20)

        if action_dir is None or np.abs(rotation) < np.abs(action_dir):
            action_dir = rotation
            selected_action_idx = ix

    return selected_action_idx, action_dir

    
def optimal_value_strategy(state, action_list):
  
    actions_consequences = []
    rotations = []
    for action in action_list:
        
        if action.name is "none":
            rotation = 0
        else:
            # optimize action
            rotation, _ = heinrich_test(state, action, False, iterations=20)
        
        rotations += [rotation]
        
        # apply optimized rotation
        state.pose.rotate(rotation)
        
        actions_consequences.append(Sim.simulateAction(action, state, num_particles=30))
        
        # restore previous rotation
        state.pose.rotate(-rotation)

    # Decide best action
    selected_action_idx = Sim.decide_minimal(actions_consequences, state)

    return selected_action_idx, rotations[selected_action_idx]    

    
class Simulator:
    def __init__(self, state, strategy, action_list):
        self.state = state
        self.strategy = strategy
        self.action_list = action_list

        #
        self.state_category = None

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
        # approach ball
        # ---------------
        self.rotate = self.state.ball_position.angle()
        self.walk_dist = self.state.ball_position.abs()

        # HACK: execute motion request
        self.state.pose.rotate(self.rotate)
        self.state.pose.translate(self.walk_dist, 0)
        self.state.ball_position = m2d.Vector2(100.0, 0.0)
        
        # ---------------
        #  make a decision
        # ---------------

        self.selected_action_idx, self.turn_around_ball = self.strategy(self.state, self.action_list)
        selected_action = self.action_list[self.selected_action_idx]

        # ---------------
        #  execute the action
        # ---------------
        if selected_action.name == "none":
            # print("INFO: NONE action while ball outside of the field.")
            if self.state_category == a.Category.INFIELD:
              print("WARNING: action is NONE, what should we do here? " + str(self.selected_action_idx))
              print("STATE: robot = {0}".format(self.state.pose))
        else:
            # rotate around ball if necessary
            self.state.pose.rotate(self.turn_around_ball)

            # hack: perfect world without noise
            perfect_world = False
            if perfect_world:
                real_action = a.Action("real_action", selected_action.speed, 0, selected_action.angle, 0)
            else:
                real_action = selected_action
              
            # expected_ball_pos should be in local coordinates for rotation calculations
            action_results = Sim.simulateAction(real_action, self.state, num_particles=1)
            
            # store the state and update the state
            new_ball_position = action_results.positions()[0]
            self.state_category = new_ball_position.cat()
            self.state.ball_position = new_ball_position.pos()
    
    
def run_experiment(origin, strategy, action_list, max_iterations=30):
  
    state = copy.deepcopy(origin)
    simulator = Simulator(state, strategy, action_list)

    # record the state of the simulator
    history = [copy.deepcopy(simulator)]

    while len(history) < max_iterations:

        simulator.step()

        history += [copy.deepcopy(simulator)]

        if simulator.state_category == a.Category.OPPGOAL:
            # print ("success")
            break
        elif simulator.state_category != a.Category.INFIELD:
            # print ("failure")
            break

    # hack: make the last run
    simulator.step()
    history += [copy.deepcopy(simulator)]
            
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
    for i in range(100):
    
        origin = State()
        origin.pose.rotation = np.radians([66.0])
        origin.pose.translation.x = 1086
        origin.pose.translation.y = 2047
        
        '''
        state = copy.deepcopy(origin)
        print("run optimal_kick_strategy")
        history1 = run_experiment(state, optimal_kick_strategy, select(actions, ["none", "kick_short"]))
        
        state = copy.deepcopy(origin)
        print("run optimal_kick_strategy")
        history2 = run_experiment(state, optimal_kick_strategy, all_actions)
        
        state = copy.deepcopy(origin)
        print("run direct_kick_strategy")
        history3 = run_experiment(state, direct_kick_strategy, all_actions)
        
        state = copy.deepcopy(origin)
        print("run optimal_value_strategy")
        history4 = run_experiment(state, optimal_value_strategy, all_actions)
        
        h1 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in history1])
        h2 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in history2])
        h3 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in history3])
        h4 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in history4])
        
        plt.clf()
        axes = plt.gca()
        tools.draw_field(axes)
        
        
        plt.plot(h1[:, 0], h1[:, 1], '-ob')
        plt.plot(h2[:, 0], h2[:, 1], '-ok')
        plt.plot(h3[:, 0], h3[:, 1], '-or')
        plt.plot(h4[:, 0], h4[:, 1], '-oy')
        
        plt.pause(2)
        '''
        
        state = copy.deepcopy(origin)
        print("run direct_kick_strategy")
        history3 = run_experiment(state, direct_kick_strategy_cool, all_actions)
        
        '''
        mm = np.max(np.abs([np.degrees(h.turn_around_ball) for h in history3[0:-1]]))
        if mm < 140:
          print mm
          continue
        '''
        
        print([np.degrees(h.turn_around_ball) for h in history3[0:-1]])
        print ([h.selected_action_idx for h in history3[0:-1]])
        
        h3 = np.array([[h.state.pose.translation.x, h.state.pose.translation.y] for h in history3])
        v3 = np.array([[np.cos(h.state.pose.rotation), np.sin(h.state.pose.rotation)] for h in history3])*200
        
        plt.clf()
        axes = plt.gca()
        tools.draw_field(axes)
        
        plt.plot(h3[:, 0], h3[:, 1], '-or')
        plt.quiver(h3[:, 0], h3[:, 1], v3[:, 0], v3[:, 1], units='width')

        plt.show()
