import math
import copy
import time
import numpy as np

from matplotlib import pyplot as plt
from matplotlib.patches import Circle

from tools import action as a
from tools import Simulation as Sim
from tools import potential_field as pf
from naoth import math2d as m2d
from tools import tools



class State:
    def __init__(self, x, y):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(x,y)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates


def draw_actions(actions_consequences, state, best_action, normal_angle, better_angle, angle_list):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.text(0, 0, best_action, fontsize=12)

    origin = state.pose.translation
    #arrow_head = m2d.Vector2(500, 0).rotate(math.radians(normal_angle + better_angle))

    for angle in angle_list:
        arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + angle))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

    #arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + better_angle))
    #axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
    
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
  #M = np.max(likelihood)
  l = likelihood - m
  return l/np.sum(l)
  
  
def resample(samples, likelihoods, n, sigma):
  
  new_samples = np.zeros(n)
  likelihoods = normalize(likelihoods)
  likelihoods = normalize(np.power(likelihoods,2))
  
  likelihood_step = 1.0/float(n)
  targetSum = np.random.random(1)*likelihood_step
  currentSum = 0.0
  
  j = 0
  for (s,v) in zip(samples,likelihoods):
    currentSum += v

    while targetSum < currentSum and j < len(new_samples):
      new_samples[j] = s + (np.random.random(1) - 0.5)*2*sigma
      targetSum += likelihood_step
      j += 1
  
  return new_samples

  
def update(samples, likelihoods, state, action, mm, mM):
  #likelihoods = np.zeros(samples.shape)
  test_action = copy.deepcopy(action)
  simulation_consequences = []
  
  simulation_num_particles = 1
  for i in range(0, len(samples)):
    # modify the action with the sample
    test_action.angle = action.angle + samples[i]
    
    test_action_consequence = a.ActionResults([])
    simulation_consequences.append(Sim.simulate_consequences(test_action, test_action_consequence, state, simulation_num_particles))
    
    if test_action_consequence.category("INFIELD") > 0:
      potential = -pf.evaluate_action2(test_action_consequence, state)
      likelihoods[i] = potential
      mm = min(mm, potential)
      mM = max(mM, potential)
    elif test_action_consequence.category("OPPGOAL") > 0:
      likelihoods[i] = mM
    else:
      likelihoods[i] = mm
            
  return likelihoods, simulation_consequences, mm, mM


  
def calculate_best_direction(x,y, iterations, show):
    state = State(x,y)
    action = a.Action("test", 1000, 150, 0, 8)
    
    # particles
    num_angle_particle = 30
    n_random = 0;
    
    samples = (np.random.random(num_angle_particle)-0.5)*2* 180.0
    likelihoods = np.ones(samples.shape) * (1/float(num_angle_particle))
    
    ####################################################################################################################
    mm = 0;
    mM = 0;
    mean_angle = None
    for iteration in range(1,iterations):
        
        # evaluate the particles
        likelihoods, simulation_consequences, mm, mM = update(samples, likelihoods, state, action, mm, mM)
          
        #resample
        samples = resample(samples, likelihoods, num_angle_particle, 5.0)
        
        # add random samples 
        if n_random > 0:
          samples[(num_angle_particle-n_random):(num_angle_particle)] = np.random.random(n_random) * 360.0
        
        mean_angle = np.mean(samples)
        
        if show:
          draw_actions(simulation_consequences, state, action.name, action.angle,  mean_angle, samples)
    
    return np.radians(mean_angle), np.radians(np.std(samples))
    
    

if __name__ == "__main__":

  single_run = True

  
  if single_run:
    # run a single directon
    calculate_best_direction(1000, 1000, 50, True)
  else:
    # run for the whole field
    x = range(-5200,5300,250)
    y = range(-3700,3800,250)
    xx, yy = np.meshgrid(x, y)
    vx = np.zeros(xx.shape)
    vy = np.zeros(xx.shape)
    f = np.zeros(xx.shape)
    
    print xx.shape, len(x), len(y)
    for ix in range(0,len(x)):
      for iy in range(0,len(y)):
        direction, direction_std = calculate_best_direction(float(x[ix]), float(y[iy]), 10, False)
        
        v = m2d.Vector2(100.0, 0.0).rotate(direction)
        vx[iy, ix] = v.x
        vy[iy, ix] = v.y
        f[iy, ix] = direction_std
        
    plt.figure()
    tools.draw_field()
    Q = plt.quiver(xx, yy, vx, vy, np.degrees(f))
    plt.show()
