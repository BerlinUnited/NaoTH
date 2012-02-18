/**
 * @file InertialFilter.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "InertialFilter.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"

using namespace naoth;

InertialFilter::InertialFilter(const MotionBlackBoard& bb, Vector2d& offset):
  theBlackBoard(bb),
  sensorData(bb.theInertialSensorData),
  theOffset(offset),
  calibrateNum(0),
  max_offet(Math::fromDegrees(6))
{
}

InertialPercept InertialFilter::filter()
{
  calibrate();

  InertialPercept result;
  result.data = sensorData.data + theOffset;

  PLOT("InertialFilter:offset.x", theOffset.x);
  PLOT("InertialFilter:offset.y", theOffset.y);

  return result;
}

void InertialFilter::calibrate()
{
  update_bhuman_like();

  return;


  if ( theBlackBoard.theMotionStatus.currentMotion == motion::stand
      && !intentionallyMoving() )
  {
    // stand and not moving
    if ( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE_RIGHT) )
    {
      // both feet on the ground
      Pose3D foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::LFoot].M;
      const Pose3D& body = theBlackBoard.theKinematicChain.theLinks[KinematicChain::Torso].M;
      Pose3D bodyInFoot = foot.invert() * body;
      Vector2d expectedAngle;
      expectedAngle.x = bodyInFoot.rotation.getXAngle();
      expectedAngle.y = bodyInFoot.rotation.getYAngle();
      Vector2d diff = sensorData.data - expectedAngle;
      if ( abs(diff.x) < max_offet && abs(diff.y) < max_offet )
      {
        // do calibrate
        theNewOffset = (theNewOffset*calibrateNum - diff) / (calibrateNum+1);
        calibrateNum++;
        if ( calibrateNum % 50 == 0 )
        {
          // every 50 datas
          theOffset = theNewOffset;
        }
        return;
      }
    }
  }

  calibrateNum = 0;
}

// check all request joints' speed, return true if all joints are almost not moving
bool InertialFilter::intentionallyMoving()
{
  const double* jointSpeed = theBlackBoard.theMotorJointData.dp;
  const double* stiffness = theBlackBoard.theMotorJointData.stiffness;
  const double min_speed = Math::fromDegrees(1); // degree per second
  const double min_stiffness = 0.05;
  for( int i=0; i<JointData::numOfJoint; i++)
  {
    if ( stiffness[i] > min_stiffness && abs(jointSpeed[i]) > min_speed )
    {
      return true;
    }
  }

  return false;
}

void InertialFilter::reset_bhuman_like()
{
  lastTime = 0;
  //lastMotion = MotionRequest::specialAction;
  calibrated = false;
  
  collectionStartTime = 0;
  cleanCollectionStartTime = 0;
  //safeGyro = Vector2<>();
  //safeAcc = Vector3<>(0.f, 0.f, -9.80665f);
  //inertiaSensorDrops = 1000;
}//end reset_bhuman_like

void InertialFilter::update_bhuman_like()
{
  // todo
  Vector2d inertialBiasProcessNoise = Vector2d(0.05, 0.05);
  MODIFY("InertialFilter:inertialBiasProcessNoise.x", inertialBiasProcessNoise.x);
  MODIFY("InertialFilter:inertialBiasProcessNoise.y", inertialBiasProcessNoise.y);

  Vector2d inertialBiasMeasurementNoise = Vector2d(0.01, 0.01);
  MODIFY("InertialFilter:inertialBiasMeasurementNoise.x", inertialBiasMeasurementNoise.x);
  MODIFY("InertialFilter:inertialBiasMeasurementNoise.y", inertialBiasMeasurementNoise.y);

  unsigned int timeFrame = 1500;
  double tmp = (double)timeFrame;
  MODIFY("InertialFilter:timeFrame", tmp);
  timeFrame = (unsigned int)tmp;

  // frame time check
  if(theBlackBoard.theFrameInfo.getTime() <= lastTime)
  {
    if(theBlackBoard.theFrameInfo.getTime() == lastTime)
      return; // done!
    reset_bhuman_like();
  }

  // it's prediction time!
  if(lastTime && calibrated)
  {
    const double timeDiff = double(theBlackBoard.theFrameInfo.getTimeSince(lastTime)) * 0.001; // in seconds
    inertialXBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.x * timeDiff));
    inertialYBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.y * timeDiff));
  }


  // detect unstable stuff...
  bool unstable = theBlackBoard.theMotionStatus.currentMotion != motion::stand
    || intentionallyMoving()
    || !( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE_RIGHT) );


  // update cleanCollectionStartTime
  if(unstable)
    cleanCollectionStartTime = 0;
  else if(!cleanCollectionStartTime)
    cleanCollectionStartTime = theBlackBoard.theFrameInfo.getTime();





  // restart sensor value collecting?
  if(unstable || (theBlackBoard.theFrameInfo.getTimeSince(collectionStartTime) > 1000))
  {
    // add collection within the time frame to the collection buffer
    if(cleanCollectionStartTime && theBlackBoard.theFrameInfo.getTimeSince(cleanCollectionStartTime) > (int)timeFrame &&
       inertialValues.getNumberOfEntries())
    {
      //ASSERT(collections.getNumberOfEntries() < collections.getMaxEntries());
      collections.add(Collection(inertialValues.getAverage(),
                                 collectionStartTime + (theBlackBoard.theFrameInfo.getTimeSince(collectionStartTime)) / 2));
    }

    // restart collecting
    inertialValues.init();
    collectionStartTime = 0;

    // look if there are any useful buffered collections
    for(int i = collections.getNumberOfEntries() - 1; i >= 0; --i)
    {
      const Collection& collection(collections.getEntry(i));
      if(theBlackBoard.theFrameInfo.getTimeSince(collection.timeStamp) < (int)timeFrame)
        break;
      if(cleanCollectionStartTime && cleanCollectionStartTime < collection.timeStamp)
      {
        // use this collection
        if(!calibrated)
        {
          calibrated = true;
          inertialXBias.init(collection.inertialAvg.x, Math::sqr(inertialBiasMeasurementNoise.x));
          inertialYBias.init(collection.inertialAvg.y, Math::sqr(inertialBiasMeasurementNoise.y));
        }
        else
        {
          inertialXBias.update(collection.inertialAvg.x, Math::sqr(inertialBiasMeasurementNoise.x));
          inertialYBias.update(collection.inertialAvg.y, Math::sqr(inertialBiasMeasurementNoise.y));
        }
      }
      collections.removeFirst();
    }
  }//end if




  // collecting....
  if(!unstable)
  {
    RotationMatrix calculatedRotation = 
      Kinematics::ForwardKinematics::calcChestFeetRotation(theBlackBoard.theKinematicChain);

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());

    // add sensor reading to the collection
    inertialValues.add(inertialExpected - sensorData.data);
    if(!collectionStartTime)
      collectionStartTime = theBlackBoard.theFrameInfo.getTime();
  }//end if !unstable



  // provide calibrated inertia readings
  if(!calibrated)
  {
    theOffset = Vector2d();
  }
  else
  {
    theOffset.x = inertialXBias.value;
    theOffset.y = inertialYBias.value;
  }

  lastTime = theBlackBoard.theFrameInfo.getTime();
}//end update_bhuman_like
