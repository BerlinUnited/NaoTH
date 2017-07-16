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
  resetSamples(samples, 30);
}

KickDirectionSimulator::~KickDirectionSimulator(){}


void KickDirectionSimulator::execute()
{
  size_t num_angle_particle = 30;
  size_t iterations = 1;
  //resetSamples(samples, num_angle_particle);
  
 
  calculate_best_direction(iterations, num_angle_particle);

}//end execute

void KickDirectionSimulator::calculate_best_direction(size_t iterations, size_t num_angle_particle){

  m_min = 0;
  m_max = 0;

  for (size_t i = 0; i < iterations; i++){
    //evaluate the particles

    update(samples);

    //resample
    resample(samples, num_angle_particle, Math::fromDegrees(5.0));

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

    Vector2d to = getRobotPose() * Vector2d(500, 0).rotate(mean_angle);

    PEN("FF0000", 50);
    ARROW(getRobotPose().translation.x, getRobotPose().translation.y, to.x, to.y);
    );
  }
}

void KickDirectionSimulator::update(SampleSet& sampleSet)
{
  //Somehow get a specific action here
  ActionSimulator::ActionParams blablaParams;
  blablaParams.angle = 0.0;
  blablaParams.speed = 1080.0;
  blablaParams.angle_std = 0;
  blablaParams.speed_std = 150;
  double friction = 0.0275;

  ActionSimulator::ActionResults actionsConsequences;
  size_t simulation_num_particles = 1;

  for (size_t i = 0; i < sampleSet.size(); i++){
    blablaParams.angle += Math::toDegrees(sampleSet[i].rotation);
    auto blablaAction = ActionSimulator::Action(KickActionModel::kick_short, blablaParams, friction);
    simulationModule->getModuleT()->simulateAction(blablaAction, actionsConsequences, simulation_num_particles);

    if (actionsConsequences.category(ActionSimulator::BallPositionCategory::INFIELD) > 0){
      sampleSet[i].likelihood = -simulationModule->getModuleT()->evaluateAction(actionsConsequences);
      m_min = std::min(m_min, sampleSet[i].likelihood);
      m_max = std::max(m_max, sampleSet[i].likelihood);
    }
    else if (actionsConsequences.category(ActionSimulator::BallPositionCategory::OPPGOAL) > 0){
      sampleSet[i].likelihood = m_max;
    }
    else{
      sampleSet[i].likelihood = m_min;
    }
    //
    FIELD_DRAWING_CONTEXT;
    std::vector<ActionSimulator::CategorizedBallPosition>::const_iterator ballPosition = actionsConsequences.positions().begin();
    for (; ballPosition != actionsConsequences.positions().end(); ++ballPosition)
    {
      Vector2d ball = getRobotPose() * ballPosition->pos();
      PEN("000000", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
    }
  }

}

void KickDirectionSimulator::resample(SampleSet& sampleSet, int n, double sigma) const
{
  SampleSet oldSampleSet = sampleSet;
  normalize(oldSampleSet);
  //Test normalize for pow2
  for (Sample& s : oldSampleSet) {
    s.likelihood = s.likelihood * s.likelihood;
  }
  normalize(oldSampleSet);
  //END TEST

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