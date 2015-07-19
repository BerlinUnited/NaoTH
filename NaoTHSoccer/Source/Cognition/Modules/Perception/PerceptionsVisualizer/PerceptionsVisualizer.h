/**
* @file PerceptionsVisualizer.h
*
* @author Kirill Yasinovskiy <yasinovs@informatik.hu-berlin.de>
* Definition of class PerceptionsVizualization
*/

#ifndef _PerceptionsVisualizer_h_
#define _PerceptionsVisualizer_h_

//include section
#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

//perceptions
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Representations/Infrastructure/FieldInfo.h"

//common representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Modeling/RobotPose.h"

#include "Tools/DoubleCamHelpers.h"

//debug
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings.h>


//module declaration section
BEGIN_DECLARE_MODULE(PerceptionsVisualizer)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)

  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)
  REQUIRE(LinePercept)
  REQUIRE(LinePerceptTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  
  REQUIRE(CameraMatrixOffset)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(PlayersPercept)
  
  REQUIRE(FieldInfo)
  REQUIRE(RobotPose)
END_DECLARE_MODULE(PerceptionsVisualizer)


class PerceptionsVisualizer: public PerceptionsVisualizerBase
{
public:
  PerceptionsVisualizer();
  ~PerceptionsVisualizer(){};
  
  virtual void execute();
  void execute(CameraInfo::CameraID id);

// double cam stuff
private:
  CameraInfo::CameraID cameraID;
  DOUBLE_CAM_PROVIDE(PerceptionsVisualizer,DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,BallPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,FieldPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,GoalPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,LinePercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,ScanLineEdgelPercept);

  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,Image);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,CameraInfo);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualizer,CameraMatrix);
};
#endif // end _PerceptionsVisualizer_h_