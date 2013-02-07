
#include "PostParticleFilter.h"

#include "Tools/Math/Probabilistics.h"

void PostParticleFilter::updateByOdometry(const Pose2D& odometryDelta)
{
  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    // move each particle with odometry
    sampleSet[i].translation = odometryDelta * sampleSet[i].translation;

    // apply some noise
    sampleSet[i].translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
    sampleSet[i].translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
  }
}//end updateByOdometry


void PostParticleFilter::updateByGoalPostPercept(const GoalPercept::GoalPost& post) 
{
  double totalWeighting = 0;

  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    AGLSample& sample = sampleSet[i];
    double weighting(0.0);

    double diff = Math::normalize(sample.getPos().angle() - post.position.angle()); //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
    weighting = Math::gaussianProbability(diff, parameters.standardDeviationAngle);

    sample.likelihood *= weighting;
    totalWeighting += sample.likelihood;
  }//end for

  sampleSet.lastTotalWeighting = totalWeighting;
}//end updateByGoalPostPercept


double PostParticleFilter::getConfidenceForObservation(const GoalPercept::GoalPost& post) const
{
  //TODO check Color -> not similar, return 0
  double weighting(0.0);
  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
    double value = Math::normalize(sampleSet[i].getPos().angle() - post.position.angle()); 
    weighting = Math::gaussianProbability(value, parameters.standardDeviationAngle);
  }

  if (weighting > parameters.weightingTreshholdForUpdateWithAngle)//TODO: check value
    return weighting;
  else
    return 0;
}//end getWeightingOfPerceptAngle


void PostParticleFilter::resampleGT07(bool noise)
{
  double totalWeighting = 0;

  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for

  // copy the samples
  // TODO: use memcopy?
  AGLSampleSet oldSampleSet = sampleSet;

  totalWeighting += parameters.resamplingThreshhold * oldSampleSet.size();
  for (unsigned int i = 0; i < oldSampleSet.size(); i++) 
  {
    oldSampleSet[i].likelihood += parameters.resamplingThreshhold;
    oldSampleSet[i].likelihood /= totalWeighting; // normalize
  }//end for


  // resample 10% of particles
  //int numberOfPartiklesToResample = (int) (((double) sampleSet.size())*0.1 + 0.5);

  double sum = -Math::random();
  unsigned int count = 0;

  //unsigned int m = 0; // Zaehler durchs Ausgangs-Set
  unsigned int n = 0; // Zaehler durchs Ziel-Set

  for (unsigned m = 0; m < sampleSet.size(); m++) 
  {
    sum += oldSampleSet[m].likelihood * oldSampleSet.size();

    // select the particle to copy
    while (count < sum && count < oldSampleSet.size()) 
    {
      if (n >= oldSampleSet.size()) break;

      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if (noise) 
      {
        sampleSet[n].translation.x += (Math::random() - 0.5) * parameters.processNoiseDistance;
        sampleSet[n].translation.y += (Math::random() - 0.5) * parameters.processNoiseDistance;
      }

      n++;
      count++;

    }//end while
  }//end for

}//end resampleGT07
