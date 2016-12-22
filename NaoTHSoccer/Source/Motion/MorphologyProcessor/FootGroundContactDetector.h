/**
* @file FootGroundContactDetector.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#ifndef _FootGroundContactDetector_H
#define _FootGroundContactDetector_H


#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FSRData.h>
#include "Representations/Modeling/GroundContactModel.h"
#include <Representations/Infrastructure/FrameInfo.h>

// tools
#include "Tools/LinearClassifier.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"


BEGIN_DECLARE_MODULE(FootGroundContactDetector)

  REQUIRE(FrameInfo)
  REQUIRE(FSRData)

  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  PROVIDE(GroundContactModel)
END_DECLARE_MODULE(FootGroundContactDetector)


class FootGroundContactDetector: private FootGroundContactDetectorBase
{
public:
  FootGroundContactDetector();
  ~FootGroundContactDetector();

  void execute();

private:

  RingBufferWithSum<double, 10> leftFSRBuffer;
  RingBufferWithSum<double, 10> rightFSRBuffer;

  class Parameters: public ParameterList
  {
  public:

      Parameters(DebugParameterList& list) : ParameterList("FootGroundContactParameters"), list(&list)
    {
      // experimental - probably needs some calibration (while playing?)
      PARAMETER_REGISTER(left) = 0.3;
      PARAMETER_REGISTER(right) = 0.3;

      syncWithConfig();

      this->list->add(this);
    }

    ~Parameters()
    {
      list->remove(this);
    }

    DebugParameterList* list;

    double left;
    double right;
  };

  Parameters footParams;

};

#endif  /* _FootGroundContactDetector_H */

