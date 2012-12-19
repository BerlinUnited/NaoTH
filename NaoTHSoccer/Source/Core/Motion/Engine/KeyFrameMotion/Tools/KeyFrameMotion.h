/**
* @file KeyFrameMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class KeyFrameMotion
*/

#ifndef _KeyFrameMotion_h_
#define _KeyFrameMotion_h_

#include "Motion/Engine/AbstractMotion.h"
#include "Tools/DataStructures/Printable.h"

#include "MotionNet.h"
#include "Tools/Math/Common.h"


#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>

BEGIN_DECLARE_MODULE(KeyFrameMotion)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(KeyFrameMotion)

class KeyFrameMotion: private KeyFrameMotionBase, public AbstractMotion, public naoth::Printable
{
private:
  std::string name;
  MotionNet currentMotionNet;

  MotionNet::KeyFrame currentKeyFrame;
  MotionNet::Transition currentTransition;

  
  /** remaining time till next frame */
  double t;

  /**  */
  naoth::MotorJointData lastMotorJointData;

  void getNextTransition(std::string condition);

public:

  double stiffness;

  KeyFrameMotion(const MotionNet& currentMotionNet, motion::MotionID id);
  KeyFrameMotion();

  void set(const MotionNet& currentMotionNet, motion::MotionID id)
  {
    this->setId(id);
    this->name = motion::getName(id);
    this->currentMotionNet = currentMotionNet;
  }

//  KeyFrameMotion(const KeyFrameMotion& other);

  // Zuweisungsoperator
//  KeyFrameMotion& operator = (const KeyFrameMotion& src);

  ~KeyFrameMotion();

  // override
  virtual void init();

  void execute();
  
  void print(std::ostream& stream) const;

  void set(MotionNet& motionNet){currentMotionNet = motionNet;}
  MotionNet getCurrentMotionNet(){return currentMotionNet;}
  
};

#endif // _KeyFrameMotion_h_
