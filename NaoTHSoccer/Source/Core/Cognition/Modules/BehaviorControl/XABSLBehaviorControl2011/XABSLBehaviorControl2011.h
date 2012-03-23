/* 
 * File:   XABSLBehaviorControl.h
 * Author: thomas
 *
 * Created on 9. Januar 2009, 21:31
 */

#ifndef _XABSLBehaviorControl2011_H
#define  _XABSLBehaviorControl2011_H

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// engine
#include <XabslEngine/XabslEngine.h>
#include <XabslEngine/XabslTools.h>

#include "Tools/Xabsl/XabslFileInputSource.h"

/*

#include "Representations/Modeling/BodyState.h"
#include "Representations/Infrastructure/CameraSettings.h"

#include "PlatformInterface/Platform.h"
*/

#include "Representations/Modeling/BehaviorStatus.h"


// symbols
#include "Symbols/BallSymbols2011.h"
#include "Symbols/GameSymbols2011.h"
#include "Symbols/MotionSymbols2011.h"
#include "Symbols/TeamSymbols2011.h"
#include "Symbols/SensorSymbols2011.h"
#include "Symbols/MathSymbols2011.h"
#include "Symbols/GoalSymbols2011.h"
#include "Symbols/LedSymbols2011.h"
#include "Symbols/SelflocSymbols2011.h"
#include "Symbols/OdometrySymbols2011.h"
#include "Symbols/FieldSymbols2011.h"
#include "Symbols/StrategySymbols2011.h"
#include "Symbols/SoundSymbols2011.h"
#include "Symbols/LineSymbols2011.h"


// ErrorHandler
class MyErrorHandler2011 : public xabsl::ErrorHandler
{
public:

  MyErrorHandler2011();
  void printError(const char* txt);

  void printMessage(const char* txt);

  virtual ~MyErrorHandler2011();
};

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////


BEGIN_DECLARE_MODULE(XABSLBehaviorControl2011)
  REQUIRE(FrameInfo)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(BallModel)
  REQUIRE(PlayerInfo)
  REQUIRE(MotionStatus)
  REQUIRE(FieldInfo)
  REQUIRE(RobotPose)
  REQUIRE(CompassDirection)

  PROVIDE(MotionRequest)
  PROVIDE(BehaviorStatus)
END_DECLARE_MODULE(XABSLBehaviorControl2011)
//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class XABSLBehaviorControl2011 :
  private XABSLBehaviorControl2011Base,
  private ModuleManager, 
  public DebugCommandExecutor
{
public:
  XABSLBehaviorControl2011();
  virtual ~XABSLBehaviorControl2011();

  virtual void execute();

  void reloadBehaviorFromFile(std::string file, std::string agent);

  virtual void executeDebugCommand(
    const std::string& command, 
    const std::map<std::string,std::string>& arguments, 
    std::ostream &outstream);

private:
  /** Register all needed symbols */
  void registerXABSLSymbols();

  /** update symbols */
  void updateXABSLSymbols();

  /** udpate output symbols */
  void updateOutputSymbols();

  /** some debug drawings */
  void draw();

  xabsl::Engine* theEngine;
  MyErrorHandler2011 theErrorHandler;
  // TODO: remove this member
  std::string agentName;

  /** time passed into the xabsl engine */
  unsigned int xabslTime;


  void fillActiveOptions(naothmessages::BehaviorStatus &status);
  void fillAction(const xabsl::Action* source, naothmessages::XABSLAction* dest);
  void fillRegisteredSymbols(naothmessages::BehaviorStatus &status);

  //symbols
  ModuleCreator<BallSymbols2011>* theBallSymbols2011;
  ModuleCreator<GameSymbols2011>* theGameSymbols2011;
  ModuleCreator<MotionSymbols2011>* theMotionSymbols2011;
  ModuleCreator<TeamSymbols2011>* theTeamSymbols2011;
  ModuleCreator<SensorSymbols2011>* theSensorSymbols2011;
  ModuleCreator<MathSymbols2011>* theMathSymbols2011;
  ModuleCreator<GoalSymbols2011>* theGoalSymbols2011;
  ModuleCreator<LedSymbols2011>* theLedSymbols2011;
  ModuleCreator<SelflocSymbols2011>* theSelflocSymbols2011;
  ModuleCreator<OdometrySymbols2011>* theOdometrySymbols2011;
  ModuleCreator<FieldSymbols2011>* theFieldSymbols2011;
  ModuleCreator<StrategySymbols2011>* theStrategySymbols2011;
  ModuleCreator<SoundSymbols2011>* theSoundSymbols2011;
  ModuleCreator<LineSymbols2011>* theLineSymbols2011;
};

#endif  /* _XABSLBehaviorControl2011_H */
