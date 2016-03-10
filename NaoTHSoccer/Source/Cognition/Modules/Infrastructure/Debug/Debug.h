/* 
 * File:   Debug.h
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <ModuleFramework/Module.h>
#include <DebugCommunication/DebugCommandExecutor.h>

//#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/FieldInfo.h>

//#include "Representations/Modeling/ColorClassificationModel.h"
//#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/CameraMatrix.h"

#include <DebugCommunication/DebugCommandManager.h>
#include "Tools/Debug/Logger.h"
#include <Representations/Debug/Stopwatch.h>
#include <Tools/Debug/DebugImageDrawings.h>
#include "Tools/Debug/DebugDrawings.h"
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"

#include <Tools/DataStructures/ParameterList.h>

// debug
#include <Tools/Debug/DebugDrawings3D.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Debug)

// debug
  PROVIDE(StopwatchManager)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugCommandManager)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)

  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
//  REQUIRE(FieldInfo)
  PROVIDE(Image)
  PROVIDE(ImageTop)

  // 3D drawings
  REQUIRE(RobotPose)
  REQUIRE(KinematicChain)
  REQUIRE(BallModel)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  PROVIDE(CameraInfo)
  PROVIDE(CameraInfoTop)

  
//  PROVIDE(ColorTable64)
//  PROVIDE(ColorClassificationModel)
//  PROVIDE(MotionRequest)
END_DECLARE_MODULE(Debug)

class Debug : public DebugBase, virtual private BlackBoardInterface, public DebugCommandExecutor
{
public:
  Debug();
  virtual ~Debug();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("DebugParameter") 
    {
      PARAMETER_REGISTER(test) = 20;

      syncWithConfig();
    }

    double test;

  } parameter;

private:
  Logger cognitionLogger;

  void draw3D();
  void drawRobot3D(const Pose3D& robotPose);
  void drawKinematicChain3D();

  void registerLogableRepresentationList()
  {
    const BlackBoard& blackBoard = BlackBoardInterface::getBlackBoard();
    BlackBoard::Registry::const_iterator iter;

    for(iter = blackBoard.getRegistry().begin(); iter != blackBoard.getRegistry().end(); ++iter)
    {
      const Representation& theRepresentation = iter->second->getRepresentation();
      cognitionLogger.addRepresentation(&theRepresentation, iter->first);
    }
  }

};

#endif  /* _DEBUG_H */

