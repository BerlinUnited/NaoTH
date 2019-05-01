/**
* @file MotionSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class MotionSymbols
*/

#ifndef _MotionSymbols_H_
#define _MotionSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"

// representations
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Modeling/BallModel.h"
#include <Representations/Motion/CollisionPercept.h>

BEGIN_DECLARE_MODULE(MotionSymbols)
  REQUIRE(MotionStatus)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)

  REQUIRE(BallModel)
  REQUIRE(CollisionPercept)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(MotionSymbols)

class MotionSymbols: public MotionSymbolsBase
{

public:
  MotionSymbols()
  :
    walkStyle(normal),
    stepControlFoot(none),
    stepControlRequestTime(0),
    stepControlRequestSpeedDirection(0),
    stepControlScale(1.0),
    actionPerformed(-1)
  {
    theInstance = this;
  }

  virtual ~MotionSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

  void updateOutputSymbols();

  enum WalkStyle
  {
    stable,
    normal,
    fast,
    num_of_styles
  };

  static std::string getWalkStyleName(WalkStyle i);

private:
  static MotionSymbols* theInstance;

  // setter and getter
  static void setHeadMotionRequest(int value);
  static int getHeadMotionRequestId();
  static int getHeadMotionStatus();
  static void setCameraId(int value);
  static int getCameraId();

  static void setArmRequestId(int value);
  static int getArmRequestId();

  static void setMotionRequestId(int value);
  static int getMotionRequestId();
  static int getMotionStatusId();
  static double getMotionStatusTime();
  static bool getMotionStatusLeftMovable();
  static bool getMotionStatusRightMovable();

  static void setWalkOffsetRot(double rot);
  static void setWalkSpeedRot(double rot);
  static void setWalkSpeedY(double x);
  static void setWalkSpeedX(double y);
  static void setKickDirection(double alpha);

  static double getWalkOffsetRot();
  static double getWalkSpeedRot();
  static double getWalkSpeedX();
  static double getWalkSpeedY();
  static double getKickDirection();

  static double getHeadPitchAngle();
  static double getHeadYawAngle();

  // some local members
  struct HeadMotion
  {
    double headYaw;
    double headPitch;
    double time;
    HeadMotion()
      :
      headYaw(0.0),
      headPitch(0.0),
      time(0.0)
      {}
  };


  WalkStyle walkStyle;

  enum StepControlFoot
  {
    left,
    right,
    none
  };

  static std::string getStepControlFootName(StepControlFoot i);

  static bool dribbleG();
  static void dribble(bool v);

  StepControlFoot stepControlFoot;
  double stepControlRequestTime;
  Pose2D stepControlRequestTarget;
  double stepControlRequestSpeedDirection;
  double stepControlScale;
  int actionPerformed;
};//end class MotionSymbols

#endif // _MotionSymbols_H_
