/**
* @file OdometryCalculator.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* OdometryCalculator: calculate the odometry according to morphology and support foot
*/


#include "OdometryCalculator.h"

using namespace naoth;

OdometryCalculator::OdometryCalculator()
  :
  init(false),
  accumulatedGyroRotationZ(0.0)
{
  DEBUG_REQUEST_REGISTER("OdometryCalculator:support_foot", "plot the support foot in OdometryCalculator", false);

  getDebugParameterList().add(&parameter);
}

OdometryCalculator::~OdometryCalculator()
{
  getDebugParameterList().remove(&parameter);
}

void OdometryCalculator::execute()
{
  // TODO: make it better
  OdometryData& od = getOdometryData();
  const KinematicChain& kc = getKinematicChainSensor();

  GroundContactModel::Foot supportFoot = getGroundContactModel().supportFoot;

  if ( init ) 
  {  
    DEBUG_REQUEST("OdometryCalculator:support_foot",
      PLOT("OdometryCalculator:support_foot",static_cast<double>(supportFoot));
    );

    const Pose3D& lastFoot = (supportFoot == GroundContactModel::LEFT) ? lastLeftFoot : lastRightFoot;

    KinematicChain::LinkID footId = (supportFoot == GroundContactModel::LEFT) ? KinematicChain::LFoot : KinematicChain::RFoot;
    const Pose3D& foot = kc.theLinks[footId].M;
    const Pose3D& hip = kc.theLinks[KinematicChain::Hip].M;

    Pose3D local = foot.local(hip);
    Pose3D lastLocal = lastFoot.local(lastHip);
    RotationMatrix rot = lastLocal.rotation.invert() * local.rotation;
    Vector3d trans = local.translation - lastLocal.translation;     
    od.translate(trans.x, trans.y);
    od.rotate(rot.getZAngle());
  } else {
    od.translation.x = 0;
    od.translation.y = 0;
    od.rotation = 0;
    init = true;
  }

  // NOTE: (deprecated) estimate the Z rotation based on simple gyro accumulation
  // only for testing purposes
  if(parameter.useGyroRotationOdometry) 
  {
    // a simple model for gyro rotation
    if(getCalibrationData().calibrated) {
      accumulatedGyroRotationZ += getGyrometerData().data.z * getRobotInfo().getBasicTimeStepInSecond();
    } else {
      accumulatedGyroRotationZ = 0.0;
    }
    PLOT("OdometryCalculator:accumulatedGyroRotationZ", accumulatedGyroRotationZ);

    getOdometryData().rotation = accumulatedGyroRotationZ;
  }

  if(parameter.useIMUDataForRotationOdometry) {
    double z_angle = RotationMatrix(getIMUData().rotation).getZAngle();
    PLOT("Motion:rotationZ", z_angle);
    getOdometryData().rotation = z_angle;
  }

  // cache data
  lastLeftFoot.translation = kc.theLinks[KinematicChain::LFoot].p;
  lastLeftFoot.rotation = kc.theLinks[KinematicChain::LFoot].R;
  lastRightFoot.translation = kc.theLinks[KinematicChain::RFoot].p;
  lastRightFoot.rotation = kc.theLinks[KinematicChain::RFoot].R;
  lastHip.translation = kc.theLinks[KinematicChain::Hip].p;
  lastHip.rotation = kc.theLinks[KinematicChain::Hip].R;

  ASSERT(!Math::isNan(od.translation.x));
  ASSERT(!Math::isNan(od.translation.y));
  ASSERT(!Math::isNan(od.rotation));
 }//end execute

