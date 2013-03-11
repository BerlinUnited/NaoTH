/**
 * @file: TeamSymbols.h
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#ifndef _TEAMSYMBOLS2011_H
#define _TEAMSYMBOLS2011_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(TeamSymbols2011)
  REQUIRE(TeamMessage)
  REQUIRE(FrameInfo)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(FieldInfo)
  REQUIRE(BodyState)
  REQUIRE(SoccerStrategy)
  REQUIRE(MotionStatus)

  PROVIDE(PlayerInfo)
END_DECLARE_MODULE(TeamSymbols2011)

class TeamSymbols2011: public TeamSymbols2011Base
{
public:

  TeamSymbols2011()
  :
    teamMessage(getTeamMessage()),
    frameInfo(getFrameInfo()),
    ballModel(getBallModel()),
    robotPose(getRobotPose()),
    fieldInfo(getFieldInfo()),
    bodyState(getBodyState()),
    soccerStrategy(getSoccerStrategy()),
    motionStatus(getMotionStatus()),
    playerInfo(getPlayerInfo())
  {
    theInstance = this;
  }

  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

  virtual ~TeamSymbols2011();
private:
  TeamMessage const& teamMessage;
  FrameInfo const& frameInfo;
  BallModel const& ballModel;
  RobotPose const& robotPose;
  FieldInfo const& fieldInfo;
  BodyState const& bodyState;
  SoccerStrategy const& soccerStrategy;
  MotionStatus const& motionStatus;
  PlayerInfo& playerInfo;

  static TeamSymbols2011* theInstance;
  static double getTeamMembersAliveCount();
  static bool calculateIfStriker();
  static bool calculateIfStrikerByTimeToBall();
  static int whoIsFastestToBall();
  static bool getWasStriker();
  static void setWasStriker(bool striker);
  static bool calculateIfTheLast();
};

#endif  /* _TEAMSYMBOLS2011_H */
