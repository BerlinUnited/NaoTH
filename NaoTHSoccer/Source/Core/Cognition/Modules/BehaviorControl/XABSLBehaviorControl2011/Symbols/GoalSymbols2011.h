/**
* @file GoalSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class GoalSymbols
*/

#ifndef __GaolSymbols2011_H_
#define __GoalSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

BEGIN_DECLARE_MODULE(GoalSymbols2011)
  REQUIRE(LocalGoalModel);
  REQUIRE(SensingGoalModel);
  REQUIRE(GoalPercept);
  REQUIRE(PlayerInfo);
  REQUIRE(FrameInfo);
END_DECLARE_MODULE(GoalSymbols2011)

class GoalSymbols2011: public GoalSymbols2011Base
{

public:
  GoalSymbols2011()
    :
    localGoalModel(getLocalGoalModel()),
    goalPercept(getGoalPercept()),
    playerInfo(getPlayerInfo()),
    frameInfo(getFrameInfo())
  {
    theInstance = this;
    };
  ~GoalSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  // representations
  LocalGoalModel const& localGoalModel;
  GoalPercept const& goalPercept;
  PlayerInfo const& playerInfo;
  FrameInfo const& frameInfo;

  static GoalSymbols2011* theInstance;


  //get-method

  static double getTimeSinceWholeOwnGoalSeen();
  static double getTimeSinceWholeOppGoalSeen();

  static double getAngleToOpponentGoal();
  static double getDistanceToOpponentGoal();
  static double getOpponentGoalX();
  static double getOpponentGoalY();
  static double getTimeSinceOpponentGoalSeen();

  static double getAngleToOwnGoal();
  static double getDistanceToOwnGoal();
  static double getOwnGoalX();
  static double getOwnGoalY();
  static double getTimeSinceOwnGoalSeen();
  static double getGoalCentroidX();
  static double getGoalCentroidY();
  static double getGoalCentroidZ();

  static bool getOpponentGoalWasSeen();
  static bool getOwnGoalWasSeen();
  static bool getOppGoalIsValid();

};//end class GoalSymbols

#endif // __GoalSymbols2011_H_
