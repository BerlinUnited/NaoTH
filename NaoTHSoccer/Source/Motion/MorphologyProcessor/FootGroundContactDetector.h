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

  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  PROVIDE(FSRData)
  PROVIDE(GroundContactModel)
END_DECLARE_MODULE(FootGroundContactDetector)


class FootGroundContactDetector: private FootGroundContactDetectorBase
{
public:
  FootGroundContactDetector();
  ~FootGroundContactDetector();

  void execute();

private:

  RingBufferWithSum<double, 100> leftFSRBuffer;
  RingBufferWithSum<double, 100> rightFSRBuffer;

  class Parameters: public ParameterList
  {
  public:

      Parameters(DebugParameterList& list) : ParameterList("FootGroundContactParameters"), list(&list)
    {
      PARAMETER_REGISTER(left) = 3;
      PARAMETER_REGISTER(right) = 3;
      PARAMETER_REGISTER(invalid) = 100;

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
    double invalid;
  };

  Parameters footParams;

};

#endif  /* _FootGroundContactDetector_H */

