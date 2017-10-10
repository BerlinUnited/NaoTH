from __future__ import division
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


def draw_robot_walk(actions_consequences, s, expected_ball_pos, best_action):
    plt.clf()
    axes = plt.gca()
    origin = s.pose.translation
    arrow_head = m2d.Vector2(500, 0).rotate(s.pose.rotation)
    x = np.array([])
    y = np.array([])

    tools.draw_field(axes)
    axes.add_artist(Circle(xy=(s.pose.translation.x, s.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.add_artist(Circle(xy=(expected_ball_pos.x, expected_ball_pos.y), radius=120, fill=True, edgecolor='blue'))
    axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
    axes.text(-4500, 3150, best_action, fontsize=12)
    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = s.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.5)


def main(x, y, rot, s, num_iter):
    s.update_pos(m2d.Vector2(x, y), rotation=rot)

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1080, 150, 0, 7)
    sidekick_left = a.Action("sidekick_left", 750, 150, 90, 10)
    sidekick_right = a.Action("sidekick_right", 750, 50, -90, 10)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    # Do several decision cycles not just one to get rid of accidental decisions
    #timings = []
    #n_kicks = []
    #n_turns = []
    num_kicks_list = []
    num_turn_degrees_list = []
    num_turn_ball_degrees_list = []
    dist_walked_list = []
    
    current_iter = 0
    #for idx in range(num_iter):
    while len(num_kicks_list) < num_iter:
        current_iter += 1
        if current_iter > num_iter:
            print("WARNING: overiteration " + num_iter)
            
        num_kicks = 0
        num_turn_degrees = 0
        num_turn_ball_degrees = 0
        dist_walked = 0
        
        goal_scored = False
        #total_time = 0
        chosen_rotation = 'none'  # This is used for deciding the rotation direction once per none decision
        s.update_pos(m2d.Vector2(x, y), rotation=rot)
        while not goal_scored:
            actions_consequences = []
            # Simulate Consequences
            for action in action_list:
                single_consequence = a.ActionResults([])
                actions_consequences.append(Sim.simulate_consequences(action, single_consequence, s, num_particles=30))

            # Decide best action
            best_action = Sim.decide_smart(actions_consequences, s)

            # expected_ball_pos should be in local coordinates for rotation calculations
            expected_ball_pos = actions_consequences[best_action].expected_ball_pos_mean

            # Check if expected_ball_pos inside opponent goal
            opp_goal_back_right = m2d.Vector2(field.opponent_goalpost_right.x + field.goal_depth, field.opponent_goalpost_right.y)
            opp_goal_box = m2d.Rect2d(opp_goal_back_right, field.opponent_goalpost_left)

            goal_scored = opp_goal_box.inside(s.pose * expected_ball_pos)
            inside_field = field.field_rect.inside(s.pose * expected_ball_pos)
            if goal_scored:
                num_kicks += 1
                # print("Goal " + str(total_time) + " " + str(math.degrees(s.pose.rotation)))
                rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
                distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
                #rotation_time = np.abs(math.degrees(rotation) / s.rotation_vel)
                #distance_time = distance / s.walking_vel
                #total_time += distance_time + rotation_time
                
                dist_walked += distance
                num_turn_degrees += np.abs(math.degrees(rotation)) #turn around own axis
                
                break

            elif not inside_field:# and not goal_scored: # it already in the else of if goal_scored
                # Asserts that expected_ball_pos is inside field or inside opp goal
                # print("Error: This position doesn't manage a goal")
                #total_time = float('nan')
                
                num_kicks = np.nan
                num_turn_degrees = np.nan
                num_turn_ball_degrees = np.nan
                dist_walked = np.nan
                break

            elif not action_list[best_action].name == "none":

                # draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)

                # calculate the time needed
                rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
                distance = np.hypot(expected_ball_pos.x, expected_ball_pos.y)
                
                #rotation_time = np.abs(math.degrees(rotation) / s.rotation_vel)
                #distance_time = distance / s.walking_vel
                #total_time += distance_time + rotation_time

                dist_walked += distance
                num_turn_ball_degrees += np.abs(math.degrees(rotation)) #turn around ball
                
                # update total turns
                #num_turn_degrees += np.abs(math.degrees(rotation))

                # reset the rotation direction
                chosen_rotation = 'none'

                # update the robots position
                s.update_pos(s.pose * expected_ball_pos, math.degrees(s.pose.rotation + rotation))
                num_kicks += 1

            elif action_list[best_action].name == "none":
                # print(str(state.pose * expected_ball_pos) + " Decision: " + str(action_list[best_action].name))
                # draw_robot_walk(actions_consequences, state, state.pose * expected_ball_pos, action_list[best_action].name)
                turn_rotation_step = 1
                # Calculate rotation time
                #total_time += np.abs(turn_rotation_step / s.rotation_vel)
                #num_turn_degrees += turn_rotation_step
                num_turn_ball_degrees += turn_rotation_step #turn around ball

                attack_direction = attack_dir.get_attack_direction(s)

                if (attack_direction > 0 and chosen_rotation) is 'none' or chosen_rotation is 'left':
                    s.update_pos(s.pose.translation, math.degrees(s.pose.rotation) + turn_rotation_step)  # Should turn right
                    chosen_rotation = 'left'
                elif (attack_direction <= 0 and chosen_rotation is 'none') or chosen_rotation is 'right':
                    s.update_pos(s.pose.translation, math.degrees(s.pose.rotation) - turn_rotation_step)  # Should turn left
                    chosen_rotation = 'right'

            else:
                sys.exit("There should not be other actions")
        # print("Total time to goal: " + str(total_time))
        # print("Num Kicks: " + str(num_kicks))
        # print("Num Turns: " + str(num_turn_degrees))

        #timings.append(total_time)
        #n_kicks.append(num_kicks)
        #n_turns.append(num_turn_degrees)
        
        num_kicks_list += [num_kicks]
        num_turn_degrees_list += [num_turn_degrees]
        num_turn_ball_degrees_list += [num_turn_ball_degrees]
        dist_walked_list += [dist_walked]

    return np.nanmean(num_kicks_list), np.nanmean(num_turn_degrees_list), np.nanmean(num_turn_ball_degrees_list), np.nanmean(dist_walked_list)


if __name__ == "__main__":
    # repeat it multiple times to see how reliable the result is
    for i in range(30):
        state = State()
        time, kicks, turns = main(state.pose.translation.x, state.pose.translation.y, math.degrees(state.pose.rotation), state, num_iter=10)
        print("Total time to goal: " + str(time) + " #Kicks: " + str(kicks) + " #Turns: " + str(turns))
