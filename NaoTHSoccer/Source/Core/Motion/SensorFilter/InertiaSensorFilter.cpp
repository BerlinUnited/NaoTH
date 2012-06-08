/**
* @file InertiaSensorFilter.cpp
* Implementation of module InertiaSensorFilter.
* @author Colin Graf (BH-2011)
* @author Heinrich Mellmann (NaoTH-2012)
* adapted from BH-2011
*/

#include "InertiaSensorFilter.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Math/RotationMatrix.h"
#include "Motion/MorphologyProcessor/ForwardKinematics.h"

//#include "Representations/MotionControl/MotionInfo.h"
//#include "Representations/MotionControl/WalkingEngineOutput.h"


InertiaSensorFilter::InertiaSensorFilter(const MotionBlackBoard& theBlackBoard, InertialModel& theInertialModel) 
  : 
  theInertialModel(theInertialModel),
  theBlackBoard(theBlackBoard),
  lastTime(0)
{
  p.processNoise = Vector2<double>(0.004f, 0.004f);
  p.accNoise = Vector3<double>(1.0, 1.0, 1.0);
  p.calculatedAccLimit = Vector2<double>(Math::fromDegrees(20.0), Math::fromDegrees(30.0));

  p.calculateConstants();
}

void InertiaSensorFilter::Parameters::calculateConstants()
{
  processCov.c[0].x = Math::sqr(processNoise.x);
  processCov.c[1].y = Math::sqr(processNoise.y);

  sensorCov[0].x = Math::sqr(accNoise.x);
  sensorCov[1].y = Math::sqr(accNoise.y);
  sensorCov[2].z = Math::sqr(accNoise.z);
}

void InertiaSensorFilter::reset()
{
  x = State<>();
  cov = p.processCov;

  lastTime = theBlackBoard.theFrameInfo.getTime() - (unsigned int)(theBlackBoard.theRobotInfo.getBasicTimeStepInSecond());
}

Vector2<double> InertiaSensorFilter::update()
{
  //MODIFY("module:InertiaSensorFilter:parameters", p);

  // check whether the filter shall be reset
  if(lastTime == 0 || theBlackBoard.theFrameInfo.getTime() == lastTime)
  {
    reset();
    return theInertialModel.orientation;
  }


  // update the filter
  if(theBlackBoard.theCalibrationData.calibrated) // use only calibrated data
  {
    double timeScale = theBlackBoard.theFrameInfo.getTimeSince(lastTime) * 0.001;
    predict(RotationMatrix(Vector3<double>(theBlackBoard.theGyrometerData.data.x * timeScale, theBlackBoard.theGyrometerData.data.y * timeScale, 0)));
  }

  // insert calculated rotation
  safeRawAngle = theBlackBoard.theInertialSensorData.data;

  /*
  if((theMotionInfo.motion == MotionRequest::walk || theMotionInfo.motion == MotionRequest::stand ||
      (theMotionInfo.motion == MotionRequest::specialAction && theMotionInfo.specialActionRequest.specialAction == SpecialActionRequest::sitDownKeeper)) &&
     abs(safeRawAngle.x) < p.calculatedAccLimit.x && abs(safeRawAngle.y) < p.calculatedAccLimit.y)
  */
  double mode = 0;
  MODIFY("InertiaSensorFilter:mode",mode);

  double modePlot = 0;
  if(
    (theBlackBoard.theMotionStatus.currentMotion == motion::stand || theBlackBoard.theMotionStatus.currentMotion == motion::walk)
    //&& ( theBlackBoard.theSupportPolygon.mode & (SupportPolygon::DOUBLE | SupportPolygon::DOUBLE_LEFT | SupportPolygon::DOUBLE_RIGHT) )
    && (theBlackBoard.theGroundContactModel.leftGroundContact || theBlackBoard.theGroundContactModel.rightGroundContact)
    && abs(safeRawAngle.x) < p.calculatedAccLimit.x && abs(safeRawAngle.y) < p.calculatedAccLimit.y
    )
  {
    // use a modeled acceleration, if the feet are on the ground

    RotationMatrix calculatedRotation = 
      Kinematics::ForwardKinematics::calcChestFeetRotation(theBlackBoard.theKinematicChain);

    Vector3<double> accGravOnly(calculatedRotation[0].z, calculatedRotation[1].z, calculatedRotation[2].z);
    accGravOnly *= -Math::g;


    PLOT("InertiaSensorFilter:accGravOnly:x", accGravOnly.x);
    PLOT("InertiaSensorFilter:accGravOnly:y", accGravOnly.y);
    PLOT("InertiaSensorFilter:accGravOnly:z", accGravOnly.z);
    modePlot = 0;

    readingUpdate(accGravOnly);
  }
  else // insert acceleration sensor values
  {
    PLOT("theCalibrationData.calibrated", theBlackBoard.theCalibrationData.calibrated);

    if(theBlackBoard.theCalibrationData.calibrated) // use only calibrated data
      readingUpdate(theBlackBoard.theAccelerometerData.data);
    else // use the aldebaran sensor as a fallback
      x.rotation = RotationMatrix(Vector3<double>(safeRawAngle.x, safeRawAngle.y, 0.0));

    modePlot = 1;
  }

  PLOT("InertiaSensorFilter:mode", modePlot);

  // fill the representation
  theInertialModel.orientation = Vector2<double>(
                                  atan2(x.rotation[1].z, x.rotation[2].z),
                                  atan2(-x.rotation[0].z, x.rotation[2].z));
  
  // this removes any kind of z-rotation from internal rotation
  if(theInertialModel.orientation.abs2() < 0.04 * 0.04)
    x.rotation = RotationMatrix(Vector3<double>(theInertialModel.orientation.x, theInertialModel.orientation.y, 0.0));


  // store some data for the next iteration
  lastTime = theBlackBoard.theFrameInfo.getTime();

  // plots
  PLOT("InertiaSensorFilter:orientationX", theInertialModel.orientation.x);
  PLOT("InertiaSensorFilter:orientationY", theInertialModel.orientation.y);

  return theInertialModel.orientation;
}//end update


void InertiaSensorFilter::predict(const RotationMatrix& rotationOffset)
{
  generateSigmaPoints();

  // update sigma points
  for(int i = 0; i < 5; ++i)
    sigmaPoints[i].rotation *= rotationOffset;

  // get new mean and cov
  meanOfSigmaPoints();
  covOfSigmaPoints();

  // add process noise
  cov.c[0].x += p.processCov.c[0].x;
  cov.c[1].y += p.processCov.c[1].y;
}

void InertiaSensorFilter::readingModel(const State<double>& sigmaPoint, Vector3<double>& reading)
{
  reading = Vector3<double>(sigmaPoint.rotation[0].z, sigmaPoint.rotation[1].z, sigmaPoint.rotation[2].z);
  reading *= -Math::g;
}

void InertiaSensorFilter::readingUpdate(const Vector3<double>& reading)
{
  generateSigmaPoints();

  for(int i = 0; i < 5; ++i)
    readingModel(sigmaPoints[i], sigmaReadings[i]);

  meanOfSigmaReadings();

  PLOT("InertiaSensorFilter:expectedAccX", readingMean.x);
  PLOT("InertiaSensorFilter:accX", reading.x);
  PLOT("InertiaSensorFilter:expectedAccY", readingMean.y);
  PLOT("InertiaSensorFilter:accY", reading.y);
  PLOT("InertiaSensorFilter:expectedAccZ", readingMean.z);
  PLOT("InertiaSensorFilter:accZ", reading.z);

  covOfSigmaReadingsAndSigmaPoints();
  covOfSigmaReadings();

  const Matrix2x3<double>& kalmanGain = readingsSigmaPointsCov.transpose() * (readingsCov + p.sensorCov).invert();
  const Vector2<double>& innovation = kalmanGain * (reading - readingMean);
  x += innovation;
  cov -= kalmanGain * readingsSigmaPointsCov;
}

void InertiaSensorFilter::generateSigmaPoints()
{
  cholOfCov();
  sigmaPoints[0] = x;
  sigmaPoints[1] = x + l.c[0];
  sigmaPoints[2] = x + l.c[1];
  sigmaPoints[3] = x + (-l.c[0]);
  sigmaPoints[4] = x + (-l.c[1]);
}

void InertiaSensorFilter::meanOfSigmaPoints()
{
  x = sigmaPoints[0];
  //for(int i = 0; i < 5; ++i) // ~= 0 .. inf
  for(int i = 0; i < 1; ++i)
  {
    Vector2<double> chunk((sigmaPoints[0] - x) +
                    ((sigmaPoints[1] - x) + (sigmaPoints[2] - x)) +
                    ((sigmaPoints[3] - x) + (sigmaPoints[4] - x)));
    chunk /= 5.f;
    x += chunk;
  }
}

void InertiaSensorFilter::covOfSigmaPoints()
{
  cov = tensor(sigmaPoints[0] - x) +
        (tensor(sigmaPoints[1] - x) + tensor(sigmaPoints[2] - x)) +
        (tensor(sigmaPoints[3] - x) + tensor(sigmaPoints[4] - x));
  cov *= 0.5f;
}

template <class V> InertiaSensorFilter::State<V> InertiaSensorFilter::State<V>::operator+(const Vector2<V>& value) const
{
  return State(*this) += value;
}

template <class V> InertiaSensorFilter::State<V>& InertiaSensorFilter::State<V>::operator+=(const Vector2<V>& value)
{
  rotation *= RotationMatrix(rotation.invert() * Vector3<V>(value.x, value.y, V()));
  return *this;
}

template <class V> Vector2<V> InertiaSensorFilter::State<V>::operator-(const State<V>& other) const
{
  const Vector3<V>& rotDiff(other.rotation * ((const RotationMatrix&)(other.rotation.invert() * rotation)).getAngleAxis());
  return Vector2<V>(rotDiff.x, rotDiff.y);
}


void InertiaSensorFilter::cholOfCov()
{
  // improved symmetry
  const double a11 = cov.c[0].x;
  const double a21 = (cov.c[0].y + cov.c[1].x) * 0.5f;

  const double a22 = cov.c[1].y;

  double& l11(l.c[0].x);
  double& l21(l.c[0].y);

  double& l22(l.c[1].y);

  //ASSERT(a11 >= 0.f);
  l11 = sqrt(std::max(a11, 0.0));
  if(l11 == 0.f) l11 = 0.0000000001f;
  l21 = a21 / l11;

  //ASSERT(a22 - l21 * l21 >= 0.f);
  l22 = sqrt(std::max(a22 - l21 * l21, 0.0));
  if(l22 == 0.f) l22 = 0.0000000001f;
}

void InertiaSensorFilter::meanOfSigmaReadings()
{
  readingMean = sigmaReadings[0];
  //for(int i = 0; i < 5; ++i) // ~= 0 .. inf
  for(int i = 0; i < 1; ++i)
  {
    Vector3<double> chunk((sigmaReadings[0] - readingMean) +
                    ((sigmaReadings[1] - readingMean) + (sigmaReadings[2] - readingMean)) +
                    ((sigmaReadings[3] - readingMean) + (sigmaReadings[4] - readingMean)));
    chunk /= 5.f;
    readingMean += chunk;
  }
}

void InertiaSensorFilter::covOfSigmaReadingsAndSigmaPoints()
{
  readingsSigmaPointsCov = (
    (tensor(sigmaReadings[1] - readingMean, l.c[0]) + tensor(sigmaReadings[2] - readingMean, l.c[1])) +
    (tensor(sigmaReadings[3] - readingMean, -l.c[0]) + tensor(sigmaReadings[4] - readingMean, -l.c[1])));
  readingsSigmaPointsCov *= 0.5f;
}

void InertiaSensorFilter::covOfSigmaReadings()
{
  readingsCov = (tensor(sigmaReadings[0] - readingMean) +
                 (tensor(sigmaReadings[1] - readingMean) + tensor(sigmaReadings[2] - readingMean)) +
                 (tensor(sigmaReadings[3] - readingMean) + tensor(sigmaReadings[4] - readingMean)));
  readingsCov *= 0.5f;
}
