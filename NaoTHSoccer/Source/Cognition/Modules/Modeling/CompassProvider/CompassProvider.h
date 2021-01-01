/* 
 * File:   CompassProvider.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _CompassProvider_H_
#define _CompassProvider_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"

#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/Math/Line.h"

// Representations
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Representations/Modeling/OdometryData.h"

#include "Representations/Modeling/ProbabilisticQuadCompas.h"
#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/LinePercept2018.h"

#include "Tools/DoubleCamHelpers.h"
#include <algorithm>
#include <set>

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(CompassProvider)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(OdometryData)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(LineGraphPercept)

  REQUIRE(RansacLinePercept)

  PROVIDE(ProbabilisticQuadCompas)
END_DECLARE_MODULE(CompassProvider)


class CompassProvider : private CompassProviderBase
{
public:
  CompassProvider();
  virtual ~CompassProvider();

  void execute();

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("CompassProvider")
    {
      PARAMETER_REGISTER(quadCompasSmoothingFactor) = 0.4;
      PARAMETER_REGISTER(minimalNumberOfPairs) = 0;

      syncWithConfig();
    }

    int minimalNumberOfPairs;
    double quadCompasSmoothingFactor;
  } params;


private: 
  //std::vector<Vector2d> edgelProjectionsBegin;
  //std::vector<Vector2d> edgelProjectionsEnd;

};

#endif  /* _CompassProvider_H_ */

