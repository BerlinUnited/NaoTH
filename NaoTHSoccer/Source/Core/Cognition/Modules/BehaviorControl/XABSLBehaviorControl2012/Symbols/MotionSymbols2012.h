/**
* @file MotionSymbols2012.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class MotionSymbols2012
*/

#ifndef _MotionSymbols2012_H_
#define _MotionSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"

// representations
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(MotionSymbols2012)
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(PlayersModel)
  REQUIRE(SensorJointData)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(MotionSymbols2012)

class MotionSymbols2012: public MotionSymbols2012Base
{

public:
  MotionSymbols2012()
  :
    headMotionRequest(getHeadMotionRequest()),
    motionRequest(getMotionRequest()),
    motionStatus(getMotionStatus()),
    kinematicChain(getKinematicChain()),
    frameInfo(getFrameInfo()),
    playerInfo(getPlayerInfo()),
    playersModel(getPlayersModel()),
    sensorJointData(getSensorJointData()),

    turn_around_radius(0.0),
    relativeToLeftFootXx(0.0),
    relativeToLeftFootXy(0.0),
    relativeToLeftFootYx(0.0),
    relativeToLeftFootYy(0.0),
    relativeToLeftFootAa(0.0),
    relativeToRightFootXx(0.0),
    relativeToRightFootXy(0.0),
    relativeToRightFootYx(0.0),
    relativeToRightFootYy(0.0),
    relativeToRightFootAa(0.0),
    isAvoidObstacle(false),
    headSpeed(0.0),

    walkStyle(normal),
    stepControlFoot(none),
    stepControlRequestTime(0),
    stepControlRequestSpeedDirection(0)
  {
    theInstance = this;
  };
  virtual ~MotionSymbols2012(){}
  
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

  // representations
  HeadMotionRequest& headMotionRequest;
  MotionRequest& motionRequest;
  const MotionStatus& motionStatus;
  const KinematicChain& kinematicChain;
  const naoth::FrameInfo& frameInfo;
  const PlayerInfo& playerInfo;
  const PlayersModel& playersModel;
  const naoth::SensorJointData& sensorJointData;

  static MotionSymbols2012* theInstance;

  double turn_around_radius;
  
  // setter and getter

  // enum
  static void setHeadMotionRequest(int value);
  static int getHeadMotionRequestId();
  static int getHeadMotionStatus();

  static void setMotionRequestId(int value);
  static int getMotionRequestId();
  static int getMotionStatusId();
  static double getMotionStatusTime();
  static bool getMotionStatusLeftMovable();
  static bool getMotionStatusRightMovable();

  static void setCameraID(int value);
  static int getCameraID();

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

  double relativeToLeftFootXx, relativeToLeftFootXy;
  static double relativeToLeftFootX();
  double relativeToLeftFootYx, relativeToLeftFootYy;
  static double relativeToLeftFootY();
  double relativeToLeftFootAa;
  static double relativeToLeftFootA();
  double relativeToRightFootXx, relativeToRightFootXy;
  static double relativeToRightFootX();
  double relativeToRightFootYx, relativeToRightFootYy;
  static double relativeToRightFootY();
  double relativeToRightFootAa;
  static double relativeToRightFootA();

  static double relativeToX(double x, double y, const RotationMatrix& R, const Vector3<double>& p);

  static double relativeToY(double x, double y, const RotationMatrix& R, const Vector3<double>& p);

  static double relativeToA(double a, const RotationMatrix& R);

  static bool avoidObstacle();
  static void setAvoidObstacle(bool b);
  static bool getAvoidObstacle();
  bool isAvoidObstacle;

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

  RingBuffer<HeadMotion, 10 > headMotionBuffer;

  double headSpeed;

  WalkStyle walkStyle;

  enum StepControlFoot
  {
    left,
    right,
    none
  };

  static std::string getStepControlFootName(StepControlFoot i);

  StepControlFoot stepControlFoot;
  double stepControlRequestTime;
  Pose2D stepControlRequestTarget;
  double stepControlRequestSpeedDirection;
};//end class MotionSymbols2012

#endif // __MotionSymbols2012_H_
