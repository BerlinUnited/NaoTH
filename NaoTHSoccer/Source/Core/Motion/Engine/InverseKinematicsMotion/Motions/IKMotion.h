/**
* @file IKMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class IKMotion
*/

#ifndef _IKMotion_h_
#define _IKMotion_h_

#include "Motion/AbstractMotion.h"
#include "IKPose.h"
#include "IKParameters.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

class IKMotion: public AbstractMotion 
{
protected:

  InverseKinematic::CoMFeetPose getStandPose(double comHeight, bool standard=true) const;

private:
  const InverseKinematicsMotionEngineService& theEngineService;

protected:

  //const IKParameters& theParameters;
  
  //virtual InverseKinematicsMotionEngineService& getInverseKinematicsMotionEngineService() const = 0;

  inline InverseKinematicsMotionEngine& getEngine() const
  {
    assert(theEngineService.theEngine != NULL);
    return *(theEngineService.theEngine);
  }

public:

  IKMotion(
    const InverseKinematicsMotionEngineService& theEngineService, 
    motion::MotionID id,
    MotionLock& lock);
  
  virtual ~IKMotion();

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& motionStatus) = 0;

};

#endif // _IKMotion_h_
