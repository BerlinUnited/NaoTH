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
  class Pose
  {
  protected:
    void localPose(Pose3D& p0, Pose3D& p1, Pose3D& p2)
    {
      p1 = p0.local(p1);
      p2 = p0.local(p2);
      p0 = Pose3D();
    }
    
  public:
    Pose3D lFoot, rFoot;
    
    virtual Pose3D& body() = 0;
    virtual const Pose3D& body() const = 0;
    
    void localInLeftFoot()
    {
      localPose(lFoot, rFoot, body());
    }

    void localInRightFoot()
    {
      localPose(rFoot, lFoot, body());
    }
  };
  
  class HipFeetPose: public Pose
  {
  public:
      Pose3D hip;
      
      virtual Pose3D& body() { return hip; }
      virtual const Pose3D& body() const { return hip; }

      void localInHip()
      {
        localPose(hip, lFoot, rFoot);
      }
  };

  class CoMFeetPose: public Pose
  {
  public:
    Pose3D com;
    
    virtual Pose3D& body() { return com; }
    virtual const Pose3D& body() const { return com; }
      
    void localInCoM()
    {
      localPose(com, lFoot, rFoot);
    }
  };
  
  class ZMPFeetPose: public Pose
  {
  public:
    Pose3D zmp;
    
    virtual Pose3D& body() { return zmp; }
    virtual const Pose3D& body() const { return zmp; }
      
    void localInZMP()
    {
      localPose(zmp, lFoot, rFoot);
    }
  };
}

#endif //__IK_POSE_h_
