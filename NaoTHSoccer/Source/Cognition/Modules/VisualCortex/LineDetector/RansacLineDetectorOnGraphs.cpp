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

