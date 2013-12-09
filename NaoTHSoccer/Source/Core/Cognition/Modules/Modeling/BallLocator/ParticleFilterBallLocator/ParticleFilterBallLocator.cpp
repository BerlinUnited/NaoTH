/**
* @file ParticleFilterBallLocator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ParticleFilterBallLocator
*/

#include "ParticleFilterBallLocator.h"

// Debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

// Tools
#include "Tools/Math/Probabilistics.h"


ParticleFilterBallLocator::ParticleFilterBallLocator()
{
  DEBUG_REQUEST_REGISTER("ParticleFilterBallLocator:draw_ball_on_field", "Draw the ball model on the field. Orange: Ball was seen.", false);
  DEBUG_REQUEST_REGISTER("ParticleFilterBallLocator:draw_samples", "Draw the sample set. Orange: Sample is moving.", false);
  DEBUG_REQUEST_REGISTER("ParticleFilterBallLocator:drawMotionUpdateBySpeed", "", false);
}

void ParticleFilterBallLocator::execute()
{
  getBallModel().reset();

  
  // update by motion model
  motionUpdate(theSampleSet, true);
  

  if(getBallPercept().ballWasSeen)
  {
    if(theSampleSet.size() > 0)
    {
      // 
      updateByBallPercept(theSampleSet);

      // 
      resampleGT07(theSampleSet, false);
    }//end if

    // replace a random particle
    if(perceptBuffer.isFull())
    {
      if(theSampleSet.size() < 20)
      {
        theSampleSet.push_back(generateNewSample());
      }
      else
      {
        int k = Math::random(static_cast<int> (theSampleSet.size()));
        theSampleSet[k] = generateNewSample();
      }
    }
  }//end if


  // calculate the model
  if(!theSampleSet.empty())
  {
    Vector2<double> mean;
    Vector2<double> meanSpeed;
    double numberOfMovingSamples = 0;
    for (unsigned int i = 0; i < theSampleSet.size(); i++)
    { 
      mean += theSampleSet[i].position;

      if(theSampleSet[i].moving)
      {
        meanSpeed += theSampleSet[i].speed;
        numberOfMovingSamples++;
      }      

    }//end for

    double meanSpeedAbs = meanSpeed.abs();
    MODIFY("ParticleFilterBallLocator:meanSpeadAbs", meanSpeedAbs);
    MODIFY("ParticleFilterBallLocator:numberOfMovingSamples", numberOfMovingSamples);


    mean /= static_cast<double> (theSampleSet.size());
    if(numberOfMovingSamples > 1)
      meanSpeed /= numberOfMovingSamples;



    // set the ball model
    if(getBallPercept().ballWasSeen)
      getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

    getBallModel().position = mean;
    //TODO change 13.03
    //getBallModel().speed = Vector2<double>();
    getBallModel().speed = meanSpeed*100;
    getBallModel().valid = true;

    updatePreviewModel();
  }//end if
  

  DEBUG_REQUEST("ParticleFilterBallLocator:draw_ball_on_field", drawBallModel(getBallModel()); );
  DEBUG_REQUEST("ParticleFilterBallLocator:draw_samples", drawSamples(theSampleSet); );


  // remember some data
  lastFrameInfo = getFrameInfo();
  lastRobotOdometry = getOdometryData();

  if (getBallPercept().ballWasSeen)
    perceptBuffer.add(getBallPercept());
}//end execute



void ParticleFilterBallLocator::updateByBallPercept(SampleSet& sampleSet)
{
  //unsigned int maxNumberOfParticles = 20;

  const double seenDistance = getBallPercept().bearingBasedOffsetOnField.abs();
  const double seenAngle = getBallPercept().bearingBasedOffsetOnField.angle();
  const double cameraHeight  = getCameraMatrix().translation.z;

  for (unsigned int i = 0; i < sampleSet.size(); i++)
  { 
    Sample& sample(sampleSet[i]);

    const double expectedDistance = sample.position.abs();
    const double expectedAngle = sample.position.angle();

    sample.likelihood *= computeDistanceWeighting(seenDistance, expectedDistance, cameraHeight, parameters.sigmaDistance, 1.0);
    sample.likelihood *= computeAngleWeighting(seenAngle, expectedAngle, parameters.sigmaAngle, 1.0);
  }//end for
}//end updateByBallPercept



ParticleFilterBallLocator::Sample ParticleFilterBallLocator::generateNewSample()
{
  Sample newSample;
  
  if(perceptBuffer.size() > 1)
  {
    int a = Math::random((int)(perceptBuffer.size()));
    int b = Math::random((int)(perceptBuffer.size()));
    // a and b must be different, of course:
    if(a == b)
    {
      if(a != 0)
        a--;
      else
        a++;
    }

    const double timeDelta = perceptBuffer[b].frameInfoWhenBallWasSeen.getTimeInSeconds() - perceptBuffer[a].frameInfoWhenBallWasSeen.getTimeInSeconds();
    // a must be older than b:
    if(timeDelta < 0)
    {
      std::swap(a,b);
    }

    newSample.position = perceptBuffer[b].bearingBasedOffsetOnField;


    Vector2<double> speed = (perceptBuffer[b].bearingBasedOffsetOnField - perceptBuffer[a].bearingBasedOffsetOnField)*
            fabs(timeDelta);

    //newSample.moving = Math::random() > 0.5;

    if(speed.abs() > parameters.speedWehnParticleIsMoving)
    {
      newSample.moving = true;
      newSample.speed  = speed;
    } else {
      newSample.moving = false;
    }
  }//end if

  return newSample;
}//end generateNewSample



void ParticleFilterBallLocator::motionUpdate(SampleSet& sampleSet, bool noise)
{
  //double motionNoiseDistance = 10;
  const Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  // time elapsed sinse last execution
  const double timeDelta = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
  ASSERT(timeDelta > 0);


  // update the buffer by odometry
  for (int i = 0; i < perceptBuffer.size(); i++) 
  {
    BallPercept& ball = perceptBuffer[i];
    ball.bearingBasedOffsetOnField = odometryDelta*ball.bearingBasedOffsetOnField;
  }//end for
  

  for (unsigned int i = 0; i < sampleSet.size(); i++)
  { 
    Sample& sample(sampleSet[i]);

    if(sample.moving)
    {


      Vector2<double> position = sample.position;

      // simple motion model
      sample.position += sample.speed*timeDelta; //TODO: add some noise

      Vector2<double> positionWithSpeed = sample.position+sample.speed;
      DEBUG_REQUEST("ParticleFilterBallLocator:drawMotionUpdateBySpeed", drawMotionUpdateBySpeed(position, positionWithSpeed););

      // apply odometry to the speed of the ball
      sample.speed = sample.speed.rotate(odometryDelta.rotation);

      // add some noise to the speed
      if(noise)
      {
        const double velocityVariance = sample.speed.abs()*parameters.velocityNoiseFactor;
        sample.speed.x += Math::sampleTriangularDistribution(velocityVariance);
        sample.speed.y += Math::sampleTriangularDistribution(velocityVariance);
      }

      // add some linear friction
      sample.speed *= parameters.frictionCoefficiant*timeDelta;
    }//end if
    
    // apply odometry to the position
    sample.position = odometryDelta * sample.position;


    if(noise)
    {
      const double odometryVariance = odometryDelta.translation.abs()*parameters.odometryNoiseFactor;
      
      sample.position.x += Math::sampleTriangularDistribution(odometryVariance);
      sample.position.y += Math::sampleTriangularDistribution(odometryVariance);
    }
  }//end for
}//end updateByOdometry


void ParticleFilterBallLocator::resampleGT07(SampleSet& sampleSet, bool noise)
{
  // parameters
  double processNoiseDistance = 5;
  MODIFY("ParticleFilterBallLocator:processNoiseDistance",processNoiseDistance);
  double resamplingThreshhold = 0.0;
  MODIFY("ParticleFilterBallLocator:resamplingThreshhold",resamplingThreshhold);

  // 
  double totalWeighting = 0;
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for
  const double averageWeighting = totalWeighting / (double)sampleSet.size();

  if (averageWeighting > 0) //wrong parameters
    PLOT("ParticleFilterBallLocator:averageWeighting", averageWeighting);

  // copy the samples 
  // TODO: use memcopy?
  std::vector<Sample> oldSampleSet = sampleSet;
  
  totalWeighting += resamplingThreshhold*static_cast<double> (oldSampleSet.size());
  for(unsigned int i = 0; i < oldSampleSet.size(); i++)
  {
    oldSampleSet[i].likelihood += resamplingThreshhold;
    oldSampleSet[i].likelihood /= totalWeighting; // normalize
  }//end for


  // resample 10% of particles
  //int numberOfPartiklesToResample = (int)(((double)sampleSet.size())*0.1+0.5);
 
  double sum = -Math::random();
  unsigned int count = 0;

  //unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  for(unsigned int m = 0; m < sampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * static_cast<double> (oldSampleSet.size());

    // select the particle to copy
    while(count < sum && count < oldSampleSet.size())
    {
      if (n >= oldSampleSet.size()) break;
      
      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      n++;
      count++;
    }//end while
  }//end for
}//end resampleGT07


inline double ParticleFilterBallLocator::computeAngleWeighting(
                                double measuredAngle, 
                                double expectedAngle,
                                double standardDeviation, 
                                double bestPossibleWeighting) const
{ 
  // TODO: normalize?
  double angleDif = Math::normalize(expectedAngle - measuredAngle);
  return Math::gaussianProbability(angleDif, standardDeviation) / bestPossibleWeighting;
}//end computeAngleWeighting


inline double ParticleFilterBallLocator::computeDistanceWeighting(
                                double measuredDistance, 
                                double expectedDistance,
                                double cameraZ,
                                double standardDeviation, 
                                double bestPossibleWeighting) const
{
  const double measuredDistanceAsAngle = atan2(measuredDistance, cameraZ);
  const double expectedDistanceAsAngle = atan2(expectedDistance, cameraZ);

  // TODO: normalize?
  double angleDif = Math::normalize(expectedDistanceAsAngle - measuredDistanceAsAngle);
  return Math::gaussianProbability(angleDif, standardDeviation) / bestPossibleWeighting;
}//end computeDistanceWeighting


void ParticleFilterBallLocator::updatePreviewModel()
{
  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;
  
  Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
  Pose2D rFootPose(rFoot.rotation.getZAngle(), rFoot.translation.x, rFoot.translation.y);

  Vector2<double> ballLeftFoot  = lFootPose/getBallModel().position;
  Vector2<double> ballRightFoot = rFootPose/getBallModel().position;

  getBallModel().positionPreview = getMotionStatus().plannedMotion.hip / getBallModel().position;
  getBallModel().positionPreviewInLFoot = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;
  getBallModel().positionPreviewInRFoot = getMotionStatus().plannedMotion.rFoot / ballRightFoot;
}//end updatePreviewModel


void ParticleFilterBallLocator::drawSamples(const SampleSet& sampleSet) const
{
  FIELD_DRAWING_CONTEXT;

  // plot samples
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    if(sampleSet[i].moving)
      PEN("FF9900", 5); //orange
    else
      PEN("0099FF", 5); //blue

    const Vector2<double>& pos = sampleSet[i].position;
    FILLOVAL( pos.x, pos.y, 10, 10);

    if(sampleSet[i].moving)
    {
      const Vector2<double>& speed = sampleSet[i].speed;
      ARROW( pos.x, pos.y, pos.x + speed.x, pos.y + speed.y);
    }//end if
  }//end for
}//end drawSamples


void ParticleFilterBallLocator::drawBallModel(const BallModel& ballModel) const
{

  const double timeDelta = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
  ASSERT(timeDelta > 0);

  FIELD_DRAWING_CONTEXT;
  
  if(ballModel.valid)
  {
    if(getBallPercept().ballWasSeen)
      PEN("FF9900", 20); //orange
    else
      PEN("0099FF", 20); //blue
  }
  else
    PEN("999999", 20); //grey

  CIRCLE( ballModel.position.x, ballModel.position.y, getFieldInfo().ballRadius-10);
  ARROW( ballModel.position.x, ballModel.position.y,
         ballModel.position.x+ballModel.speed.x*timeDelta,
         ballModel.position.y+ballModel.speed.y*timeDelta);

  // draw the distribution
  /*
  PEN("00FFFF", 20);
  OVAL(getBallModel().position.x, getBallModel().position.y, Px[0][0], Py[0][0]);
  PEN("FF00FF", 20);
  OVAL(getBallModel().position.x, getBallModel().position.y, Px[1][1], Py[1][1]);
  */

  PEN("999999", 10); //grey
  for(int i=1; i <= BALLMODEL_MAX_FUTURE_SECONDS; i++)
  {
    CIRCLE(ballModel.futurePosition[i].x, ballModel.futurePosition[i].y, getFieldInfo().ballRadius-5);
  }
}//end drawBallModel

void ParticleFilterBallLocator::drawMotionUpdateBySpeed(const Vector2<double> position, const Vector2<double> positionWithSpeed) const
{

    FIELD_DRAWING_CONTEXT;

    PEN("999999", 8); //grey

    CIRCLE( position.x, position.y, getFieldInfo().ballRadius-10);
    LINE( position.x, position.y,
           positionWithSpeed.x,
           positionWithSpeed.y);
    CIRCLE( positionWithSpeed.x, positionWithSpeed.y, getFieldInfo().ballRadius-10);

}//end drawMotionUpdateBySpeed
