/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _COM_ERROR_PROVIDER_H
#define _COM_ERROR_PROVIDER_H

#include <ModuleFramework/Module.h>

#include "FootStep.h"
#include "../IKParameters.h"
#include "Representations/Motion/Walk2018/CommandBuffer.h"
#include "Representations/Motion/Walk2018/CoMErrors.h"

#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(CoMErrorProvider)
  REQUIRE(KinematicChainSensor)
  REQUIRE(CommandBuffer)

  PROVIDE(CoMErrors)
END_DECLARE_MODULE(CoMErrorProvider)

class CoMErrorProvider : private CoMErrorProviderBase
{
public:
  CoMErrorProvider(){}

  virtual void execute(){
      int observerMeasurementDelay = 40;
      int index = int(observerMeasurementDelay / 10 - 0.5);
      ASSERT(index >= 0);

      // we need enough history
      if(getCommandBuffer().poses.size() == 0 || getCommandBuffer().poses.size() <= index ) {
        return;
      }

      InverseKinematic::CoMFeetPose expectedCoMFeetPose = getCommandBuffer().poses[index];

      Vector3d requested_com;
      Vector3d observed_com;

      // if right support
      if(getCommandBuffer().footIds[index] == FootStep::LEFT)
      {
        //const Pose3D& footRef_right = expectedCoMFeetPose.feet.right;
        //requested_com = footRef_right.local(expectedCoMFeetPose.com).translation;
        expectedCoMFeetPose.localInRightFoot();
        requested_com = expectedCoMFeetPose.com.translation;

        Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
        footObs.translate(0, 0, -NaoInfo::FootHeight);
        footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
        observed_com = footObs.invert() * getKinematicChainSensor().CoM;
      }
      else
      {
        //const Pose3D& footRef_left = expectedCoMFeetPose.feet.left;
        //requested_com = footRef_left.local(expectedCoMFeetPose.com).translation;
        expectedCoMFeetPose.localInLeftFoot();
        requested_com = expectedCoMFeetPose.com.translation;

        Pose3D footObs = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
        footObs.translate(0, 0, -NaoInfo::FootHeight);
        footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
        observed_com = footObs.invert() * getKinematicChainSensor().CoM;
      }

      Vector3d e = requested_com - observed_com;
      getCoMErrors().absolute2.add(e.abs2());
      getCoMErrors().e.add(e);
  }

};

#endif // _COM_ERROR_PROVIDER_H
