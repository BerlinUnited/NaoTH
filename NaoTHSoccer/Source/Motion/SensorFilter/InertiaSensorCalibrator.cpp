/**
 * @file InertiaSensorCalibrator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @author Inspired by the BH-2011
 */

#include "InertiaSensorCalibrator.h"

#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"

using namespace naoth;

InertiaSensorCalibrator::InertiaSensorCalibrator()
{
  DEBUG_REQUEST_REGISTER("InertiaSensorCalibrator:force_calibrate", "", false);
  reset();
}

// check all request joints' speed, return true if all joints are almost not moving
bool InertiaSensorCalibrator::intentionallyMoving()
{
  const double* jointSpeed = getMotorJointData().dp;
  const double* stiffness = getMotorJointData().stiffness;
  const double min_speed = Math::fromDegrees(1); // degree per second
  const double min_stiffness = 0.05;
  for( int i=0; i<JointData::numOfJoint; i++)
  {
    if ( stiffness[i] > min_stiffness && fabs(jointSpeed[i]) > min_speed )
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
  stableStartTime = 0;
}//end reset


void InertiaSensorCalibrator::execute()
{
  // inertial params
  Vector2d inertialBiasProcessNoise = Vector2d(0.05, 0.05);
  MODIFY("InertiaSensorCalibrator:inertialBiasProcessNoise.x", inertialBiasProcessNoise.x);
  MODIFY("InertiaSensorCalibrator:inertialBiasProcessNoise.y", inertialBiasProcessNoise.y);

  Vector2d inertialBiasMeasurementNoise = Vector2d(0.01, 0.01);
  MODIFY("InertiaSensorCalibrator:inertialBiasMeasurementNoise.x", inertialBiasMeasurementNoise.x);
  MODIFY("InertiaSensorCalibrator:inertialBiasMeasurementNoise.y", inertialBiasMeasurementNoise.y);

  // gyro params
  static const Vector3d gyroBiasProcessNoise(0.05f, 0.05f, 0.05f);
  static const Vector3d gyroBiasMeasurementNoise(0.01f, 0.01f, 0.01f); // stand
  //static const Vector2d gyroBiasWalkMeasurementNoise(0.1f, 0.1f);


  // acc params
  static const Vector3d accBiasProcessNoise = Vector3d(0.01f, 0.01f, 0.01f);
  static const Vector3d accBiasStandMeasurementNoise = Vector3d(0.1f, 0.1f, 0.1f);
  //static const Vector3d accBiasWalkMeasurementNoise = Vector3d(1.f, 1.f, 1.f);
  const Vector3d& accBiasMeasurementNoise = accBiasStandMeasurementNoise;

  // collecting time
  unsigned int timeFrame = 1500;
  double tmp = (double)timeFrame;
  MODIFY("InertiaSensorCalibrator:timeFrame", tmp);
  timeFrame = (unsigned int)tmp;


  // frame time check: ansure the monotonical time increase
  // TODO: do we need it?
  if(getFrameInfo().getTime() <= lastTime)
  {
    if(getFrameInfo().getTime() == lastTime) {
      return; // done!
    }
    reset();
  }

  // it's prediction time!
  if(lastTime && calibrated)
  {
    const double timeDiff = static_cast<double>(getFrameInfo().getTimeSince(lastTime)) * 0.001; // in seconds
    inertialXBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.x * timeDiff));
    inertialYBias.predict(0.0, Math::sqr(inertialBiasProcessNoise.y * timeDiff));
    accXBias.predict(0.0, Math::sqr(accBiasProcessNoise.x * timeDiff));
    accYBias.predict(0.0, Math::sqr(accBiasProcessNoise.y * timeDiff));
    accZBias.predict(0.0, Math::sqr(accBiasProcessNoise.z * timeDiff));
    gyroXBias.predict(0.0, Math::sqr(gyroBiasProcessNoise.x * timeDiff));
    gyroYBias.predict(0.0, Math::sqr(gyroBiasProcessNoise.y * timeDiff));
    gyroZBias.predict(0.0, Math::sqr(gyroBiasProcessNoise.z * timeDiff));
  }


  // detect unstable stuff...
  bool unstable = 
        getMotionStatus().currentMotion != motion::stand
    || !getGroundContactModel().leftGroundContact
    || !getGroundContactModel().rightGroundContact;
    //|| intentionallyMoving()
    //|| !( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE_RIGHT) );


  DEBUG_REQUEST("InertiaSensorCalibrator:force_calibrate", unstable=false; );
  PLOT("InertiaSensorCalibrator:unstable", unstable);

  // update cleanCollectionStartTime
  if(unstable) {
    stableStartTime = 0;
  } else if(!stableStartTime) {
    stableStartTime = getFrameInfo().getTime();
  }

  // restart sensor value collecting?
  if(unstable || (getFrameInfo().getTimeSince(collectionStartTime) > 1000))
  {
    // add collection within the time frame to the collection buffer
    if(stableStartTime && getFrameInfo().getTimeSince(stableStartTime) > (int)timeFrame &&
       inertialValues.size())
    {
      //ASSERT(collections.getNumberOfEntries() < collections.getMaxEntries());
      collections.add(Collection(inertialValues.getAverage(),
                                 accValues.getAverage(),
                                 gyroValues.getAverage(),
                                 // mean time of the collection
                                 collectionStartTime + getFrameInfo().getTimeSince(collectionStartTime) / 2));
    }

    // restart collecting
    accValues.clear();
    gyroValues.clear();
    inertialValues.clear();
    collectionStartTime = 0;

    // look if there are any useful buffered collections
    for(int i = collections.size() - 1; i >= 0; --i)
    {
      const Collection& collection(collections.getEntry(i));
      if(getFrameInfo().getTimeSince(collection.timeStamp) < (int)timeFrame) {
        break;
      }

      if(stableStartTime && stableStartTime < collection.timeStamp)
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
          gyroZBias.init(collection.gyroAvg.z, Math::sqr(gyroBiasMeasurementNoise.z));
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
          gyroZBias.update(collection.gyroAvg.z, Math::sqr(gyroBiasMeasurementNoise.z));
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
      Kinematics::ForwardKinematics::calcChestFeetRotation(getKinematicChainSensor());

    // calculate expected acceleration sensor reading
    Vector2d inertialExpected(calculatedRotation.getXAngle(), calculatedRotation.getYAngle());
    Vector3d accExpected(calculatedRotation[0].z, calculatedRotation[1].z, calculatedRotation[2].z);
    accExpected *= -Math::g;

    // add sensor reading to the collection
    inertialValues.add(inertialExpected - getInertialSensorData().data);
    accValues.add(getAccelerometerData().data - accExpected);
    gyroValues.add( -getGyrometerData().data );

    if(!collectionStartTime) {
      collectionStartTime = getFrameInfo().getTime();
    }
  }//end if !unstable



  lastTime = getFrameInfo().getTime();


  // provide calibrated inertia readings
  if(!calibrated)
  {
    getCalibrationData().accSensorOffset = Vector3d();
    getCalibrationData().gyroSensorOffset = Vector3d();
    getCalibrationData().inertialSensorOffset = Vector2d();
  }
  else
  {
    getCalibrationData().inertialSensorOffset.x = inertialXBias.value;
    getCalibrationData().inertialSensorOffset.y = inertialYBias.value;

    getCalibrationData().accSensorOffset.x = accXBias.value;
    getCalibrationData().accSensorOffset.y = accYBias.value;
    getCalibrationData().accSensorOffset.z = accZBias.value;

    getCalibrationData().gyroSensorOffset.x = gyroXBias.value;
    getCalibrationData().gyroSensorOffset.y = gyroYBias.value;
    getCalibrationData().gyroSensorOffset.z = gyroZBias.value;
  }

  getCalibrationData().calibrated = calibrated;

  PLOT("InertiaSensorCalibrator:inertialOffset.x", getCalibrationData().inertialSensorOffset.x);
  PLOT("InertiaSensorCalibrator:inertialOffset.y", getCalibrationData().inertialSensorOffset.y);

  PLOT("InertiaSensorCalibrator:gyroOffset.x", getCalibrationData().gyroSensorOffset.x);
  PLOT("InertiaSensorCalibrator:gyroOffset.y", getCalibrationData().gyroSensorOffset.y);
  PLOT("InertiaSensorCalibrator:gyroOffset.z", getCalibrationData().gyroSensorOffset.z);

  PLOT("InertiaSensorCalibrator:accOffset.x", getCalibrationData().accSensorOffset.x);
  PLOT("InertiaSensorCalibrator:accOffset.y", getCalibrationData().accSensorOffset.y);
  PLOT("InertiaSensorCalibrator:accOffset.z", getCalibrationData().accSensorOffset.z);

}//end execute
