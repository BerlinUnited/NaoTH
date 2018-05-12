/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _COM_ERROR_PROVIDER_H
#define _COM_ERROR_PROVIDER_H

#include <ModuleFramework/Module.h>

#include "Motion/Engine/InverseKinematicsMotion/Motions/IKPose.h"
#include "Representations/Motion/Walk2018/CommandBuffer.h"
#include "Representations/Motion/Walk2018/CoMErrors.h"

BEGIN_DECLARE_MODULE(CoMErrorProvider)
  REQUIRE(KinematicChainSensor) // required to calculate current CoM in support foot
  REQUIRE(CommandBuffer)        // required to compare current CoM with the requested one

  PROVIDE(CoMErrors)            // provide the error, absolute error to the power of 2 and relative (3D)
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
      Pose3D footObs;

      // if right support
      if(getCommandBuffer().footIds[index] == FootStep::LEFT)
      {
        expectedCoMFeetPose.localInRightFoot();
        footObs = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
      }
      else
      {
        expectedCoMFeetPose.localInLeftFoot();
        footObs = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
      }

      footObs.translate(0, 0, -NaoInfo::FootHeight);
      // TODO: check, why is x and z ignored?
      footObs.rotation = RotationMatrix::getRotationY(footObs.rotation.getYAngle()); // assume the foot is flat on the ground
      observed_com = footObs.invert() * getKinematicChainSensor().CoM;

      requested_com = expectedCoMFeetPose.com.translation;
      Vector3d e = requested_com - observed_com;
      getCoMErrors().absolute2.add(e.abs2());
      getCoMErrors().e.add(e);
  }
};

#endif // _COM_ERROR_PROVIDER_H
