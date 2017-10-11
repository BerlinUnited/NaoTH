from __future__ import division
import os, sys, inspect
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import sys
import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from tools import raw_attack_direction_provider as attack_dir
from state import State

from run_simulation_with_particleFilter import calculate_best_direction as heinrich_test


def direct_kick_strategy(state, action_list):
  action_dir = 0
  actions_consequences = []
  # Simulate Consequences
  for action in action_list:
      single_consequence = a.ActionResults([])
      actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, num_particles=30))

  # Decide best action
  selected_action_idx = Sim.decide_smart(actions_consequences, state)

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
  
  
def run(state, strategy, action_list):
  
  run_finished = False
  
  turn_direction = 0
  turn_speed = 1
  
  history = [[state.pose.translation.x, state.pose.translation.y]]
  
  while not run_finished:
    
    # ---------------
    #  make a decision
    # ---------------
    
    selected_action_idx, action_dir = strategy(state, action_list)
    selected_action = action_list[selected_action_idx]
    
    print ("action: " + selected_action.name)
    
    # ---------------
    #  execute the action
    # ---------------
    
    if selected_action.name == "none":
      
      attack_direction = attack_dir.get_attack_direction(state)

      if turn_direction == 0:
        turn_direction = np.sign(attack_direction) # "> 0" => left, "< 0" => right
      
      state.update_pos(state.pose.translation, math.degrees(state.pose.rotation) + turn_direction*turn_speed)

    else:
      # NOTE: reset turn direction
      turn_direction = 0

      # rotate around ball if necessary
      state.pose.rotation += action_dir
      
      # create a new action which will actually be executed
      new_action = a.Action("new_action", selected_action.speed, selected_action.speed_std, selected_action.angle, 0)
            
      actions_consequences = []
      single_consequence = a.ActionResults([])
      single_consequence = Sim.simulate_consequences(new_action, single_consequence, state, num_particles=30)

      # expected_ball_pos should be in local coordinates for rotation calculations
      new_ball_position = single_consequence.expected_ball_pos_mean

      
      # walk to ball now
      rotation_to_ball = new_ball_position.angle()
      #distance_to_ball = new_ball_position.abs()
      
      
      state.update_pos(state.pose * new_ball_position, math.degrees(state.pose.rotation + rotation_to_ball))

      history += [[state.pose.translation.x, state.pose.translation.y]]
      
      
    # ---------------
    #  evaluate the real situation
    # ---------------
    
    # Check if expected_ball_pos inside opponent goal
    opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
    opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

    ball_pos_global = state.pose * state.ball_position
    
    goal_scored = opp_goal_box.inside(ball_pos_global)
    inside_field = field.field_rect.inside(ball_pos_global)
    
    if goal_scored:
      #print ("success")
      break
    elif not inside_field:
      #print ("failure")
      break
    else:
      # continue
      pass
       
  return history

def select(actions, ids):
  return [actions[x] for x in actions if x in ids]

if __name__ == "__main__":
    
    # set up the avaliable actions
    actions = {
      "none" : a.Action("none", 0, 0, 0, 0),
      "kick_short" : a.Action("kick_short", 1080, 150, 0, 7),
      "sidekick_left" : a.Action("sidekick_left", 750, 150, 90, 10),
      "sidekick_right" : a.Action("sidekick_right", 750, 50, -90, 10)
    }

    all_actions = select(actions, ["none", "kick_short", "sidekick_left", "sidekick_right"])
    
    # repeat it multiple times to see how reliable the result is
    for i in range(10):
        state = State()
        state.pose.rotation = np.radians(180)
        history1 = run(state, optimal_kick_strategy, select(actions, ["none", "kick_short"]))
        
        state = State()
        state.pose.rotation = np.radians(180)
        history2 = run(state, optimal_kick_strategy, all_actions)
        
        state = State()
        state.pose.rotation = np.radians(180)
        history3 = run(state, direct_kick_strategy, all_actions)
        
        plt.clf()
        axes = plt.gca()
        tools.draw_field(axes)
        
        h1 = np.array(history1)
        h2 = np.array(history2)
        h3 = np.array(history3)
        
        plt.plot(h1[:,0], h1[:,1], '-ob')
        plt.plot(h2[:,0], h2[:,1], '-ok')
        plt.plot(h3[:,0], h3[:,1], '-or')
        
        plt.pause(1)
        
