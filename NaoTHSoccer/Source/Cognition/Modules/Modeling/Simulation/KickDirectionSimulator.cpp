/**
* @file KickDirectionSimulator.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class KickDirectionSimulator
*/

#include "KickDirectionSimulator.h"
#include <algorithm>
#include <numeric>

using namespace naoth;
using namespace std;

KickDirectionSimulator::KickDirectionSimulator()
 // : obstacleFilter(0.01, 0.1)
{
  simulationModule = registerModule<ActionSimulator>(std::string("Simulation"), true);
  DEBUG_REQUEST_REGISTER("KickDirectionSimulator:draw_best_direction", "best direction", false);
}

KickDirectionSimulator::~KickDirectionSimulator(){}


void KickDirectionSimulator::execute()
{
  size_t num_angle_particle = 30;
  resetSamples(samples, num_angle_particle);
  
  m_min = 0;
  m_max = 0;

  // 10 iterations
  for (size_t i = 0; i < 10;i++){
    //evaluate the particles
    //likelihoods, simulation_consequences, m_min, m_max = update(samples, likelihoods, state, action, m_min, m_max)
    update(samples);
    //resample
    int asdhaosdho = resample(samples, num_angle_particle, 5.0);

    // add random samples - currently disabled
    
    // calculate mean angle
    // TODO do it differently since the values are circular
    double mean_angle = 0;
    for (size_t i = 0; i < samples.size(); i++){
      mean_angle += samples[i].rotation;
    }
    mean_angle = mean_angle / samples.size();

    DEBUG_REQUEST("KickDirectionSimulator:draw_best_direction",
      FIELD_DRAWING_CONTEXT;

      Vector2d to = Vector2d(500, 0).rotate(Math::fromDegrees(mean_angle));
      PEN("FF0000", 50);
      ARROW(0, 0, to.x, to.y);
    );
  }

}//end execute

void KickDirectionSimulator::update(SampleSet& samples)
{
  //Somehow get a specific action here
  ActionSimulator::ActionParams blablaParams;
  blablaParams.angle = 0.0;
  blablaParams.speed = 1080.0;
  blablaParams.angle_std = 9.0;
  blablaParams.speed_std = 150;
  double friction = 0.0275;

  ActionSimulator::ActionResults actionsConsequences;
  size_t simulation_num_particles = 1;

  for (size_t i = 0; i < samples.size();i++){
    blablaParams.angle += samples[i].rotation;
    auto blablaAction = ActionSimulator::Action(KickActionModel::kick_short, blablaParams, friction);
    simulationModule->getModuleT()->simulateAction(blablaAction, actionsConsequences, simulation_num_particles);

    if (actionsConsequences.category(ActionSimulator::BallPositionCategory::INFIELD) > 0){
      samples[i].likelihood = -simulationModule->getModuleT()->evaluateAction(actionsConsequences);
      m_min = std::min(m_min, samples[i].likelihood);
      m_max = std::max(m_max, samples[i].likelihood);
    }
    else if (actionsConsequences.category(ActionSimulator::BallPositionCategory::OPPGOAL) > 0){
      samples[i].likelihood = m_max;
    }
    else{
      samples[i].likelihood = m_min;
    }
  }
 

  //TODO call simulator here with custom action
  std::cout << samples.size(); // just for test delete when finished
}

int KickDirectionSimulator::resample(SampleSet& sampleSet, int n, double sigma) const
{
  SampleSet oldSampleSet = sampleSet;
  normalize(oldSampleSet);

  double likelihood_step = 1.0/n; // the step in the weighting so we get exactly n particles
  
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
    while(targetSum < currentSum && j < oldSampleSet.size())
    {
      sampleSet[j] = oldSampleSet[i];
      targetSum += likelihood_step;

      // noise
      oldSampleSet[i].rotation += (Math::random()-0.5)*2.0*sigma;
      j++;
    }
  }

  return (int)j;
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

  for (Sample& s : samples) {
    s.likelihood /= sum;
  }
}