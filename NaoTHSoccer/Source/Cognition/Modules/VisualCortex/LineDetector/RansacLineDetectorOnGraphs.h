#ifndef RANSACLINEDETECTORONGRAPHS_H
#define RANSACLINEDETECTORONGRAPHS_H

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
#include "Representations/Perception/LinePercept2018.h"

#include "Ellipse.h"

BEGIN_DECLARE_MODULE(RansacLineDetectorOnGraphs)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(LineGraphPercept)

  PROVIDE(ShortLinePercept)
  PROVIDE(GraphRansacCirclePercept)
  PROVIDE(GraphRansacCirclePerceptTop)
END_DECLARE_MODULE(RansacLineDetectorOnGraphs)

class RansacLineDetectorOnGraphs: public RansacLineDetectorOnGraphsBase
{
public:
 RansacLineDetectorOnGraphs();
 ~RansacLineDetectorOnGraphs();

 virtual void execute();

  struct GraphEdgel
  {
    size_t subgraph_id;
    size_t edgel_id;
    int line_id;
  };

  //TODO: Provide this!
  std::vector<std::vector<GraphEdgel>> graphEdgels;
  std::vector<std::vector<GraphEdgel>> graphEdgelsTop;

private:
  int ransac(Math::LineSegment& result, std::vector<GraphEdgel>& subgraphEdgels, const std::vector<std::vector<EdgelD>>& lineGraphs);

  int ransacEllipse(Ellipse& result, std::vector<std::vector<GraphEdgel>>& graphEdgels, const std::vector<std::vector<EdgelD>>& lineGraphs);

//private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("RansacLineDetectorOnGraphs")
    {
      //Lines
      PARAMETER_REGISTER(iterations) = 10;
      PARAMETER_REGISTER(outlierThreshold) = 40;
      PARAMETER_REGISTER(inlierMin) = 4;
      PARAMETER_REGISTER(directionSimilarity) = 0.8;
      PARAMETER_REGISTER(maxLines) = 11;

      //Circle
      PARAMETER_REGISTER(circle_iterations) = 20;
      PARAMETER_REGISTER(circle_outlierThreshold) = 70;
      PARAMETER_REGISTER(circle_inlierMin) = 10;
      PARAMETER_REGISTER(circle_degeneration) = 0.9;
      PARAMETER_REGISTER(circle_line_threshold) = 0.5;

      PARAMETER_REGISTER(enable_ellipse_fitting) = false;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }
    int iterations;
    double outlierThreshold;
    int inlierMin;
    double directionSimilarity;
    int maxLines;

    int circle_iterations;
    double circle_outlierThreshold;
    int circle_inlierMin;
    double circle_degeneration;
    double circle_line_threshold;
    bool enable_ellipse_fitting;

  } params;

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim(const Math::Line& line, const EdgelD& edgel) const
  {
    double s = 0.0;
    if(line.getDirection()*edgel.direction > 0) {
      Vector2d v(edgel.point - line.getBase());
      v.rotateRight().normalize();
      s = 1.0-0.5*(fabs(line.getDirection()*v) + fabs(edgel.direction*v));
    }

    return s;
  }

  //swaps v[i] with a random v[x], x in [0:i]
  size_t swap_random(std::vector<size_t> &v, int i) {
    int r = Math::random(i+1);
    std::swap(v[r], v[i]);

    return v[i];
  }
};

#endif // RANSACLINEDETECTORONGRAPHS_H
