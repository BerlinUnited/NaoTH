/**
* @file IKPose.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of Pose for IK motion
*/

#ifndef __IK_POSE_H_
#define __IK_POSE_H_
#include "Tools/Math/Pose3D.h"
#include "Core/Motion/MorphologyProcessor/InverseKinematics.h"
#include "Tools/NaoInfo.h"

namespace InverseKinematic
{
  template<typename T>
  void localPose(T& p0, T& p1, T& p2)
  {
    p1 = p0.local(p1);
    p2 = p0.local(p2);
    p0 = T();
  }
  
  struct HipFeetPose
  {
      Pose3D lFoot, rFoot, hip;

      void localInHip()
      {
        localPose(hip, lFoot, rFoot);
      }

      void localInLeftFoot()
      {
        localPose(lFoot, rFoot, hip);
      }

      void localInRightFoot()
      {
        localPose(rFoot, lFoot, hip);
      }
  };

  struct CoMFeetPose
  {
      Pose3D lFoot, rFoot, com;
      
      void localInCoM()
      {
        localPose(com, lFoot, rFoot);
      }

      void localInLeftFoot()
      {
        localPose(lFoot, rFoot, com);
      }

      void localInRightFoot()
      {
        localPose(rFoot, lFoot, com);
      }
  };
}

#endif //__IK_POSE_h_
