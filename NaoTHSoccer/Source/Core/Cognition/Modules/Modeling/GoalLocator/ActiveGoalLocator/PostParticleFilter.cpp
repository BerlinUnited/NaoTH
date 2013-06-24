
#include "PostParticleFilter.h"

#include "Tools/Math/Probabilistics.h"

// Debug
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace std;

PostParticleFilter::PostParticleFilter()
  : sampleSet(10)
{
}

void PostParticleFilter::updateByOdometry(const Pose2D& odometryDelta)
{
  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    Pose2D noisyOdometryDelta(
      odometryDelta.rotation + (Math::random()-0.5)*parameters.motionNoiseRotation,
      odometryDelta.translation.x + (Math::random()-0.5)*parameters.motionNoiseDistance,
      odometryDelta.translation.x + (Math::random()-0.5)*parameters.motionNoiseDistance);

    // move each particle with odometry
    sampleSet[i].translation = noisyOdometryDelta * sampleSet[i].translation;
  }
}//end updateByOdometry


double PostParticleFilter::getWeightingByPercept(const AGLSample& sample, const GoalPercept::GoalPost& post ,const double cameraHeight) const
{
    double weightingByAngle(1.0);
    double weightingByDistance(1.0);

    // angle
    double angleDiff = Math::normalize(sample.getPos().angle() - post.position.angle()); //expected - measuered ,normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
    weightingByAngle = Math::gaussianProbability(angleDiff, parameters.standardDeviationAngle);

    // distenace
    if(post.positionReliable)
    {
      const double measuredDistanceAsAngle = atan2(post.position.abs(), cameraHeight);
      const double expectedDistanceAsAngle = atan2(sample.getPos().abs(), cameraHeight);

      double distDif = Math::normalize(expectedDistanceAsAngle - measuredDistanceAsAngle);
      weightingByDistance = Math::gaussianProbability(distDif, parameters.standardDeviationDistance);
    }

    return (weightingByAngle * weightingByDistance);

}//end getWeightingByPercept

void PostParticleFilter::updateByGoalPostPercept(const GoalPercept::GoalPost& post, const double cameraHeight)
{
  double totalWeighting = 0;

  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    sampleSet[i].likelihood *= getWeightingByPercept(sampleSet[i], post, cameraHeight);
    totalWeighting += sampleSet[i].likelihood;
  }//end for

  sampleSet.lastTotalWeighting = totalWeighting;

  //merkt sich beobachtungen um im resampling sensor updates zu machen
  PerceptBuffer.add(post.position);
}//end updateByGoalPostPercept


// experimental
double PostParticleFilter::getMeanVerticalDeviation(const GoalPercept::GoalPost& post, const double cameraHeight) const
{
  double meanVerticalError = 0;
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    // experimental
    if(post.positionReliable)
    {
      const double measuredDistanceAsAngle = atan2(post.position.abs(), cameraHeight);
      const double expectedDistanceAsAngle = atan2(sampleSet[i].getPos().abs(), cameraHeight);

      double distDif = Math::normalize(expectedDistanceAsAngle - measuredDistanceAsAngle);
      meanVerticalError += distDif;
    }
  }//end for

  return meanVerticalError /= (double)sampleSet.size();
}//end getMeanVerticalDeviation

double PostParticleFilter::getMeanHorizontalDeviation(const GoalPercept::GoalPost& post) const
{
  double meanHorizontalError = 0;
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    double angleDiff = Math::normalize(sampleSet[i].getPos().angle() - post.position.angle()); //expected - measuered ,normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
    meanHorizontalError += angleDiff;
  }//end for

  return meanHorizontalError /= (double)sampleSet.size();
}//end getMeanHorizontalDeviation


double PostParticleFilter::getConfidenceForObservation(const GoalPercept::GoalPost& post, const double cameraHeight) const
{
  //TODO check Color -> not similar, return 0
  double weighting(0.0);
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    weighting += getWeightingByPercept(sampleSet[i], post, cameraHeight);
  }

  return weighting / (double)sampleSet.size();
}//end getWeightingOfPerceptAngle


void PostParticleFilter::resampleGT07(bool noise)
{
  // calculate the weighting (BH paper)
  // not used yet

  /*
  double totalWeighting = 0;
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for
  const double averageWeighting = totalWeighting / sampleSet.size();
  static double w_slow = averageWeighting;
  static double w_fast = averageWeighting;
  const double alpha_slow = 0.0059;
  const double alpha_fast = 0.006;

  w_slow += alpha_slow*(averageWeighting - w_slow);
  w_fast += alpha_fast*(averageWeighting - w_fast);

  double sensorResetCount = max(0.0, 1.0 - w_fast/w_slow);
  PLOT("ActiveGoalLocator:sensorResetCount", sensorResetCount);
  */

  // copy the samples
  AGLSampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize();

  // add a uniform offset for stabilization
  // "verringerung der relativen unterschiede ... "
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    oldSampleSet[i].likelihood += parameters.resamplingThreshhold;
  }//end for
  oldSampleSet.normalize();


  // resample 10% of particles
  int countOfSensorReset = 0; //(int)(((double)sampleSet.numberOfParticles)*0.05+0.5);

  if(parameters.particlesToReset > 0) {
    countOfSensorReset = (int)parameters.particlesToReset;
  }

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

  const double cameraHeight = 450;
  double sum = -Math::random();
  //eigentlich zweite summe, hier aber wird der indize verwendet

  unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  //low variance sampling
  for(m = 0; m < oldSampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * (double)oldSampleSet.size();

    // select the particle to copy
    while(n < sum && n < oldSampleSet.size())
    {

      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise)
      {
        const double rotationNoise = (Math::random()-0.5)*parameters.processNoiseRotation;
        const double distanceNoise = (Math::random()-0.5)*parameters.processNoiseDistance;
        const double expectedDistanceAsAngle = atan2(sampleSet[n].getPos().abs(), cameraHeight);
        const double distance = cameraHeight*tan(expectedDistanceAsAngle + distanceNoise);

        sampleSet[n].translation.rotate(rotationNoise);
        sampleSet[n].translation.normalize(distance);
      }

      n++;
    }//end while

  }//end for


  //entscheidung zum sensor reset.
  //TODO: wenn filter auseinander laeuft (sensor reset noetig), und keine percepte im buffer (weil zu alt) -> filter loeschen???
  if (PerceptBuffer.size() > 0) {

    //later w_low/w_fast instead numberOf...
    for (unsigned int i = 0; i < (unsigned int)countOfSensorReset; i++)
    {
      unsigned int iToResetSample = Math::random((int)sampleSet.size());

      unsigned int iToResetObserv = Math::random((int)PerceptBuffer.size());

      sampleSet[iToResetSample].translation.x = PerceptBuffer.getEntry(iToResetObserv).x;
      sampleSet[iToResetSample].translation.y = PerceptBuffer.getEntry(iToResetObserv).y;
      sampleSet[iToResetSample].likelihood = 0.1;

      sampleSet[iToResetSample].translation.x += (Math::random()-0.5)*parameters.processNoiseDistance;
      sampleSet[iToResetSample].translation.y += (Math::random()-0.5)*parameters.processNoiseDistance;

    }//for

  }//if PerceptBuffer.size() > 0

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
    
    DebugDrawings::Color inner_color;
    if(colorDiff > 0) {
      inner_color[0] = Math::clamp((sample.likelihood - minValue)/colorDiff,0.0,1.0);
    }

    PEN(inner_color, 10);
    FILLOVAL(sample.translation.x, sample.translation.y, 10, 10);
  }//end for

  PEN(color, 50);
  TEXT_DRAWING(sampleSet.mean.x+10, sampleSet.mean.y+10, idx);
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
