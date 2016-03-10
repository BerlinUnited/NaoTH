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
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_neighbors", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_neighbors_field", "mark the edgels on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:edgel_cluster", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_lines", "draw the esimated lines in the image", false);

  DEBUG_REQUEST_REGISTER("Vision:LineGraphProvider:draw_compas", "draw compas direcion based on the edgel directions", false);

  getDebugParameterList().add(&parameters);
}


LineGraphProvider::~LineGraphProvider()
{
  getDebugParameterList().remove(&parameters);
}


void LineGraphProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  calculatePairsAndNeigbors(getScanLineEdgelPercept().pairs, edgelPairs, edgelNeighbors, parameters.edgelSimThreshold);

  DEBUG_REQUEST("Vision:LineGraphProvider:edgel_pairs",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < edgelPairs.size(); i++)
    {
      const EdgelPair& pair = edgelPairs[i];
      const ScanLineEdgelPercept::EdgelPair& edgelOne = getScanLineEdgelPercept().pairs[pair.left];
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[pair.right];

      PEN("000000",0.1);
      //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
      LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      PEN("FF0000",0.1);
      CIRCLE( edgelOne.point.x, edgelOne.point.y, 3);
      PEN("0000FF",0.1);
      CIRCLE( edgelTwo.point.x, edgelTwo.point.y, 2);
    }
  );


  DEBUG_REQUEST("Vision:LineGraphProvider:draw_neighbors",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < edgelNeighbors.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
      bool draw_node = false;

      if(edgelNeighbors[i].left != -1) {
        const ScanLineEdgelPercept::EdgelPair& edgel_left = getScanLineEdgelPercept().pairs[edgelNeighbors[i].left];
        PEN("000000",0.1);
        LINE(edgel.point.x, edgel.point.y, edgel_left.point.x, edgel_left.point.y);
        draw_node = true;
      }

      if(edgelNeighbors[i].right != -1) {
        const ScanLineEdgelPercept::EdgelPair& edgel_right = getScanLineEdgelPercept().pairs[edgelNeighbors[i].right];
        PEN("0000FF",0.1);
        LINE(edgel.point.x, edgel.point.y, edgel_right.point.x, edgel_right.point.y);
        draw_node = true;
      }

      if(draw_node) {
        PEN("FF0000",0.1);
        CIRCLE( edgel.point.x, edgel.point.y, 3);
      }
    }
  );

  DEBUG_REQUEST("Vision:LineGraphProvider:draw_neighbors_field",
    FIELD_DRAWING_CONTEXT;
    for(size_t i = 0; i < edgelNeighbors.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
      bool draw_node = false;
      
      Vector2d pointOnFieldEdgel;
      if(!CameraGeometry::imagePixelToFieldCoord(
          getCameraMatrix(), getCameraInfo(), 
          edgel.point, 
          0.0, 
          pointOnFieldEdgel))
      {
        continue;
      }

      if(edgelNeighbors[i].left != -1) {
        const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[edgelNeighbors[i].left];

        Vector2d pointOnField;
        if(CameraGeometry::imagePixelToFieldCoord(
            getCameraMatrix(), getCameraInfo(),
            edgel_s.point, 
            0.0, 
            pointOnField))
        {
          PEN("000000",1);
          LINE(pointOnFieldEdgel.x, pointOnFieldEdgel.y, pointOnField.x, pointOnField.y);
          draw_node = true;
        }
      }

      if(edgelNeighbors[i].right != -1) {
        const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[edgelNeighbors[i].right];
        Vector2d pointOnField;
        if(CameraGeometry::imagePixelToFieldCoord(
            getCameraMatrix(), getCameraInfo(), 
            edgel_s.point, 
            0.0, 
            pointOnField))
        {
          PEN("000000",1);
          LINE(pointOnFieldEdgel.x, pointOnFieldEdgel.y, pointOnField.x, pointOnField.y);
          draw_node = true;
        }
      }

      if(draw_node) {
        PEN("FF0000",1);
        CIRCLE( pointOnFieldEdgel.x, pointOnFieldEdgel.y, 25);
      }
    }
  );


  // calculate the projection for all edgels
  edgelProjections.resize(getScanLineEdgelPercept().pairs.size());
  for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
  {  
    const EdgelT<double>& edgelOne = getScanLineEdgelPercept().pairs[i];
    
    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      edgelOne.point.x,
      edgelOne.point.y,
      0.0,
      edgelProjections[i]);
  }

  // calculate the LineGraphPercept
  for(size_t j = 0; j < edgelPairs.size(); j++)
  {
    const EdgelPair& edgelPair = edgelPairs[j];
    const Vector2d& edgelLeft = edgelProjections[edgelPair.left];
    const Vector2d& edgelRight = edgelProjections[edgelPair.right];

    Edgel edgel;
    edgel.point = Vector2d(edgelLeft + edgelRight)*0.5;
    edgel.direction = (edgelRight - edgelLeft).normalize(); // is it correct?

    getLineGraphPercept().edgels.push_back(edgel);
  }

  // fill the compas
  if((int)edgelPairs.size() > parameters.minimalNumberOfPairs)
  {
    getProbabilisticQuadCompas().setSmoothing(parameters.quadCompasSmoothingFactor);
    for(size_t j = 0; j < edgelPairs.size(); j++)
    {
      const EdgelPair& edgelPair = edgelPairs[j];
      const Vector2d& edgelLeft = edgelProjections[edgelPair.left];
      const Vector2d& edgelRight = edgelProjections[edgelPair.right];

      double r = (edgelLeft - edgelRight).angle();
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
}//end execute

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

    if(getScanLineEdgelPercept().endPoints[edgelOne.id].posInImage.y > edgelOne.point.y) {
      continue;
    }
    
    int j_max = -1;
    double s_max = 0.0;

    for(size_t j = i+1; j < edgels.size(); j++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

      if(getScanLineEdgelPercept().endPoints[edgelTwo.id].posInImage.y > edgelTwo.point.y) {
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
