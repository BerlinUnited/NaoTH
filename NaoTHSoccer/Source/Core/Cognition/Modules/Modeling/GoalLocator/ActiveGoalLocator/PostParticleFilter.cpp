
#include "PostParticleFilter.h"

#include "Tools/Math/Probabilistics.h"

// Debug
#include "Tools/Debug/DebugDrawings.h"

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

  return weighting;
}//end getWeightingOfPerceptAngle


void PostParticleFilter::resampleGT07(std::vector<GoalPercept::GoalPost> postArr, bool noise)
{
  // calculate the weighting (BH paper)
  // not used yet

  double totalWeighting = 0;
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for
  const double averageWeighting = totalWeighting / sampleSet.size();
  static double w_slow = averageWeighting;
  static double w_fast = averageWeighting;
  double alpha_slow = 0.0059;
  double alpha_fast = 0.006;

  w_slow += alpha_slow*(averageWeighting - w_slow);
  w_fast += alpha_fast*(averageWeighting - w_fast);


  //TODO: to test particle filter module
  //double pp = max(0.0, 1.0 - w_fast/w_slow);
  //DEBUG_REQUEST("AGL:plots",
  //  PLOT("ActiveGoalLocator:pp", pp);
  //  PLOT("ActiveGoalLocator:averageWeighting", averageWeighting);
  //);

  // copy the samples
  // TODO: use memcopy?
  AGLSampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize();

  // add a uniform offset for stabilization
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    oldSampleSet[i].likelihood += parameters.resamplingThreshhold;
  }//end for
  oldSampleSet.normalize();


  // resample 10% of particles
  int numberOfParticlesToResample = 1; //(int)(((double)sampleSet.numberOfParticles)*0.05+0.5);

  //numberOfPartiklesToResample = (int)pp;

  // don't resample if no global information is available
  //if(getGoalPercept().getNumberOfSeenPosts() + getLinePercept().flags.size() < 2 &&
  //   !getLinePercept().middleCircleWasSeen)
  //{
    // TODO: add crossings as global information
    //numberOfPartiklesToResample = 0;
  //}

  /*
  if(getGoalPercept().getNumberOfSeenPosts() == 0)
    numberOfPartiklesToResample = 0;
    */

  double sum = -Math::random();
  unsigned int count = 0;

  unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  for(m = 0; m < sampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * oldSampleSet.size();

    // select the particle to copy
    while(count < sum && count < oldSampleSet.size())
    {
      if (n >= oldSampleSet.size() - numberOfParticlesToResample) break;

      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise)
      {
        sampleSet[n].translation.x += (Math::random()-0.5)*parameters.processNoiseDistance;
        sampleSet[n].translation.y += (Math::random()-0.5)*parameters.processNoiseDistance;
        //sampleSet[n].rotation = Math::normalize(sampleSet[n].rotation + (Math::random()-0.5)*parameters.processNoiseAngle);
      }

      n++;
      count++;
    }//end while

    if (n >= oldSampleSet.size()-numberOfParticlesToResample) break;
  }//end for



  // sensor resetting by whole goal
  //if(n < oldSampleSet.size())
  //{
  //  n = sensorResetBySensingGoalModel(sampleSet, n);
  //}

  //sensor resetting by the goal posts
  if(postArr.size() > 0)
  {
    //TODO: does not work properly yet
    //n = sensorResetByGoalPosts(sampleSet, n);
  }


  // fill up by copying random samples
  // (shouldn't happen)
  while (n < sampleSet.size())
  {
    int i = Math::random(sampleSet.size());//(int)(Math::random()*(sampleSet.numberOfParticles-1) + 0.5);
    sampleSet[n] = sampleSet[i];
    n++;

    std::cout << "fill one more sample " << n << " ready" << std::endl;
  }//end while

}//end resampleGT07


void PostParticleFilter::drawParticles(const std::string& color, int idx) const
{
  if (!sampleSet.getIsValid()) {
    return;
  }

  // normalize the colors (black: less importent, red more importent)
  double minValue = 1;
  double maxValue = 0;
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    maxValue = max(sampleSet[i].likelihood, maxValue);
    minValue = min(sampleSet[i].likelihood, minValue);
  }
  double colorDiff = maxValue-minValue;

  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    const AGLSample& sample = sampleSet[i];

    PEN(color, 10);
    CIRCLE(sample.translation.x, sample.translation.y, 20);
    TEXT_DRAWING(sample.translation.x+10, sample.translation.y+10, idx);


    DebugDrawings::Color inner_color;
    if(colorDiff > 0) {
      inner_color[0] = Math::clamp((sample.likelihood - minValue)/colorDiff,0.0,1.0);
    }

    PEN(inner_color, 10);
    FILLOVAL(sample.translation.x, sample.translation.y, 10, 10);
  }//end for
}//end drawParticles


void PostParticleFilter::drawSamplesImportance() const
{
  // normalize the colors (black: less importent, red more importent)
  double minValue = 1;
  double maxValue = 0;
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    maxValue = max(sampleSet[i].likelihood, maxValue);
    minValue = min(sampleSet[i].likelihood, minValue);
  }
  double colorDiff = maxValue-minValue;

  // plot samples
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    const AGLSample& sample = sampleSet[i];

    DebugDrawings::Color color;
    if(colorDiff > 0) {
      color[0] = Math::clamp((sample.likelihood - minValue)/colorDiff,0.0,1.0);
    }

    PEN(color, 20);
    CIRCLE(sample.translation.x, sample.translation.y, 20);
  }//end for
}//end drawSamplesImportance
