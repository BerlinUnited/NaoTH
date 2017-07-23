#include "RansacLineDetectorOnGraphs.h"

RansacLineDetectorOnGraphs::RansacLineDetectorOnGraphs()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_edgels_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_lines_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:fit_and_draw_circle_field", "", false);

  getDebugParameterList().add(&params);
}

RansacLineDetectorOnGraphs::~RansacLineDetectorOnGraphs()
{
  getDebugParameterList().remove(&params);
}

void RansacLineDetectorOnGraphs::execute()
{
  getLinePercept().reset();
  getLinePerceptTop().reset();

  DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:draw_edgels_field",
    FIELD_DRAWING_CONTEXT;

    for(const Edgel& e: getLineGraphPercept().edgels)
    {
      PEN("FF0000",2);

      if(e.point.x > 0 || e.point.y > 0) {
        CIRCLE(e.point.x, e.point.y, 25);
      }
      PEN("000000",0.1);
      LINE(e.point.x, e.point.y, e.point.x + e.direction.x*100.0, e.point.y + e.direction.y*100.0);
    }
  );

  graphEdgels.clear();
  graphEdgelsTop.clear();
  for (size_t subgraph_id = 0; subgraph_id<getLineGraphPercept().lineGraphs.size(); subgraph_id++) {
    std::vector<GraphEdgel> subgraphEdgels;
    subgraphEdgels.reserve(getLineGraphPercept().lineGraphs[subgraph_id].size());
    for (size_t edgel_id = 0; edgel_id<getLineGraphPercept().lineGraphs[subgraph_id].size(); edgel_id++) {
      GraphEdgel graphEdgel;
      graphEdgel.subgraph_id = subgraph_id;
      graphEdgel.edgel_id = edgel_id;
      graphEdgel.line_id = -1;
      subgraphEdgels.push_back(graphEdgel);
    }
    graphEdgels.push_back(subgraphEdgels);
  }
  for (size_t subgraph_id = 0; subgraph_id<getLineGraphPercept().lineGraphsTop.size(); subgraph_id++) {
    std::vector<GraphEdgel> subgraphEdgels;
    subgraphEdgels.reserve(getLineGraphPercept().lineGraphsTop[subgraph_id].size());
    for (size_t edgel_id = 0; edgel_id<getLineGraphPercept().lineGraphsTop[subgraph_id].size(); edgel_id++) {
      GraphEdgel graphEdgel;
      graphEdgel.subgraph_id = subgraph_id;
      graphEdgel.edgel_id = edgel_id;
      graphEdgel.line_id = -1;
      subgraphEdgels.push_back(graphEdgel);
    }
    graphEdgelsTop.push_back(subgraphEdgels);
  }

  //TODO: Remove this after Debug
  std::vector<EdgelD> inlierList;

  for (std::vector<GraphEdgel>& subgraphEdgels: graphEdgels) {
    Math::LineSegment result;
    if (ransac(result, subgraphEdgels, getLineGraphPercept().lineGraphs, inlierList)) {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().lines.push_back(fieldLine);
    }
  }

  for (std::vector<GraphEdgel>& subgraphEdgels: graphEdgelsTop) {
    Math::LineSegment result;
    if (ransac(result, subgraphEdgels, getLineGraphPercept().lineGraphsTop, inlierList)) {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePerceptTop().lines.push_back(fieldLine);
    }
  }

  DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:draw_lines_field",
    FIELD_DRAWING_CONTEXT;

    if (!getLinePercept().lines.empty()) {
      for(const LinePercept::FieldLineSegment& line: getLinePercept().lines)
      {
        PEN("FF0000", 30);
        LINE(
          line.lineOnField.begin().x, line.lineOnField.begin().y,
          line.lineOnField.end().x, line.lineOnField.end().y);
      }
    }
    if (!getLinePerceptTop().lines.empty()) {
      for(const LinePercept::FieldLineSegment& line: getLinePerceptTop().lines)
      {
        PEN("0000FF", 30);
        LINE(
          line.lineOnField.begin().x, line.lineOnField.begin().y,
          line.lineOnField.end().x, line.lineOnField.end().y);
      }
    }

    for(const std::vector<GraphEdgel>& subgraphEdgels : graphEdgels) {
      for (const GraphEdgel& ge : subgraphEdgels) {
        if(ge.line_id > -1) {
          PEN("00FF00", 5);
        } else {
          PEN("FF0000", 5);
        }
        const EdgelD& e = getLineGraphPercept().lineGraphs[ge.subgraph_id][ge.edgel_id];
        CIRCLE(e.point.x, e.point.y, 25);
      }
    }
    for(const std::vector<GraphEdgel>& subgraphEdgels : graphEdgelsTop) {
      for (const GraphEdgel& ge : subgraphEdgels) {
        if(ge.line_id > -1) {
          PEN("00FF00", 5);
        } else {
          PEN("FF0000", 5);
        }
        const EdgelD& e = getLineGraphPercept().lineGraphsTop[ge.subgraph_id][ge.edgel_id];
        CIRCLE(e.point.x, e.point.y, 25);
      }
    }
  );

  DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:fit_and_draw_circle_field",
    FIELD_DRAWING_CONTEXT;
    // fit ellipse
    Ellipse circResult;

    if (ransacEllipse(circResult, graphEdgels, getLineGraphPercept().lineGraphs)) {
      double c[2];
      circResult.getCenter(c);

      double a[2];
      circResult.axesLength(a);

      PEN("009900", 50);

      CIRCLE(c[0], c[1], 30);
      OVAL_ROTATED(c[0], c[1], a[0], a[1], circResult.rotationAngle());

      PEN("0000AA", 20);
      for(size_t i=0; i<circResult.x_toFit.size(); i++) {
        CIRCLE(circResult.x_toFit[i], circResult.y_toFit[i], 20);
      }
    }

    if (ransacEllipse(circResult, graphEdgelsTop, getLineGraphPercept().lineGraphsTop)) {
      double c[2];
      circResult.getCenter(c);

      double a[2];
      circResult.axesLength(a);

      PEN("009900", 50);

      CIRCLE(c[0], c[1], 30);
      OVAL_ROTATED(c[0], c[1], a[0], a[1], circResult.rotationAngle());

      PEN("0000AA", 20);
      for(size_t i=0; i<circResult.x_toFit.size(); i++) {
        CIRCLE(circResult.x_toFit[i], circResult.y_toFit[i], 20);
      }
    }
  );
}

int RansacLineDetectorOnGraphs::ransac(Math::LineSegment& result, std::vector<GraphEdgel>& subgraphEdgels, const std::vector<std::vector<EdgelD>>& lineGraphs, std::vector<EdgelD>& inlierList)
{
  if(subgraphEdgels.size() < params.inlierMin) {
    return 0;
  }

  Math::Line bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.iterations; ++i)
  {
    //pick two random points
    int i0 = Math::random((int)subgraphEdgels.size());
    int i1 = Math::random((int)subgraphEdgels.size());

    if(i0 == i1) {
      continue;
    }

    const GraphEdgel& gEdgel1 = subgraphEdgels[i0];
    const GraphEdgel& gEdgel2 = subgraphEdgels[i1];
    const EdgelD& a = lineGraphs[gEdgel1.subgraph_id][gEdgel1.edgel_id];
    const EdgelD& b = lineGraphs[gEdgel2.subgraph_id][gEdgel2.edgel_id];

    if(a.sim(b) < params.directionSimilarity) {
      continue;
    }

    Math::Line model(a.point, b.point-a.point);

    double inlierError = 0;
    int inlier = 0;

    for(const GraphEdgel& ge : subgraphEdgels) {
      const EdgelD& e = lineGraphs[ge.subgraph_id][ge.edgel_id];
      double d = model.minDistance(e.point);

      // inlier
      if(d < params.outlierThreshold && sim(model, e) > params.directionSimilarity) {
        ++inlier;
        inlierError += d;
      }
    }

    if(inlier >= params.inlierMin && (inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))) {
      bestModel = model;
      bestInlier = inlier;
      bestInlierError = inlierError;
    }
  }

  if(bestInlier) {
    double minT = 0;
    double maxT = 0;

    for(GraphEdgel& ge : subgraphEdgels)
    {
      const EdgelD& e = lineGraphs[ge.subgraph_id][ge.edgel_id];

      double d = bestModel.minDistance(e.point);

      if(d < params.outlierThreshold && sim(bestModel, e) > params.directionSimilarity) {
        double t = bestModel.project(e.point);
        minT = std::min(t, minT);
        maxT = std::max(t, maxT);
        inlierList.push_back(e);
        ge.line_id = 1;
      }
    }
    // return results
    result = Math::LineSegment(bestModel.point(minT), bestModel.point(maxT));
  }

  return bestInlier;
}

int RansacLineDetectorOnGraphs::ransacEllipse(Ellipse& result, std::vector<std::vector<GraphEdgel>>& graphEdgels, const std::vector<std::vector<EdgelD>>& graph)
{

  if(graphEdgels.empty()) {
    return 0;
  }

  Ellipse bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  // TODO: i < params.circle_iterations ?
  for(size_t i = 0; i < graphEdgels.size(); ++i)
  {
    if (graph[i].size() < 5) continue;
    // create model
    Ellipse ellipse;


    std::vector<double> x, y;
    x.reserve(graphEdgels[i].size());
    y.reserve(graphEdgels[i].size());
    for(const GraphEdgel& ge : graphEdgels[i]) {
      const EdgelD& e = graph[ge.subgraph_id][ge.edgel_id];
      x.push_back(e.point.x);
      y.push_back(e.point.y);
    }

    ellipse.fitPoints(x,y);

    // check aspect ratio
    double a[2];
    ellipse.axesLength(a);
    if ( ((a[0] <= a[1]) && (a[0]/a[1] < params.circle_degeneration)) ||  ((a[1] <= a[0]) && (a[1]/a[0] < params.circle_degeneration))) {
      continue;
    }

    // check model
    double inlierError = 0;
    int inlier = 0;

    for(const std::vector<GraphEdgel>& subgraphEdgels : graphEdgels) {
      for(const GraphEdgel& ge : subgraphEdgels) {
        const EdgelD& e = graph[ge.subgraph_id][ge.edgel_id];
        double d = ellipse.error_to(e.point.x, e.point.y);

        if(d <= params.circle_outlierThreshold) {
          ++inlier;
          inlierError += d;
          continue;
        }
      }
    }

    if(inlier >= params.circle_inlierMin && (inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))) {
      bestModel = ellipse;
      bestInlier = inlier;
      bestInlierError = inlierError;
    }
  }

  if (bestInlier) {
    result = bestModel;
  }

  return bestInlier;

}
