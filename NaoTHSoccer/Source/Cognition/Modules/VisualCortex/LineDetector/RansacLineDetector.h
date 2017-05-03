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
//#include "Representations/Perception/ScanLineEdgelPercept.h"

#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(RansacLineDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  //REQUIRE(ScanLineEdgelPercept)
  //REQUIRE(ScanLineEdgelPerceptTop)

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

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("RansacLineDetector")
    {
      PARAMETER_REGISTER(iterations) = 20;
      PARAMETER_REGISTER(outlierThreshold) = 70;
      PARAMETER_REGISTER(inlierMin) = 10;
      PARAMETER_REGISTER(directionSimilarity) = 0.8;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    int iterations;
    double outlierThreshold;
    int inlierMin;
    double directionSimilarity;
  } params;

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim(const Math::Line& line, const Edgel& edgel) const
  {
    double s = 0.0;
    if(line.getDirection()*edgel.direction > 0) {
      Vector2d v(edgel.point - line.getBase());
      v.rotateRight().normalize();
      s = 1.0-0.5*(fabs(line.getDirection()*v) + fabs(edgel.direction*v));
    }

    return s;
  }
};

#endif // RANSACLINEDETECTOR_H
