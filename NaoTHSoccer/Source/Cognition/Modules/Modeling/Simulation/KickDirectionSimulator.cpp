/**
* @file KickDirectionSimulator.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class KickDirectionSimulator
*/

#include "KickDirectionSimulator.h"

using namespace naoth;
using namespace std;

KickDirectionSimulator::KickDirectionSimulator()
 // : obstacleFilter(0.01, 0.1)
{

}

KickDirectionSimulator::~KickDirectionSimulator(){}


void KickDirectionSimulator::execute()
{
  resetSamples(samples, 30);
}//end execute

void KickDirectionSimulator::resetSamples(SampleSet& samples, size_t n) const
{
  samples.resize(n);
  double likelihood = 1.0/static_cast<double>(n);
  for(Sample& s: samples) {
    s.likelihood = likelihood;
    s.rotation =  Math::random(-Math::pi, Math::pi);
  }
}

void KickDirectionSimulator::normalize(SampleSet& samples) const
{
  Sample minSample = *std::min_element(samples.begin(), samples.end(), 
                          [](const Sample& a, const Sample& b) { return a.likelihood < b.likelihood; });
  
  double sum = 0.0;
  for(Sample& s: samples) {
    sum += s.likelihood - minSample.likelihood;
  }

  for(Sample& s: samples) {
    s.likelihood /= sum;
  }
}

void KickDirectionSimulator::update(SampleSet& samples) const
{
  std::cout << samples.size();
}

int KickDirectionSimulator::resample(SampleSet& sampleSet, int n, double sigma) const
{
  SampleSet oldSampleSet = sampleSet;
  normalize(oldSampleSet);
  // todo pow2??

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

