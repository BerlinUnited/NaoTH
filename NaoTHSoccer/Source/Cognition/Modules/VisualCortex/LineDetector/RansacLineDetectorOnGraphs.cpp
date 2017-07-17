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

  //TODO: Remove this after Debug
  std::vector<EdgelD> inlierList;

  for (const std::vector<EdgelD>& subgraph : getLineGraphPercept().lineGraphs) {
    Math::LineSegment result;
    if (ransac(result, subgraph, inlierList)) {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().lines.push_back(fieldLine);
    }
  }

  for (const std::vector<EdgelD>& subgraph : getLineGraphPercept().lineGraphsTop) {
    Math::LineSegment result;
    if (ransac(result, subgraph, inlierList)) {
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

    for(const EdgelD& e : inlierList) {
      CIRCLE(e.point.x, e.point.y, 25);
    }


  );

  DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:fit_and_draw_circle_field",
    FIELD_DRAWING_CONTEXT;
    // fit ellipse
    Ellipse circResult;

    if (ransacEllipse(circResult, getLineGraphPercept().lineGraphs)) {
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

    if (ransacEllipse(circResult, getLineGraphPercept().lineGraphsTop)) {
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

int RansacLineDetectorOnGraphs::ransac(Math::LineSegment& result, const std::vector<EdgelD>& subgraph, std::vector<EdgelD>& inlierList)
{
  if(subgraph.size() < params.inlierMin) {
    return 0;
  }

  Math::Line bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.iterations; ++i)
  {
    //pick two random points
    int i0 = Math::random((int)subgraph.size());
    int i1 = Math::random((int)subgraph.size());

    if(i0 == i1) {
      continue;
    }

    const EdgelD& a = subgraph[i0];
    const EdgelD& b = subgraph[i1];

    if(a.sim(b) < params.directionSimilarity) {
      continue;
    }

    Math::Line model(a.point, b.point-a.point);

    double inlierError = 0;
    int inlier = 0;

    for(const EdgelD& e : subgraph) {
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

    for(const EdgelD& e : subgraph)
    {
      double d = bestModel.minDistance(e.point);

      if(d < params.outlierThreshold && sim(bestModel, e) > params.directionSimilarity) {
        double t = bestModel.project(e.point);
        minT = std::min(t, minT);
        maxT = std::max(t, maxT);
        inlierList.push_back(e);
      }
    }
    // return results
    result = Math::LineSegment(bestModel.point(minT), bestModel.point(maxT));
  }

  return bestInlier;
}

int RansacLineDetectorOnGraphs::ransacEllipse(Ellipse& result, const std::vector<std::vector<EdgelD>>& graph)
{

  if(graph.empty()) {
    return 0;
  }

  Ellipse bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  // TODO: i < params.circle_iterations ?
  for(size_t i = 0; i < graph.size(); ++i)
  {
    if (graph[i].size() < 5) continue;
    // create model
    Ellipse ellipse;


    std::vector<double> x, y;
    x.reserve(graph[i].size());
    y.reserve(graph[i].size());
    for(const EdgelD& e : graph[i]) {
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

    for(const std::vector<EdgelD>& subgraph : graph) {
      for(const EdgelD& e : subgraph) {
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
