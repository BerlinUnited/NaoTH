/**
 * @file InertiaSensorCalibrator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "InertiaSensorCalibrator.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"

using namespace naoth;

InertiaSensorCalibrator::InertiaSensorCalibrator(
  const MotionBlackBoard& bb, 
  CalibrationData& theCalibrationData)
  :
  theBlackBoard(bb),
  theCalibrationData(theCalibrationData)
{
}


// check all request joints' speed, return true if all joints are almost not moving
bool InertiaSensorCalibrator::intentionallyMoving()
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
}//end intentionallyMoving


void InertiaSensorCalibrator::reset()
{
  lastTime = 0;
  calibrated = false;
  
  collectionStartTime = 0;
  cleanCollectionStartTime = 0;
}//end reset


void InertiaSensorCalibrator::update()
{
  // inertial params
  Vector2d inertialBiasProcessNoise = Vector2d(0.05, 0.05);
  MODIFY("InertiaSensorCalibrator:inertialBiasProcessNoise.x", inertialBiasProcessNoise.x);
  MODIFY("InertiaSensorCalibrator:inertialBiasProcessNoise.y", inertialBiasProcessNoise.y);

  Vector2d inertialBiasMeasurementNoise = Vector2d(0.01, 0.01);
  MODIFY("InertiaSensorCalibrator:inertialBiasMeasurementNoise.x", inertialBiasMeasurementNoise.x);
  MODIFY("InertiaSensorCalibrator:inertialBiasMeasurementNoise.y", inertialBiasMeasurementNoise.y);

  // gyro params
  Vector2d gyroBiasProcessNoise(0.05f, 0.05f);
  Vector2d gyroBiasMeasurementNoise(0.01f, 0.01f); // stand
  //Vector2d gyroBiasWalkMeasurementNoise(0.1f, 0.1f);


  // acc params
  Vector3d accBiasProcessNoise = Vector3d(0.01f, 0.01f, 0.01f);
  Vector3d accBiasStandMeasurementNoise = Vector3d(0.1f, 0.1f, 0.1f);
  //Vector3d accBiasWalkMeasurementNoise = Vector3d(1.f, 1.f, 1.f);
  const Vector3d& accBiasMeasurementNoise = accBiasStandMeasurementNoise;

  // collecting time
  unsigned int timeFrame = 1500;
  double tmp = (double)timeFrame;
  MODIFY("InertiaSensorCalibrator:timeFrame", tmp);
  timeFrame = (unsigned int)tmp;




  // frame time check
  if(theBlackBoard.theFrameInfo.getTime() <= lastTime)
  {
    if(theBlackBoard.theFrameInfo.getTime() == lastTime)
      return; // done!
    reset();
  }

  // it's prediction time!
  if(lastTime && calibrated)
  {
    const double timeDiff = double(theBlackBoard.theFrameInfo.getTimeSince(lastTime)) * 0.001; // in seconds
    inertialXBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.x * timeDiff));
    inertialYBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.y * timeDiff));
    accXBias.predict(0.0, Math::sqr(accBiasProcessNoise.x * timeDiff));
    accYBias.predict(0.0, Math::sqr(accBiasProcessNoise.y * timeDiff));
    accZBias.predict(0.0, Math::sqr(accBiasProcessNoise.z * timeDiff));
    gyroXBias.predict(0.0, Math::sqr(gyroBiasProcessNoise.x * timeDiff));
    gyroYBias.predict(0.0, Math::sqr(gyroBiasProcessNoise.y * timeDiff));
  }


  // detect unstable stuff...
  bool unstable = theBlackBoard.theMotionStatus.currentMotion != motion::stand
    || !theBlackBoard.theGroundContactModel.leftGroundContactAverage
    || !theBlackBoard.theGroundContactModel.rightGroundContactAverage;

    //|| intentionallyMoving()
    //|| !( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE_RIGHT) );

  PLOT("InertiaSensorCalibrator:unstable", unstable);

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
                                 accValues.getAverage(),
                                 gyroValues.getAverage(),
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
          accXBias.init(collection.accAvg.x, Math::sqr(accBiasMeasurementNoise.x));
          accYBias.init(collection.accAvg.y, Math::sqr(accBiasMeasurementNoise.y));
          accZBias.init(collection.accAvg.z, Math::sqr(accBiasMeasurementNoise.z));
          gyroXBias.init(collection.gyroAvg.x, Math::sqr(gyroBiasMeasurementNoise.x));
          gyroYBias.init(collection.gyroAvg.y, Math::sqr(gyroBiasMeasurementNoise.y));
        }
        else
        {
          inertialXBias.update(collection.inertialAvg.x, Math::sqr(inertialBiasMeasurementNoise.x));
          inertialYBias.update(collection.inertialAvg.y, Math::sqr(inertialBiasMeasurementNoise.y));
          accXBias.update(collection.accAvg.x, Math::sqr(accBiasMeasurementNoise.x));
          accYBias.update(collection.accAvg.y, Math::sqr(accBiasMeasurementNoise.y));
          accZBias.update(collection.accAvg.z, Math::sqr(accBiasMeasurementNoise.z));
          gyroXBias.update(collection.gyroAvg.x, Math::sqr(gyroBiasMeasurementNoise.x));
          gyroYBias.update(collection.gyroAvg.y, Math::sqr(gyroBiasMeasurementNoise.y));
        }
      }
      collections.removeFirst();
    }
  }//end if


  PLOT("InertiaSensorCalibrator:calibrated", calibrated);

  // collecting....
  if(!unstable)
  {
    // Body rotation, which was calculated using kinematics.
    RotationMatrix calculatedRotation = 
      Kinematics::ForwardKinematics::calcChestFeetRotation(theBlackBoard.theKinematicChain);

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());
    Vector3d accGravOnly(calculatedRotation[0].z, calculatedRotation[1].z, calculatedRotation[2].z);
    accGravOnly *= -Math::g;

    // add sensor reading to the collection
    inertialValues.add(inertialExpected - theBlackBoard.theInertialSensorData.data);
    accValues.add(theBlackBoard.theAccelerometerData.data - accGravOnly);
    gyroValues.add( -theBlackBoard.theGyrometerData.data );

    if(!collectionStartTime)
      collectionStartTime = theBlackBoard.theFrameInfo.getTime();
  }//end if !unstable



  lastTime = theBlackBoard.theFrameInfo.getTime();


  // provide calibrated inertia readings
  if(!calibrated)
  {
    theCalibrationData.accSensorOffset = Vector3d();
    theCalibrationData.gyroSensorOffset = Vector2d();
    theCalibrationData.inertialSensorOffset = Vector2d();
  }
  else
  {
    theCalibrationData.inertialSensorOffset.x = inertialXBias.value;
    theCalibrationData.inertialSensorOffset.y = inertialYBias.value;

    theCalibrationData.accSensorOffset.x = accXBias.value;
    theCalibrationData.accSensorOffset.y = accYBias.value;
    theCalibrationData.accSensorOffset.z = accZBias.value;

    theCalibrationData.gyroSensorOffset.x = gyroXBias.value;
    theCalibrationData.gyroSensorOffset.y = gyroYBias.value;
  }

  theCalibrationData.calibrated = calibrated;

  PLOT("InertiaSensorCalibrator:inertialOffset.x", theCalibrationData.inertialSensorOffset.x);
  PLOT("InertiaSensorCalibrator:inertialOffset.y", theCalibrationData.inertialSensorOffset.y);

  PLOT("InertiaSensorCalibrator:gyroOffset.x", theCalibrationData.gyroSensorOffset.x);
  PLOT("InertiaSensorCalibrator:gyroOffset.y", theCalibrationData.gyroSensorOffset.y);

  PLOT("InertiaSensorCalibrator:accOffset.x", theCalibrationData.accSensorOffset.x);
  PLOT("InertiaSensorCalibrator:accOffset.y", theCalibrationData.accSensorOffset.y);
  PLOT("InertiaSensorCalibrator:accOffset.z", theCalibrationData.accSensorOffset.z);

}//end update
