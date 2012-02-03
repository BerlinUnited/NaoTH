/**
* @file GoalSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class GoalSymbols
*/

#ifndef __GaolSymbols_H_
#define __GoalSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

BEGIN_DECLARE_MODULE(GoalSymbols)
  REQUIRE(LocalGoalModel);
  REQUIRE(SensingGoalModel);
  REQUIRE(GoalPercept);
  REQUIRE(PlayerInfo);
  REQUIRE(FrameInfo);
END_DECLARE_MODULE(GoalSymbols)

class GoalSymbols: public GoalSymbolsBase
{

public:
  GoalSymbols()
    :
    localGoalModel(getLocalGoalModel()),
    goalPercept(getGoalPercept()),
    playerInfo(getPlayerInfo()),
    frameInfo(getFrameInfo())
  {
    theInstance = this;
    };
  ~GoalSymbols(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  // representations
  LocalGoalModel const& localGoalModel;
  GoalPercept const& goalPercept;
  PlayerInfo const& playerInfo;
  FrameInfo const& frameInfo;

  static GoalSymbols* theInstance;


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

#endif // __GoalSymbols_H_
