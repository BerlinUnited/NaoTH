/* 
 * File:   LineGraphProvider.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _LineGraphProvider_H_
#define _LineGraphProvider_H_

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

#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/ParameterList.h>
//#include "Tools/naoth_opencv.h"
#include <algorithm>
#include <set>

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(LineGraphProvider)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(OdometryData)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(ProbabilisticQuadCompas)
  PROVIDE(LineGraphPercept)
END_DECLARE_MODULE(LineGraphProvider)


class LineGraphProvider : private LineGraphProviderBase
{
public:
  LineGraphProvider();
  virtual ~LineGraphProvider();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    getProbabilisticQuadCompas().reset();
    getLineGraphPercept().edgels.clear();

    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
    
    getProbabilisticQuadCompas().normalize();
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("LineGraphProvider")
    {
      PARAMETER_REGISTER(edgelSimThreshold) = 0.8;
      PARAMETER_REGISTER(quadCompasSmoothingFactor) = 0.4;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    double edgelSimThreshold;
    double quadCompasSmoothingFactor;
  } parameters;

  struct Neighbors {
    Neighbors():left(-1), right(-1), w_left(0), w_right(0){}
    int left;
    int right;
    double w_left;
    double w_right;
    int operator[](int i){ return i==0?left:right; }
    void reset() {left = right= -1; w_left = w_right = 0; }
  };
  
  struct EdgelPair {
    int left;
    int right;
    double sim;
    EdgelPair(int left, int right, double sim) : left(left), right(right), sim(sim) {}
  };

private: // data members
  CameraInfo::CameraID cameraID;

  std::vector<Neighbors> edgelNeighbors;
  std::vector<EdgelPair> edgelPairs;
  std::vector<Vector2d> edgelProjections;

private: // method members
  static double edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2);

  void calculatePairsAndNeigbors(
    const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, 
    std::vector<EdgelPair>& edgelPairs, 
    std::vector<Neighbors>& neighbors, double threshold) const;

  DOUBLE_CAM_PROVIDE(LineGraphProvider, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraInfo);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraMatrix);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, ScanLineEdgelPercept);
};

#endif  /* _LineGraphProvider_H_ */

