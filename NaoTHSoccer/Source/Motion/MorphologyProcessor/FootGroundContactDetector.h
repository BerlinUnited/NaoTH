/**
* @file FootGroundContactDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:yigit.akcay@icloud.com">Yigit, Akcay</a>
* detect if the foot touch the ground
*/

#ifndef _FootGroundContactDetector_H_
#define _FootGroundContactDetector_H_


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

    Parameters() : ParameterList("FootGroundContactParameters")
    {
      // experimental - probably needs some calibration (while playing?)
      PARAMETER_REGISTER(left) = 0.1;
      PARAMETER_REGISTER(right) = 0.1;
      PARAMETER_REGISTER(useMaxMedian) = false;

      syncWithConfig();
    }

    double left;
    double right;
    bool useMaxMedian;
  };

  Parameters footParams;
};

#endif  /* _FootGroundContactDetector_H */

