/**
* @file KeyFrameMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class KeyFrameMotion
*/

#ifndef _KeyFrameMotion_h_
#define _KeyFrameMotion_h_

#include "MotionNet.h"
#include "Tools/Math/Common.h"
#include "Motion/AbstractMotion.h"
#include "Tools/DataStructures/Printable.h"

class KeyFrameMotion: public AbstractMotion, public naoth::Printable
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
//  KeyFrameMotion(const KeyFrameMotion& other);

  // Zuweisungsoperator
//  KeyFrameMotion& operator = (const KeyFrameMotion& src);

  ~KeyFrameMotion();

  // override
  virtual void init();
  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/);
  
  void print(std::ostream& stream) const;

  void set(MotionNet& motionNet){currentMotionNet = motionNet;}
  MotionNet getCurrentMotionNet(){return currentMotionNet;}
  
};

#endif // _KeyFrameMotion_h_
