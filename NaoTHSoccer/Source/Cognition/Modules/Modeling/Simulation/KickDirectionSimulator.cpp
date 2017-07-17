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
  resetSamples(samples, 30);

  //calculate the actions
  
  action_local.reserve(KickActionModel::numOfActions); // here we dont simulate the none action so it could be numOfActions-1
  action_local.push_back(ActionSimulator::Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
  action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
  action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right

  actionsConsequences.resize(action_local.size());  
}

KickDirectionSimulator::~KickDirectionSimulator(){}


void KickDirectionSimulator::execute()
{
  //resetSamples(samples, num_angle_particle);

  //TestCase with action defined here - should produce the same result as before
  //Somehow get a specific action here
  ActionSimulator::ActionParams blablaParams;
  blablaParams.angle = 0.0;
  blablaParams.speed = 1080.0;
  blablaParams.angle_std = 6;
  blablaParams.speed_std = 150;
  ActionSimulator::Action& blablaAction = ActionSimulator::Action(KickActionModel::kick_short, blablaParams, theParameters.friction);
  calculate_best_direction(blablaAction);


  //end test Case
  // simulate the best directions for all actions
  /*
  for (size_t i = 0; i < action_local.size(); i++) {
    calculate_best_direction(); // TODO do it for every action
    //simulateConsequences(action_local[i], actionsConsequences[i]);
  }
  */
  FIELD_DRAWING_CONTEXT;
  for (size_t i = 0; i < samples.size(); i++)
  {
    Vector2d point(100,0);
    point.rotate(samples[i].rotation);
    point = getRobotPose() * point;
    PEN("000000", 1);
    FILLOVAL(point.x, point.y, 50, 50);
  }
  
}//end execute

void KickDirectionSimulator::calculate_best_direction(ActionSimulator::Action& action){

  m_min = 0;
  m_max = 0;

  for(int i = 0; i < theParameters.iterations; i++){
    //evaluate the particles

    update(action);

    //resample
    resample(samples, Math::fromDegrees(5.0));

    // add random samples - currently disabled

    // calculate mean angle
    double a = 0;
    double b = 0;
    for (size_t i = 0; i < samples.size(); ++i)
    {
      a += sin(samples[i].rotation);
      b += cos(samples[i].rotation);
    }
    double mean_angle = atan2(a, b);

    DEBUG_REQUEST("KickDirectionSimulator:draw_best_direction",
      FIELD_DRAWING_CONTEXT;

    Vector2d to = getRobotPose() * Vector2d(500, 0).rotate(mean_angle);

      PEN("FF0000", 50);
      ARROW(getRobotPose().translation.x, getRobotPose().translation.y, to.x, to.y);
    );
  }
}

void KickDirectionSimulator::update(ActionSimulator::Action& blablaAction)
{
  for (size_t i = 0; i < samples.size(); i++){
    size_t simulation_num_particles = 1;
    ActionSimulator::ActionResults turningConsequences;

    //FIXME Dirty getter and setter
    blablaAction.setAngle(blablaAction.getAngle() + Math::toDegrees(samples[i].rotation));
    
    simulationModule->getModuleT()->simulateAction(blablaAction, turningConsequences, simulation_num_particles);

    if (turningConsequences.category(ActionSimulator::BallPositionCategory::INFIELD) > 0){
      samples[i].likelihood = -simulationModule->getModuleT()->evaluateAction(turningConsequences);
      m_min = std::min(m_min, samples[i].likelihood);
      m_max = std::max(m_max, samples[i].likelihood);
    }
    else if (turningConsequences.category(ActionSimulator::BallPositionCategory::OPPGOAL) > 0){
      samples[i].likelihood = m_max;
    }
    else{
      samples[i].likelihood = m_min;
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

void KickDirectionSimulator::normalize(SampleSet& samples) const
{
  Sample minSample = *std::min_element(samples.begin(), samples.end(),
    [](const Sample& a, const Sample& b) { return a.likelihood < b.likelihood; });

  double sum = 0.0;
  for (Sample& s : samples) {
    sum += s.likelihood - minSample.likelihood;
  }
  if (sum != 0.0){
    for (Sample& s : samples) {
      s.likelihood /= sum;
    }
  }
  
}