#ifndef RANSACLINEDETECTOR_H
#define RANSACLINEDETECTOR_H

#include <iostream>
#include <forward_list>

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Perception/LineGraphPercept.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

BEGIN_DECLARE_MODULE(RansacLineDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)

  REQUIRE(LineGraphPercept)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
 RansacLineDetector();
 ~RansacLineDetector();

 virtual void execute();

private:
  std::vector<size_t> outliers;
  int ransac(Math::LineSegment& result);
};

#endif // RANSACLINEDETECTOR_H
