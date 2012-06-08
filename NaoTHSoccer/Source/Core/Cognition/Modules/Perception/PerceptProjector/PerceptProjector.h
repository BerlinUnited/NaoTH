/**
* @file PerceptProjector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PerceptProjector
*/

#ifndef _PerceptProjector_h_
#define _PerceptProjector_h_

#include <ModuleFramework/Module.h>



#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Modeling/GoalModel.h"

#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/BallPercept.h"

// Tools
#include "Tools/DataStructures/RingBufferWithSum.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PerceptProjector)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldInfo)
  REQUIRE(SensingGoalModel)
  
  PROVIDE(CameraMatrixOffset)
  PROVIDE(GoalPercept)
  PROVIDE(LinePercept)
  PROVIDE(BallPercept)
END_DECLARE_MODULE(PerceptProjector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class PerceptProjector: public PerceptProjectorBase
{
public:

  PerceptProjector();
  virtual ~PerceptProjector();

  /** executes the module */
  void execute();

private:

  /** */
  bool correct_the_line_percept(
    Vector2<double>& offset,
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;

  /** */
  double projectionErrorLine(
    double offsetX,
    double offsetY,
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;


  /** */
  RingBufferWithSum<Vector2<double>, 100> offsetBuffer;
};

#endif //__PerceptProjector_h_
