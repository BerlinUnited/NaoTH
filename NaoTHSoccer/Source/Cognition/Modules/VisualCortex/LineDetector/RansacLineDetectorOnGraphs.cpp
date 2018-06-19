#include "RansacLineDetectorOnGraphs.h"

RansacLineDetectorOnGraphs::RansacLineDetectorOnGraphs()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_edgels_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_lines_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:fit_and_draw_circle_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_circle_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetectorOnGraphs:draw_circle_field_top", "", false);

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

    for(const Edgel& e: getLineGraphPercept().edgelsOnField)
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

  for (std::vector<GraphEdgel>& subgraphEdgels: graphEdgels) {
    Math::LineSegment result;
    if (ransac(result, subgraphEdgels, getLineGraphPercept().lineGraphs)) {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().extended_lines.push_back(fieldLine);

      getLinePercept().short_lines.push_back(fieldLine);
    }
  }

  for (std::vector<GraphEdgel>& subgraphEdgels: graphEdgelsTop) {
    Math::LineSegment result;
    if (ransac(result, subgraphEdgels, getLineGraphPercept().lineGraphsTop)) {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePerceptTop().extended_lines.push_back(fieldLine);

      // Top and Bottom graphs are provided in Bottom Line Percept
      getLinePercept().short_lines.push_back(fieldLine);
    }
  }

  Ellipse circResult;

  getLinePercept().middleCircleWasSeen = false;

  if (ransacEllipse(circResult, graphEdgels, getLineGraphPercept().lineGraphs)) {
    getLinePercept().middleCircleWasSeen = true;

    double c[2];
    circResult.getCenter(c);
    getLinePercept().middleCircleCenter.x = c[0];
    getLinePercept().middleCircleCenter.y = c[1];

    DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:draw_circle_field",
      FIELD_DRAWING_CONTEXT;
      double a[2];
      circResult.axesLength(a);

      PEN("009900", 50);

      CIRCLE(c[0], c[1], 30);
      OVAL_ROTATED(c[0], c[1], a[0], a[1], circResult.rotationAngle());
    );
  }

  if (params.enable_ellipse_fitting
      && ransacEllipse(circResult, graphEdgelsTop, getLineGraphPercept().lineGraphsTop)) {
    getLinePercept().middleCircleWasSeen = true;

    double c[2];
    circResult.getCenter(c);
    getLinePercept().middleCircleCenter.x = c[0];
    getLinePercept().middleCircleCenter.y = c[1];

    DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:draw_circle_field_top",
      FIELD_DRAWING_CONTEXT;
      double a[2];
      circResult.axesLength(a);

      PEN("009900", 50);

      CIRCLE(c[0], c[1], 30);
      OVAL_ROTATED(c[0], c[1], a[0], a[1], circResult.rotationAngle());
    );
  }

  DEBUG_REQUEST("Vision:RansacLineDetectorOnGraphs:draw_lines_field",
    FIELD_DRAWING_CONTEXT;

    if (!getLinePercept().extended_lines.empty()) {
      for(const LinePercept::FieldLineSegment& line: getLinePercept().extended_lines)
      {
        PEN("FF0000", 30);
        LINE(
          line.lineOnField.begin().x, line.lineOnField.begin().y,
          line.lineOnField.end().x, line.lineOnField.end().y);
      }
    }
    if (!getLinePerceptTop().extended_lines.empty()) {
      for(const LinePercept::FieldLineSegment& line: getLinePerceptTop().extended_lines)
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

      for(int i=0; i < 100; i++) {
        double x_test = Math::random(c[0] - (a[0]*2), c[0] + (a[0]*2));
        double y_test = Math::random(c[1] - (a[1]*2), c[1] + (a[1]*2));


        if (circResult.mahalanobis_distance(x_test, y_test) < circResult.mahalanobis_radius()) {
          PEN("007700", 10);
        } else {
          PEN("770000", 10);
        }
        CIRCLE(x_test, y_test, 30);
      }
      /*
      for(size_t i=0; i<circResult.x_toFit.size(); i++) {
        std::cout << circResult.x_toFit[i] << ", ";
      }
      std::cout << std::endl;
      for(size_t i=0; i<circResult.y_toFit.size(); i++) {
        std::cout << circResult.y_toFit[i] << ", ";
      }
      std::cout << std::endl;
      for(size_t i=0; i<6; i++) {
        std::cout << circResult.params[i] << ", ";
      }
      std::cout << std::endl;
      */
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

      for(int i=0; i < 100; i++) {
        double x_test = Math::random(c[0] - (a[0]*2), c[0] + (a[0]*2));
        double y_test = Math::random(c[1] - (a[1]*2), c[1] + (a[1]*2));

        if (circResult.mahalanobis_distance(x_test, y_test) < circResult.mahalanobis_radius()) {
          PEN("007700", 10);
        } else {
          PEN("770000", 10);
        }
        CIRCLE(x_test, y_test, 30);
      }
      /*
      for(size_t i=0; i<circResult.x_toFit.size(); i++) {
        std::cout << circResult.x_toFit[i] << ", ";
      }
      std::cout << std::endl;
      for(size_t i=0; i<circResult.y_toFit.size(); i++) {
        std::cout << circResult.y_toFit[i] << ", ";
      }
      std::cout << std::endl;
      for(size_t i=0; i<6; i++) {
        std::cout << circResult.params[i] << ", ";
      }
      std::cout << std::endl;
      */
    }
  );
}

int RansacLineDetectorOnGraphs::ransac(Math::LineSegment& result, std::vector<GraphEdgel>& subgraphEdgels, const std::vector<std::vector<EdgelD>>& lineGraphs)
{
  if(static_cast<int>(subgraphEdgels.size()) < params.inlierMin) {
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
        ge.line_id = 1;
      }
    }
    // return results
    result = Math::LineSegment(bestModel.point(minT), bestModel.point(maxT));
  }

  return bestInlier;
}

int RansacLineDetectorOnGraphs::ransacEllipse(Ellipse& bestModel, std::vector<std::vector<GraphEdgel>>& graphEdgels, const std::vector<std::vector<EdgelD>>& graph)
{

  if(graphEdgels.empty()) {
    return 0;
  }

  int bestInlier = 0;
  int bestValidity = 0;
  double bestInlierError = 0;

  // TODO: i < params.circle_iterations ?
  for(size_t i = 0; i < graphEdgels.size(); ++i)
  {
    if (graphEdgels[i].size() < 5) continue;
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
    int validity = 0;

    for(const std::vector<GraphEdgel>& subgraphEdgels : graphEdgels) {
      for(const GraphEdgel& ge : subgraphEdgels) {
        const EdgelD& e = graph[ge.subgraph_id][ge.edgel_id];
        double err = ellipse.error_to(e.point.x, e.point.y);

        double r = ellipse.mahalanobis_radius();
        double d = ellipse.mahalanobis_distance(e.point.x, e.point.y);

        if((ge.line_id > -1) && (ge.subgraph_id != i) && (d < params.circle_line_threshold * r)) {
          //std::cout << "d: " << d << " r: " << r << " e: " << err << " !" << std::endl << std::endl;
          ++validity;
        }
        if(err <= params.circle_outlierThreshold) {
          ++inlier;
          inlierError += err;
          continue;
        }

      }
    }

    if(inlier >= params.circle_inlierMin && validity > bestValidity && (inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))) {
      bestModel = ellipse;
      bestValidity = validity;
      bestInlier = inlier;
      bestInlierError = inlierError;
    }
  }
  /*
  if (bestInlier && bestValidity) {
    std::cout << "Valid: " << bestValidity << "!" << std::endl << std::endl;
  }
  */
  return bestInlier;

}
