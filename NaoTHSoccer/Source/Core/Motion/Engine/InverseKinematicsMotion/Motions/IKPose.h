/**
* @file IKPose.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of Pose for IK motion
*/

#ifndef __IK_POSE_H_
#define __IK_POSE_H_
#include "Tools/Math/Pose3D.h"
#include "Motion/MorphologyProcessor/InverseKinematics.h"
#include "Tools/NaoInfo.h"

namespace InverseKinematic
{
  class FeetPose
  {    
  public:
    Pose3D left, right;
  };
  
  class BodyFeetPose
  {
  protected:
    void localPose(Pose3D& p0, Pose3D& p1, Pose3D& p2)
    {
      p1 = p0.local(p1);
      p2 = p0.local(p2);
      p0 = Pose3D();
    }
    
    void localInBody()
    {
      localPose(body(), feet.left, feet.right);
    }
    
  public:
    FeetPose feet;
    
    virtual Pose3D& body() = 0;
    virtual const Pose3D& body() const = 0;
    
    void localInLeftFoot()
    {
      localPose(feet.left, feet.right, body());
    }

    void localInRightFoot()
    {
      localPose(feet.right, feet.left, body());
    }
  };
  
  class HipFeetPose: public BodyFeetPose
  {
  public:
      Pose3D hip;
      
      virtual Pose3D& body() { return hip; }
      virtual const Pose3D& body() const { return hip; }

      void localInHip()
      {
        localInBody();
      }
  };

  class CoMFeetPose: public BodyFeetPose
  {
  public:
    Pose3D com;
    
    virtual Pose3D& body() { return com; }
    virtual const Pose3D& body() const { return com; }
      
    void localInCoM()
    {
      localInBody();
    }
  };
  
  class ZMPFeetPose: public BodyFeetPose
  {
  public:
    Pose3D zmp;
    
    virtual Pose3D& body() { return zmp; }
    virtual const Pose3D& body() const { return zmp; }
      
    void localInZMP()
    {
      localInBody();
    }
  };
}

#endif //__IK_POSE_h_
