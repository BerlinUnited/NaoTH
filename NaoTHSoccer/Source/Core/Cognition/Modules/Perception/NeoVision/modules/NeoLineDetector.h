/* 
 * File:   NeoLineDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _NeoLineDetector_H_
#define _NeoLineDetector_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/ImageProcessing/Edgel.h"


// Representations
#include "Representations/Perception/ScanLineEdgelPercept.h"


#include <Tools/DataStructures/ParameterList.h>


BEGIN_DECLARE_MODULE(NeoLineDetector)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(NeoLineDetector)


class NeoLineDetector : private NeoLineDetectorBase
{
public:
  NeoLineDetector();
  virtual ~NeoLineDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);

    CANVAS_PX_BOTTOM;
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NeoLineDetector")
    {
      PARAMETER_REGISTER(x) = 6;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int x;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;
  

  const ScanLineEdgelPercept& getScanLineEdgelPercept()
  {
    if(cameraID == CameraInfo::Top) {
      return NeoLineDetectorBase::getScanLineEdgelPerceptTop();
    } else {
      return NeoLineDetectorBase::getScanLineEdgelPercept();
    }
  }

};

#endif  /* _NeoLineDetector_H_ */

