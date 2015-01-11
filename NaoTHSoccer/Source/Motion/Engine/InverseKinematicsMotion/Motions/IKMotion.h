/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class IKMotion
*/

#ifndef _IKMotion_h_
#define _IKMotion_h_

#include "Motion/Engine/AbstractMotion.h"

#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

class IKMotion: public AbstractMotion
{
protected:

  InverseKinematic::CoMFeetPose getStandPose(double comHeight, bool standard=true) const;

private:
  const InverseKinematicsMotionEngineService& theEngineService;

protected:

  /**
  *
  */
  inline InverseKinematicsMotionEngine& getEngine() const
  {
    return theEngineService.getEngine();
  }

public:

  IKMotion(
    const InverseKinematicsMotionEngineService& theEngineService, 
    motion::MotionID id,
    MotionLock& lock);
  
  virtual ~IKMotion();
};

#endif // _IKMotion_h_
