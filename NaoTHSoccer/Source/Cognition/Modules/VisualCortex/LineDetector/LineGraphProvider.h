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
#include "Representations/Perception/ScanGrid.h"

#include "Tools/DoubleCamHelpers.h"
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
  PROVIDE(DebugParameterList)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(ScanGrid)
  REQUIRE(ScanGridTop)

  REQUIRE(OdometryData)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  //PROVIDE(ProbabilisticQuadCompas)
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
    //getProbabilisticQuadCompas().reset();
    getLineGraphPercept().reset();

    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
    
    //getProbabilisticQuadCompas().normalize();



    DEBUG_REQUEST("Vision:LineGraphProvider:draw_extended_line_graph",
      FIELD_DRAWING_CONTEXT;
      int c = 0; 
      for(const std::vector<EdgelD>& subgraph : getLineGraphPercept().lineGraphs) {     

        PEN(ColorClasses::colorClassToHex((ColorClasses::Color)c),2);
        c = (c+1) % ColorClasses::numOfColors;

        for(size_t i = 0; i < subgraph.size(); i++) {
          const Vector2d& first = subgraph[i].point;
          CIRCLE( subgraph[i].point.x, subgraph[i].point.y, 25);

          if(i+1 < subgraph.size()) {
            const Vector2d& next = subgraph[i+1].point;
            LINE(first.x,first.y,next.x,next.y);
          }
        }
      }
    );
    DEBUG_REQUEST("Vision:LineGraphProvider:draw_extended_line_graph_top",
      FIELD_DRAWING_CONTEXT;
      int c = 0;
      for(const std::vector<EdgelD>& subgraph : getLineGraphPercept().lineGraphsTop) {

        PEN(ColorClasses::colorClassToHex((ColorClasses::Color)c),2);
        c = (c+1) % ColorClasses::numOfColors;

        for(size_t i = 0; i < subgraph.size(); i++) {
          const Vector2d& first = subgraph[i].point;
          CIRCLE( subgraph[i].point.x, subgraph[i].point.y, 25);

          if(i+1 < subgraph.size()) {
            const Vector2d& next = subgraph[i+1].point;
            LINE(first.x,first.y,next.x,next.y);
          }
        }
      }
    );
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("LineGraphProvider")
    {
      PARAMETER_REGISTER(edgelSimThreshold) = 0.8;
      PARAMETER_REGISTER(quadCompasSmoothingFactor) = 0.4;
      PARAMETER_REGISTER(minimalNumberOfPairs) = 0;
      PARAMETER_REGISTER(maximalProjectedLineWidth) = 30;

      // pixels, pairs must be apart from each other, to avoid aliasing
      PARAMETER_REGISTER(min_pair_pixel_distance) = 10;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    double edgelSimThreshold;
    double quadCompasSmoothingFactor;
    int minimalNumberOfPairs;
    int maximalProjectedLineWidth;

    int min_pair_pixel_distance;
  } params;

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

  std::vector<Vector2d> edgelProjectionsBegin;
  std::vector<Vector2d> edgelProjectionsEnd;

  std::vector<std::vector<int>> lineGraphsIds;

private: // method members
  static double edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2);

  void calculatePairsAndNeigbors(
    const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, 
    std::vector<EdgelPair>& edgelPairs, 
    std::vector<Neighbors>& neighbors, double threshold) const;

  void extendLineGraph(std::vector<Neighbors>& neighbors);

  DOUBLE_CAM_PROVIDE(LineGraphProvider, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraInfo);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraMatrix);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, ScanLineEdgelPercept);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, ScanGrid);
};

#endif  /* _LineGraphProvider_H_ */

