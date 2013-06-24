/**
* @file PerceptionsVisualization.h
*
* @author Kirill Yasinovskiy <yasinovs@informatik.hu-berlin.de>
* Definition of class PerceptionsVizualization
*/

#ifndef _PerceptionsVisualization_h_
#define _PerceptionsVisualization_h_

//include section
#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>
//perceptions
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/ObjectPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Representations/Infrastructure/FieldInfo.h"

//common representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/CameraMatrixOffset.h"

#include "Tools/DoubleCamHelpers.h"

//module declaration section
BEGIN_DECLARE_MODULE(PerceptionsVisualization)
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
  
  PROVIDE(CameraMatrixOffset)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  PROVIDE(Image)
  PROVIDE(ImageTop)

  REQUIRE(ObjectPercept)
  REQUIRE(PlayersPercept)
  
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(PerceptionsVisualization)


class PerceptionsVisualization: public PerceptionsVisualizationBase
{
public:
  PerceptionsVisualization();
  ~PerceptionsVisualization(){};
  
  virtual void execute();
  void execute(CameraInfo::CameraID id);

// double cam stuff
private:
  CameraInfo::CameraID cameraID;
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,BallPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,FieldPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,GoalPercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,LinePercept);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,ScanLineEdgelPercept);

  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,Image);
  DOUBLE_CAM_REQUIRE(PerceptionsVisualization,CameraMatrix);
};
#endif // end _PerceptionsVisualization_h_