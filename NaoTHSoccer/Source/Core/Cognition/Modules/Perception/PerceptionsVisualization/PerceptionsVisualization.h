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

//module declaration section
BEGIN_DECLARE_MODULE(PerceptionsVisualization)
  REQUIRE(BallPercept)
  REQUIRE(PlayersPercept)
  REQUIRE(FieldPercept)
  REQUIRE(GoalPercept)
  REQUIRE(LinePercept)
  REQUIRE(ObjectPercept)
  REQUIRE(ScanLineEdgelPercept)

  REQUIRE(CameraMatrix)

  REQUIRE(FieldInfo)

  PROVIDE(Image)
END_DECLARE_MODULE(PerceptionsVisualization)

class PerceptionsVisualization: public PerceptionsVisualizationBase
{
public:
  PerceptionsVisualization();
  ~PerceptionsVisualization(){};
  virtual void execute();
};
#endif // end _PerceptionsVisualization_h_