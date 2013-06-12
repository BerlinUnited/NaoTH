/* 
 * File:   XABSLBehaviorControl.h
 * Author: thomas
 *
 * Created on 9. Januar 2009, 21:31
 */

#ifndef _XABSLBehaviorControl2012_H
#define _XABSLBehaviorControl2012_H

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
#include "Symbols/BallSymbols2012.h"
#include "Symbols/GameSymbols2012.h"
#include "Symbols/MotionSymbols2012.h"
#include "Symbols/TeamSymbols2012.h"
#include "Symbols/SensorSymbols2012.h"
#include "Symbols/MathSymbols2012.h"
#include "Symbols/GoalSymbols2012.h"
#include "Symbols/LedSymbols2012.h"
#include "Symbols/SelflocSymbols2012.h"
#include "Symbols/OdometrySymbols2012.h"
#include "Symbols/FieldSymbols2012.h"
#include "Symbols/StrategySymbols2012.h"
#include "Symbols/SoundSymbols2012.h"
#include "Symbols/LineSymbols2012.h"


// ErrorHandler
class MyErrorHandler2012 : public xabsl::ErrorHandler
{
public:

  MyErrorHandler2012(std::ostream& out);
  void printError(const char* txt);

  void printMessage(const char* txt);

  virtual ~MyErrorHandler2012();

private:
  std::ostream& out;
};

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////


BEGIN_DECLARE_MODULE(XABSLBehaviorControl2012)
  REQUIRE(FrameInfo)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(BallModel)
  REQUIRE(PlayerInfo)
  REQUIRE(MotionStatus)
  REQUIRE(FieldInfo)
  REQUIRE(RobotPose)
  REQUIRE(CompassDirection)
  REQUIRE(MotionRequest)

  PROVIDE(BehaviorStatus)
END_DECLARE_MODULE(XABSLBehaviorControl2012)
//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class XABSLBehaviorControl2012 : 
  private XABSLBehaviorControl2012Base, 
  private ModuleManager, 
  public DebugCommandExecutor
{
public:
  XABSLBehaviorControl2012();
  virtual ~XABSLBehaviorControl2012();

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
  MyErrorHandler2012 theErrorHandler;
  std::stringstream error_stream;

  // TODO: remove this member
  std::string agentName;

  /** time passed into the xabsl engine */
  unsigned int xabslTime;


  void fillActiveOptions(naothmessages::BehaviorStatus &status);
  void fillAction(const xabsl::Action* source, naothmessages::XABSLAction* dest);
  void fillRegisteredSymbols(naothmessages::BehaviorStatus &status);

  //symbols
  ModuleCreator<BallSymbols2012>* theBallSymbols2012;
  ModuleCreator<GameSymbols2012>* theGameSymbols2012;
  ModuleCreator<MotionSymbols2012>* theMotionSymbols2012;
  ModuleCreator<TeamSymbols2012>* theTeamSymbols2012;
  ModuleCreator<SensorSymbols2012>* theSensorSymbols2012;
  ModuleCreator<MathSymbols2012>* theMathSymbols2012;
  ModuleCreator<GoalSymbols2012>* theGoalSymbols2012;
  ModuleCreator<LedSymbols2012>* theLedSymbols2012;
  ModuleCreator<SelflocSymbols2012>* theSelflocSymbols2012;
  ModuleCreator<OdometrySymbols2012>* theOdometrySymbols2012;
  ModuleCreator<FieldSymbols2012>* theFieldSymbols2012;
  ModuleCreator<StrategySymbols2012>* theStrategySymbols2012;
  ModuleCreator<SoundSymbols2012>* theSoundSymbols2012;
  ModuleCreator<LineSymbols2012>* theLineSymbols2012;
};

#endif  /* _XABSLBehaviorControl2012_H */
