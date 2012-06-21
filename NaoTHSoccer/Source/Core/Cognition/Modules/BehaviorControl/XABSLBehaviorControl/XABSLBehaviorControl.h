/* 
 * File:   XABSLBehaviorControl.h
 * Author: thomas
 *
 * Created on 9. Januar 2009, 21:31
 */

#ifndef _XABSLBehaviorControl_H
#define _XABSLBehaviorControl_H

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
#include "Symbols/BallSymbols.h"
#include "Symbols/GameSymbols.h"
#include "Symbols/MotionSymbols.h"
#include "Symbols/TeamSymbols.h"
#include "Symbols/SensorSymbols.h"
#include "Symbols/MathSymbols.h"
#include "Symbols/GoalSymbols.h"
#include "Symbols/LedSymbols.h"
#include "Symbols/SelflocSymbols.h"
#include "Symbols/OdometrySymbols.h"
#include "Symbols/FieldSymbols.h"
#include "Symbols/StrategySymbols.h"
#include "Symbols/SoundSymbols.h"
#include "Symbols/LineSymbols.h"


// ErrorHandler
class MyErrorHandler : public xabsl::ErrorHandler
{
public:

  MyErrorHandler(std::ostream& out);
  void printError(const char* txt);

  void printMessage(const char* txt);

  virtual ~MyErrorHandler();

private:
  std::ostream& out;
};

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////


BEGIN_DECLARE_MODULE(XABSLBehaviorControl)
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
END_DECLARE_MODULE(XABSLBehaviorControl)
//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class XABSLBehaviorControl : 
  private XABSLBehaviorControlBase, 
  private ModuleManager, 
  public DebugCommandExecutor
{
public:
  XABSLBehaviorControl();
  virtual ~XABSLBehaviorControl();

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
  MyErrorHandler theErrorHandler;
  std::stringstream error_stream;

  // TODO: remove this member
  std::string agentName;

  /** time passed into the xabsl engine */
  unsigned int xabslTime;


  void fillActiveOptions(naothmessages::BehaviorStatus &status);
  void fillAction(const xabsl::Action* source, naothmessages::XABSLAction* dest);
  void fillRegisteredSymbols(naothmessages::BehaviorStatus &status);

  //symbols
  ModuleCreator<BallSymbols>* theBallSymbols;
  ModuleCreator<GameSymbols>* theGameSymbols;
  ModuleCreator<MotionSymbols>* theMotionSymbols;
  ModuleCreator<TeamSymbols>* theTeamSymbols;
  ModuleCreator<SensorSymbols>* theSensorSymbols;
  ModuleCreator<MathSymbols>* theMathSymbols;
  ModuleCreator<GoalSymbols>* theGoalSymbols;
  ModuleCreator<LedSymbols>* theLedSymbols;
  ModuleCreator<SelflocSymbols>* theSelflocSymbols;
  ModuleCreator<OdometrySymbols>* theOdometrySymbols;
  ModuleCreator<FieldSymbols>* theFieldSymbols;
  ModuleCreator<StrategySymbols>* theStrategySymbols;
  ModuleCreator<SoundSymbols>* theSoundSymbols;
  ModuleCreator<LineSymbols>* theLineSymbols;
};

#endif  /* _XABSLBehaviorControl_H */
