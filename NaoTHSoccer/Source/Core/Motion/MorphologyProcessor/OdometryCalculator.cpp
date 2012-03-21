/**
* @file OdometryCalculator.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* OdometryCalculator: calculate the odometry according to morphology and support foot
*/

#include <Tools/Math/Pose3D.h>
#include <Tools/Math/Pose2D.h>

#include "OdometryCalculator.h"

using namespace naoth;

OdometryCalculator::OdometryCalculator()
  :
  supportFoot(false),
  init(false)
{

//TODO
    //DEBUG_REQUEST_REGISTER("Motion:OdometryCalculator:support_foot", "plot the support foot in OdometryCalculator", false);
}

OdometryCalculator::~OdometryCalculator()
{
}

void OdometryCalculator::calculateOdometry(OdometryData& od, const KinematicChain& kc, const FSRData& fsr)
{
    static bool lastFootChoiceValid = true;

    // simple support foot detection: the foot which has greater force is the support foot.
    double leftForce = 0;
    double rightForce = 0;
    for (int i = FSRData::LFsrFL; i <= FSRData::LFsrBR; i++) {
      if ( fsr.valid[i] ){
        leftForce += fsr.force[i];
      }
    }
    for (int i = FSRData::RFsrFL; i <= FSRData::RFsrBR; i++) {
      if ( fsr.valid[i] ){
        rightForce += fsr.force[i];
      }
    }

    if ( leftForce <=0 && rightForce <= 0 ) 
    {
      if(lastFootChoiceValid)
        lastFootChoiceValid = false;
      else
        return;
    }else
    {
      lastFootChoiceValid = true;
    }

    if (leftForce > rightForce) {
        supportFoot = true;
    } else if (leftForce < rightForce) {
        supportFoot = false;
    }

    if ( init ) {
      //TODO
      //DEBUG_REQUEST("Motion:OdometryCalculator:support_foot",
      //  PLOT("OdometryCalculator.support_foot",static_cast<double>(supportFoot));
      //  );

        const Pose3D& lastFoot = supportFoot ? lastLeftFoot : lastRightFoot;

        KinematicChain::LinkID footId = supportFoot ? KinematicChain::LFoot : KinematicChain::RFoot;
        const Pose3D& foot = kc.theLinks[footId].M;
        const Pose3D& hip = kc.theLinks[KinematicChain::Hip].M;

        Pose3D local = foot.local(hip);
        Pose3D lastLocal = lastFoot.local(lastHip);
        RotationMatrix rot = lastLocal.rotation.invert() * local.rotation;
        Vector3<double> trans = local.translation - lastLocal.translation;     
        od.translate(trans.x, trans.y);
        od.rotate(rot.getZAngle());
    }
    else{
        od.translation.x = 0;
        od.translation.y = 0;
        od.rotation = 0;
        init = true;
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
 }

