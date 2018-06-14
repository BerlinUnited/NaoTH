/**
* @file KickDirectionSimulator.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class KickDirectionSimulator
*/

#include "KickDirectionSimulator.h"
#include <algorithm>
#include <cmath>

using namespace naoth;
using namespace std;

KickDirectionSimulator::KickDirectionSimulator()
 // : obstacleFilter(0.01, 0.1)
{
  simulationModule = registerModule<ActionSimulator>(std::string("Simulation"), true);
  
  DEBUG_REQUEST_REGISTER("KickDirectionSimulator:draw_best_direction", "best direction", false);
  
  //resetSamples(samples, theParameters.num_angle_particle);
  resetSamplesUniform(samples, theParameters.num_angle_particle);
  //calculate the actions
  
  action_local.reserve(KickActionModel::numOfActions); // here we dont simulate the none action so it could be numOfActions-1
  action_local.push_back(ActionSimulator::Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
  action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
  action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right

  correctionAngle.resize(action_local.size()); 

  getDebugParameterList().add(&theParameters);
}

KickDirectionSimulator::~KickDirectionSimulator()
{
  getDebugParameterList().remove(&theParameters);
}


void KickDirectionSimulator::execute()
{
  int bestActionID = 0;
  
  for (size_t i = 0; i < action_local.size(); i++) 
  {

    correctionAngle[i] = calculate_best_direction(action_local[i]);
    
    // remember the smallest rotation
    if (i == 0 || correctionAngle[i] < correctionAngle[bestActionID]) {
      bestActionID = i;
    }

    DEBUG_REQUEST("KickDirectionSimulator:draw_best_direction",
      FIELD_DRAWING_CONTEXT;

      Vector2d to = getRobotPose() * (Vector2d(500, 0).rotate(action_local[i].getAngle()).rotate(correctionAngle[i]));

      PEN("FF0000", 50);
      ARROW(getRobotPose().translation.x, getRobotPose().translation.y, to.x, to.y);
    );

  }

  getKickActionModel().bestAction = KickActionModel::none;
  
  //TODO replace 5.0 with value of angle_std
  if (fabs(correctionAngle[bestActionID]) < Math::fromDegrees(5.0)) {
    getKickActionModel().bestAction = action_local[bestActionID].id();
  }

  // if the robot is not already rotated close enough to the optimized rotation of the chosen
  // kick calculate the attackDirection and leave Action to none
  getSoccerStrategy().attackDirection = Vector2d(1.0,0.0).rotate(correctionAngle[bestActionID]);

  //getKickActionModel().rotation = actionsConsequences[bestActionID];
  //std::cout << "Best Action: " << action_local[bestActionID].name() << std::endl;
  /*
  FIELD_DRAWING_CONTEXT;
  for (size_t i = 0; i < samples.size(); i++)
  {
    Vector2d point(100,0);
    point.rotate(samples[i].rotation);
    point = getRobotPose() * point;
    PEN("000000", 1);
    FILLOVAL(point.x, point.y, 50, 50);
  }
  */
}//end execute

double KickDirectionSimulator::calculate_best_direction(const ActionSimulator::Action& basisAction)
{
  //resetSamples(samples, theParameters.num_angle_particle);
  resetSamplesUniform(samples, theParameters.num_angle_particle);
  
  double noise = Math::fromDegrees(30.0);
  double min_noise = Math::fromDegrees(3.0);

  for(int i = 0; i < theParameters.iterations; i++)
  {
    //evaluate the particles
    update(basisAction);

    //resample
    resample(samples, noise);

    noise = std::max(min_noise, noise*0.5);
  }

  // calculate mean angle
  double a = 0;
  double b = 0;
  for (size_t i = 0; i < samples.size(); ++i) {
    a += sin(samples[i].rotation);
    b += cos(samples[i].rotation);
  }
  double mean_angle = atan2(a, b);

  return mean_angle;
}

void KickDirectionSimulator::update(const ActionSimulator::Action& basisAction)
{
  ActionSimulator::Action test_action(basisAction);
  size_t simulation_num_particles = 1;
  ActionSimulator::ActionResults turningConsequences;

  struct Stats {
    double oppGoal;
    double inFiled;
    double potential;
  };

  // store some statistics for later evaluation
  static std::vector<Stats> stats;
  stats.resize(samples.size());

  // evaluate all particles
  for (size_t i = 0; i < samples.size(); i++)
  {
    //FIXME Dirty getter and setter
    test_action.setAngle(basisAction.getAngle() + Math::toDegrees(samples[i].rotation));
    
    //turningConsequences.reset(); // reset is inside simulateAction
    simulationModule->getModuleT()->simulateAction(test_action, turningConsequences, simulation_num_particles);

    stats[i].oppGoal = turningConsequences.likelihood(ActionSimulator::BallPositionCategory::OPPGOAL);
    stats[i].inFiled = turningConsequences.likelihood(ActionSimulator::BallPositionCategory::INFIELD);
    
    if(stats[i].oppGoal + stats[i].inFiled > 0) {
      stats[i].potential = -simulationModule->getModuleT()->evaluateAction(turningConsequences);
      m_min = std::min(m_min, stats[i].potential);
      m_max = std::max(m_max, stats[i].potential);
    } else {
      stats[i].potential = 0.0;
    }
  }

  double diff = m_max - m_min;
  if (diff == 0) {
    diff = 1.0;
  }
      
  // calculate the weights for the particles
  for (size_t i = 0; i < samples.size(); i++)
  {
    if(stats[i].oppGoal + stats[i].inFiled > theParameters.good_threshold_percentage) {
      samples[i].likelihood = (stats[i].oppGoal + stats[i].inFiled)*((stats[i].potential - m_min) / diff) + stats[i].oppGoal;
    } else {
      samples[i].likelihood = 0.0;
    }
  }
}

void KickDirectionSimulator::resample(SampleSet& sampleSet, double sigma) const
{
  SampleSet oldSampleSet = sampleSet;
  normalize(oldSampleSet);
  //Test normalize for pow2
  for (Sample& s : oldSampleSet) {
    s.likelihood = s.likelihood * s.likelihood;
  }
  normalize(oldSampleSet);
  //END TEST

  double likelihood_step = 1.0 / theParameters.num_angle_particle; // the step in the weighting so we get exactly n particles
  
  double targetSum = Math::random()*likelihood_step;
  double currentSum = 0;

  // Stochastic universal sampling
  // i - count over the old sample set
  // j - over the new one :)
  size_t j = 0;
  for(size_t i = 0; i < oldSampleSet.size(); i++)
  {
    currentSum += oldSampleSet[i].likelihood;

    // select the particle to copy
    while (targetSum < currentSum && j < sampleSet.size())
    {
      sampleSet[j] = oldSampleSet[i];
      targetSum += likelihood_step;

      // noise
      sampleSet[j].rotation += (Math::random()-0.5)*2.0*sigma;
      j++;
    }
  }
}

void KickDirectionSimulator::resetSamples(SampleSet& samples, size_t n) const
{
  samples.resize(n);
  double likelihood = 1.0 / static_cast<double>(n);
  for (Sample& s : samples) {
    s.likelihood = likelihood;
    s.rotation = Math::random(-Math::pi, Math::pi);
  }
}

void KickDirectionSimulator::resetSamplesUniform(SampleSet& samples, size_t n) const
{
  samples.resize(n);
  double likelihood = 1.0 / static_cast<double>(n);
  double step = Math::pi2 / static_cast<double>(n);

  for (size_t i = 0; i < n; ++i) {
    samples[i].likelihood = likelihood;
    samples[i].rotation = -Math::pi + i*step;
  }
}

void KickDirectionSimulator::normalize(SampleSet& samples) const
{
  Sample minSample = *std::min_element(samples.begin(), samples.end(),
    [](const Sample& a, const Sample& b) { return a.likelihood < b.likelihood; });

  double sum = 0.0;
  for (Sample& s : samples) {
    s.likelihood -= minSample.likelihood;
    sum += s.likelihood;
  }
  if (sum != 0.0){
    for (Sample& s : samples) {
      s.likelihood /=  sum;
    }
  }
  
}
