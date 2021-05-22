/*
 * File:   LineGraphProvider.cpp
 * Author: Heinrich Mellmann
 */

#include "LineGraphProvider.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Math/Geometry.h"

LineGraphProvider::LineGraphProvider()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:edgel_pairs", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:edgel_pairs_field", "mark the edgels pairs on the field", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_neighbors", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_neighbors_field", "mark the edgels on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:edgel_cluster", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_lines", "draw the esimated lines in the image", false);

  //DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_compas", "draw compas direcion based on the edgel directions", false);

  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_line_graph", "", false);

  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_extended_line_graph", "", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_extended_line_graph_top", "", false);

  getDebugParameterList().add(&params);
}


LineGraphProvider::~LineGraphProvider()
{
  getDebugParameterList().remove(&params);
}


void LineGraphProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  calculatePairsAndNeigbors(getScanLineEdgelPercept().pairs, edgelPairs, edgelNeighbors, params.edgelSimThreshold);


  // calculate the projection for all edgels
  edgelProjectionsBegin.resize(getScanLineEdgelPercept().pairs.size());
  edgelProjectionsEnd.resize(getScanLineEdgelPercept().pairs.size());
  for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];

    const Edgel& end = getScanLineEdgelPercept().edgels[pair.end];
    const Edgel& begin = getScanLineEdgelPercept().edgels[pair.begin];

    // NOTE: edgels are assumed to be always below horizon and so the projection should be allways valid
    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      end.point.x,
      end.point.y,
      0.0,
      edgelProjectionsEnd[i]);

    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      begin.point.x,
      begin.point.y,
      0.0,
      edgelProjectionsBegin[i]);

    //pair.projectedWidth = Vector2d(beginPointOnField - endPointOnField).abs();
  }

  // extend line graph
  lineGraphsIds.clear();
  extendLineGraph(edgelNeighbors);

  std::vector<std::vector<EdgelD>>& graph = (cameraID == CameraInfo::Top)?
        getLineGraphPercept().lineGraphsTop:
        getLineGraphPercept().lineGraphs;

  // add the graphs to the representation
  for(const std::vector<int>& subgraph : lineGraphsIds)
  {
    graph.emplace_back();
    for(size_t left=0, right=1; right < subgraph.size(); ++left, ++right) {

      const Vector2d& edgelLeft = (edgelProjectionsBegin[subgraph[left]] + edgelProjectionsEnd[subgraph[left]])*0.5;
      const Vector2d& edgelRight = (edgelProjectionsBegin[subgraph[right]] + edgelProjectionsEnd[subgraph[right]])*0.5;

      EdgelD edgel;
      edgel.point = Vector2d(edgelLeft + edgelRight)*0.5;
      edgel.direction = (edgelRight - edgelLeft).normalize();

      graph.back().push_back(edgel);
    }
  }

  // calculate the LineGraphPercept
  for(size_t j = 0; j < edgelPairs.size(); j++)
  {
    const EdgelPair& edgelPair = edgelPairs[j];
    const Vector2d& edgelLeft = (edgelProjectionsBegin[edgelPair.left] + edgelProjectionsEnd[edgelPair.left])*0.5;
    const Vector2d& edgelRight = (edgelProjectionsBegin[edgelPair.right] + edgelProjectionsEnd[edgelPair.right])*0.5;

    const double projectedWidthLeft = (edgelProjectionsBegin[edgelPair.left] - edgelProjectionsEnd[edgelPair.left]).abs();
    const double projectedWidthRight = (edgelProjectionsBegin[edgelPair.right] - edgelProjectionsEnd[edgelPair.right]).abs();

    //TODO: should this be double?
    Edgel edgel;
    edgel.point = Vector2d(edgelLeft + edgelRight)*0.5;
    edgel.direction = (edgelRight - edgelLeft).normalize(); // is it correct?

    //const ScanLineEdgelPercept::EdgelPair& el = getScanLineEdgelPercept().pairs[edgelPair.left];
    //const ScanLineEdgelPercept::EdgelPair& er = getScanLineEdgelPercept().pairs[edgelPair.right];

    if(projectedWidthLeft > params.maximalProjectedLineWidth && projectedWidthRight > params.maximalProjectedLineWidth) {
      getLineGraphPercept().edgelsOnField.push_back(edgel);

      DEBUG_REQUEST("Vision:LineGraphProvider:edgel_pairs_field",
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000",2);
        CIRCLE(edgel.point.x, edgel.point.y, 12);
        PEN("000000",2);
        LINE(edgelLeft.x, edgelLeft.y, edgelRight.x, edgelRight.y);
      );

      DEBUG_REQUEST("Vision:LineGraphProvider:draw_line_graph",
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000",2);
        CIRCLE( edgel.point.x, edgel.point.y, 25);

        PEN("0000FF",2);
        CIRCLE( edgelProjectionsBegin[edgelPair.left].x, edgelProjectionsBegin[edgelPair.left].y, 10);
        PEN("FF0000",2);
        CIRCLE( edgelProjectionsEnd[edgelPair.left].x, edgelProjectionsEnd[edgelPair.left].y, 10);
        PEN("000000",2);
        LINE( edgelProjectionsBegin[edgelPair.left].x, edgelProjectionsBegin[edgelPair.left].y, edgelProjectionsEnd[edgelPair.left].x, edgelProjectionsEnd[edgelPair.left].y);

        PEN("0000FF",2);
        CIRCLE( edgelProjectionsBegin[edgelPair.right].x, edgelProjectionsBegin[edgelPair.right].y, 10);
        PEN("FF0000",2);
        CIRCLE( edgelProjectionsEnd[edgelPair.right].x, edgelProjectionsEnd[edgelPair.right].y, 10);
        PEN("000000",2);
        LINE( edgelProjectionsBegin[edgelPair.right].x, edgelProjectionsBegin[edgelPair.right].y, edgelProjectionsEnd[edgelPair.right].x, edgelProjectionsEnd[edgelPair.right].y);
      );
    }
  }



  // neighbors line graph in image
  for(size_t i = 0; i < edgelNeighbors.size(); i++)
  {
    bool add_to_percept = false;

    if(edgelNeighbors[i].left != -1) {
      add_to_percept = true;
    }

    if(edgelNeighbors[i].right != -1) {
      add_to_percept = true;
    }

    if(add_to_percept) {
      // TODO: some information might get lost here while copying from EdgelPair to Edgel
      if(cameraID == CameraInfo::Top) {
        getLineGraphPercept().edgelsInImageTop.push_back(getScanLineEdgelPercept().pairs[i]);
      } else {
        getLineGraphPercept().edgelsInImage.push_back(getScanLineEdgelPercept().pairs[i]);
      }
    }
  }
  /*
  // fill the compas
  if((int)edgelPairs.size() > parameters.minimalNumberOfPairs)
  {
    getProbabilisticQuadCompas().setSmoothing(parameters.quadCompasSmoothingFactor);
    for(size_t j = 0; j < edgelPairs.size(); j++)
    {
      const EdgelPair& edgelPair = edgelPairs[j];
      //const Vector2d& edgelLeft = edgelProjections[edgelPair.left];
      //const Vector2d& edgelRight = edgelProjections[edgelPair.right];

      // TODO: mean difference?
      double r = (edgelProjectionsBegin[edgelPair.left] - edgelProjectionsBegin[edgelPair.right]).angle();
      getProbabilisticQuadCompas().add(r, edgelPair.sim);
    }
  }


  DEBUG_REQUEST("Vision:LineGraphProvider:draw_compas",
    if(cameraID == CameraInfo::Top) {
      getProbabilisticQuadCompas().normalize();

      Vector2d last;
      double last_v = 0;

      double scale = 5000;
      double offset = 150;

      FIELD_DRAWING_CONTEXT;
      for(unsigned int x = 0; x < getProbabilisticQuadCompas().size()*4+1; x++)
      {
        double v = getProbabilisticQuadCompas()[x];
        Vector2d a(offset + v*scale, 0.0);
        a.rotate(Math::fromDegrees(x*5));
        if(x > 0) {

          double d = Math::clamp(std::min(v, last_v)/0.1, 0.0, 1.0);
          Color c(d, 0.0, 1-d);
          PEN(c, 10);

          LINE(last.x,last.y,a.x,a.y);
        }
        last = a;
        last_v = v;
      }
    }
  );
  */

  DEBUG_REQUEST("Vision:LineGraphProvider:edgel_pairs",
    CANVAS(((cameraID == CameraInfo::Top) ? "ImageTop" : "ImageBottom"));
  for (size_t i = 0; i < edgelPairs.size(); i++)
  {
    const EdgelPair& pair = edgelPairs[i];
    const ScanLineEdgelPercept::EdgelPair& edgelOne = getScanLineEdgelPercept().pairs[pair.left];
    const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[pair.right];

    PEN("000000", 0.1);
    //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
    LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
    PEN("FF0000", 0.1);
    CIRCLE(edgelOne.point.x, edgelOne.point.y, 3);
    PEN("0000FF", 0.1);
    CIRCLE(edgelTwo.point.x, edgelTwo.point.y, 2);
  }
  );


  DEBUG_REQUEST("Vision:LineGraphProvider:draw_neighbors",
    CANVAS(((cameraID == CameraInfo::Top) ? "ImageTop" : "ImageBottom"));
  for (size_t i = 0; i < edgelNeighbors.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
    bool draw_node = false;

    if (edgelNeighbors[i].left != -1) {
      const ScanLineEdgelPercept::EdgelPair& edgel_left = getScanLineEdgelPercept().pairs[edgelNeighbors[i].left];
      PEN("000000", 0.1);
      LINE(edgel.point.x, edgel.point.y, edgel_left.point.x, edgel_left.point.y);
      draw_node = true;
    }

    if (edgelNeighbors[i].right != -1) {
      const ScanLineEdgelPercept::EdgelPair& edgel_right = getScanLineEdgelPercept().pairs[edgelNeighbors[i].right];
      PEN("0000FF", 0.1);
      LINE(edgel.point.x, edgel.point.y, edgel_right.point.x, edgel_right.point.y);
      draw_node = true;
    }

    if (draw_node)
    {
      PEN("00FF00", 0.1);
      Vector2d pointOnField;
      if (CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), getCameraInfo(),
        edgel.point,
        0.0,
        pointOnField))
      {
        //if(pointOnField.abs() < 2500) {
        if (edgelNeighbors[i].left != -1) {
          const int pairIdx = edgelNeighbors[i].left;
          //const ScanLineEdgelPercept::EdgelPair& el = getScanLineEdgelPercept().pairs[pairIdx];

          const double projectedWidthLeft = (edgelProjectionsBegin[pairIdx] - edgelProjectionsEnd[pairIdx]).abs();
          if (projectedWidthLeft < 30) {
            PEN("FF0000", 0.1);
          }
          CIRCLE(edgel.point.x, edgel.point.y, projectedWidthLeft / 20);
        }
        if (edgelNeighbors[i].right != -1) {
          const int pairIdx = edgelNeighbors[i].right;
          //const ScanLineEdgelPercept::EdgelPair& er = getScanLineEdgelPercept().pairs[pairIdx];

          const double projectedWidthRight = (edgelProjectionsBegin[pairIdx] - edgelProjectionsEnd[pairIdx]).abs();
          if (projectedWidthRight < 30) {
            PEN("FF0000", 0.1);
          }
          CIRCLE(edgel.point.x, edgel.point.y, projectedWidthRight / 20);
        }
        //}
        //else {
        //  PEN("0000FF",0.1);
        //}
      }
      PEN("000000", 0.1);
      CIRCLE(edgel.point.x, edgel.point.y, 1);
    }
  }
  );

  DEBUG_REQUEST("Vision:LineGraphProvider:draw_neighbors_field",
    FIELD_DRAWING_CONTEXT;
  for (size_t i = 0; i < edgelNeighbors.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
    bool draw_node = false;

    Vector2d pointOnFieldEdgel;
    if (!CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      edgel.point,
      0.0,
      pointOnFieldEdgel))
    {
      continue;
    }

    if (edgelNeighbors[i].left != -1) {
      const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[edgelNeighbors[i].left];

      Vector2d pointOnField;
      if (CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), getCameraInfo(),
        edgel_s.point,
        0.0,
        pointOnField))
      {
        PEN("000000", 1);
        LINE(pointOnFieldEdgel.x, pointOnFieldEdgel.y, pointOnField.x, pointOnField.y);
        draw_node = true;
      }
    }

    if (edgelNeighbors[i].right != -1) {
      const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[edgelNeighbors[i].right];
      Vector2d pointOnField;
      if (CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), getCameraInfo(),
        edgel_s.point,
        0.0,
        pointOnField))
      {
        PEN("000000", 1);
        LINE(pointOnFieldEdgel.x, pointOnFieldEdgel.y, pointOnField.x, pointOnField.y);
        draw_node = true;
      }
    }

    if (draw_node) {
      PEN("FF0000", 1);
      CIRCLE(pointOnFieldEdgel.x, pointOnFieldEdgel.y, 25);
    }
  }
  );

}//end execute

void LineGraphProvider::extendLineGraph(std::vector<Neighbors>& neighbors) {
  std::vector<bool> processed(neighbors.size(), false);

  for (size_t i=0; i<processed.size(); i++) {
    if(processed[i] ||
        (neighbors[i].left != -1 && neighbors[neighbors[i].left].right == static_cast<int>(i))) // not a begin of a graph
    {
      continue;
    }

    std::vector<int> subGraph;

    int in = static_cast<int>(i);
    do {
      subGraph.push_back(in);
      processed[in] = true;
    } while((in = neighbors[in].right) > -1);

    if (subGraph.size() >= 2) {
      lineGraphsIds.push_back(subGraph);
    }
  }
}

double LineGraphProvider::edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2)
{
  double s = 0.0;
  if(e1.direction*e2.direction > 0) {
    Vector2d v = (e2.point - e1.point).rotateRight().normalize();
    s = 1.0-0.5*(fabs(e1.direction*v) + fabs(e2.direction*v));
  }

  return s;
}

void LineGraphProvider::calculatePairsAndNeigbors(
  const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels,
  std::vector<EdgelPair>& pairs,
  std::vector<Neighbors>& neighbors, double threshold) const
{
  neighbors.resize(edgels.size());
  pairs.clear();

  // TODO: this is rather inefficient
  for(unsigned int i = 0; i < edgels.size(); i++) {
    neighbors[i].reset();
  }

  for(unsigned int i = 0; i < edgels.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& edgelOne = edgels[i];

    if(!getScanLineEdgelPercept().endPoints.empty() &&
       getScanLineEdgelPercept().endPoints[edgelOne.id].posInImage.y > edgelOne.point.y)
    {
      continue;
    }

    int j_max = -1;
    double s_max = 0.0;

    if(edgelOne.adaptive) {
      // this edgel comes from vertical adaptive scanlines,
      // the search for a partner needs to be different.

      int next_idx = -1;
      int min_y = 0;

      for(size_t j = i+1; j < edgels.size(); j++)
      {
        const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

        if(!edgelTwo.adaptive) {
          // only check adaptive edgels with adaptive edgels
          break;
        }

        ASSERT(edgelTwo.id >= edgelOne.id);

        if(edgelTwo.point.x - edgelOne.point.x < params.min_pair_pixel_distance) {
          // edgels should be at least min_pair_pixel_distance apart from each other, to avoid aliasing
          continue;
        }

        if(edgelTwo.id != next_idx) {
          if(next_idx != -1) {
            // calculate next min_y from old next scanline
            ASSERT(next_idx < static_cast<int>(getScanGrid().vertical.size()));
            size_t min_y_idx = getScanGrid().vertical[next_idx].bottom;
            min_y = std::max(min_y, getScanGrid().vScanPattern[min_y_idx]);
          }
          ASSERT(edgelTwo.id > next_idx);
          next_idx = edgelTwo.id;
        }

        if(edgelTwo.point.y <= min_y) {
          // we already checked below min y
          continue;
        }

        double s = edgelSim(edgelOne, edgelTwo);

        if(s > s_max) {
          s_max = s;
          j_max = (int)j;
        }

        // update neigbors
        if(s > threshold)
        {
          int idx_left = edgelTwo.id < edgelOne.id ? (int)j : (int)i;
          int idx_right = edgelTwo.id < edgelOne.id ? (int)i : (int)j;

          Neighbors& left_node = neighbors[idx_left];
          Neighbors& right_node = neighbors[idx_right];

          // set the left neighbor for the right node
          if(right_node.left == -1 || right_node.w_left < s) {
            right_node.left = idx_left;
            right_node.w_left = s;
          }

          // set the right neighbor for the left node
          if(left_node.right == -1 || left_node.w_right < s) {
            left_node.right = idx_right;
            left_node.w_right = s;
          }
        }
      }//end for j

    } else {
      // normal edgel
      for(size_t j = i+1; j < edgels.size(); j++)
      {
        const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

        if(!getScanLineEdgelPercept().endPoints.empty() &&
           getScanLineEdgelPercept().endPoints[edgelTwo.id].posInImage.y > edgelTwo.point.y)
        {
          continue;
        }

        if(edgelTwo.id == edgelOne.id + 1 || edgelOne.id == edgelTwo.id + 1)
        {
          double s = edgelSim(edgelOne, edgelTwo);

          if(s > s_max) {
            s_max = s;
            j_max = (int)j;
          }

          // update neigbors
          if(s > threshold)
          {
            int idx_left = edgelTwo.id < edgelOne.id ? (int)j : (int)i;
            int idx_right = edgelTwo.id < edgelOne.id ? (int)i : (int)j;

            Neighbors& left_node = neighbors[idx_left];
            Neighbors& right_node = neighbors[idx_right];

            // set the left neighbor for the right node
            if(right_node.left == -1 || right_node.w_left < s) {
              right_node.left = idx_left;
              right_node.w_left = s;
            }

            // set the right neighbor for the left node
            if(left_node.right == -1 || left_node.w_right < s) {
              left_node.right = idx_right;
              left_node.w_right = s;
            }
          }
        }
      }//end for j

    }

    if(j_max > -1 && s_max > threshold) {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j_max];
      if(edgelOne.id < edgelTwo.id) {
        pairs.push_back(EdgelPair(i,j_max,s_max));
      } else {
        pairs.push_back(EdgelPair(j_max,i,s_max));
      }
    }
  }//end for i
}
