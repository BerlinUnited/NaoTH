/**
* @file PerceptProjector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PerceptProjector
*/

#ifndef __PerceptProjector_h_
#define __PerceptProjector_h_

#include <ModuleFramework/Module.h>



#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/BallPercept.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PerceptProjector)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)

  PROVIDE(GoalPercept)
  PROVIDE(LinePercept)
  PROVIDE(BallPercept)
END_DECLARE_MODULE(PerceptProjector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class PerceptProjector: public PerceptProjectorBase
{
public:

  PerceptProjector();
  ~PerceptProjector();

  /** executes the module */
  void execute();

private:
  

  void correct_the_line_percept(
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;

  double projectionErrorLine(
    double offsetX,
    double offsetY,
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;


};

#endif //__PerceptProjector_h_
