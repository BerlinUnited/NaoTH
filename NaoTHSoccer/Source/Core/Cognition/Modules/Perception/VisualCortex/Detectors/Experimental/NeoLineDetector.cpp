/* 
 * File:   NeoLineDetector.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "NeoLineDetector.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Math/Geometry.h"
#include "Tools/DataStructures/Histogram.h"

NeoLineDetector::NeoLineDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:edgel_pairs", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_neighbors", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_neighbors_field", "mark the edgels on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:edgel_cluster", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_lines", "draw the esimated lines in the image", false);

  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_compas", "draw compas direcion based on the edgel directions", false);
}


NeoLineDetector::~NeoLineDetector(){}


void NeoLineDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  double threshold = 0.8;
  MODIFY("NeoLineDetector:threshold",threshold);
  
  //calculateNeigbors(getScanLineEdgelPercept().pairs, edgelNeighbors, threshold);
  //calculatePairs(getScanLineEdgelPercept().pairs, edgelPairs, threshold);
  calculatePairsAndNeigbors(getScanLineEdgelPercept().pairs, edgelPairs, edgelNeighbors, threshold);

  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:edgel_pairs",
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


  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_neighbors",
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

  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_neighbors_field",
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

      if(edgelNeighbors[i].left != -1 && edgelNeighbors[i].w_left > threshold) {
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

      if(edgelNeighbors[i].right != -1 && edgelNeighbors[i].w_right > threshold) {
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

  getProbabilisticQuadCompas().setSmoothing(0.6);
  // fill the compas
  for(size_t j = 0; j < edgelPairs.size(); j++)
  {
    const EdgelPair& edgelPair = edgelPairs[j];
    const Vector2d& edgelLeft = edgelProjections[edgelPair.left];
    const Vector2d& edgelRight = edgelProjections[edgelPair.right];

    double r = (edgelLeft - edgelRight).angle();
    getProbabilisticQuadCompas().add(r, edgelPair.sim);
  }


  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_compas",
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
          DebugDrawings::Color c(d, 0.0, 1-d);
          PEN(c, 10);

          LINE(last.x,last.y,a.x,a.y);
        }
        last = a;
        last_v = v;
      }
    }
  );


  //////////////////////////////////////
  // This is highly experimental stuff
  /////////////////////////////////////

  /*
  // test for the edgelSim applied to the edges between the edgels
  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  for(size_t i = 0; i < neighbors.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];

    PEN("000000",0.3);
    if(neighbors[i].left != -1 && neighbors[i].right != -1) {
      const ScanLineEdgelPercept::EdgelPair& edgel_left = getScanLineEdgelPercept().pairs[neighbors[i].left];
      const ScanLineEdgelPercept::EdgelPair& edgel_right = getScanLineEdgelPercept().pairs[neighbors[i].right];

      Vector2d n1(edgel.center - edgel_left.center);
      Vector2d p1(edgel.center + edgel_left.center);

      Vector2d n2(edgel_right.center - edgel.center);
      Vector2d p2(edgel_right.center + edgel.center);

      double d = edgelSim(p1*0.5, n1.angle(), p2*0.5, n2.angle());
      //neighbors[i].w_left = (neighbors[i].w_left + neighbors[i].w_right*d)*0.5;
      //neighbors[i].w_right = (neighbors[i].w_right + neighbors[i].w_left*d)*0.5;

      if(d > threshold) {
        CIRCLE( edgel.center.x, edgel.center.y, 3);
        LINE(edgel.center.x, edgel.center.y, edgel_left.center.x, edgel_left.center.y);
        LINE(edgel.center.x, edgel.center.y, edgel_right.center.x, edgel_right.center.y);
      }
    }
  }
  */


  //std::vector<int> cluster(getScanLineEdgelPercept().pairs.size(),-1);
  //std::vector<EdgelCluster> lines;

  /*
  // test: extract chains from neighbors
  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  size_t k = 0;
  for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
  {
    const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];

    if(cluster[i] != -1) { continue; }

    size_t k = lines.size();
    lines.push_back(EdgelCluster(k));
    cluster[i] = k;
    lines[k].add(edgel);
    lines[k].inside_the_field = lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel.id].posInImage.y < edgel.point.y;

    //int idx = k % (unsigned int)ColorClasses::numOfColors;
    //int radius = k / (unsigned int)ColorClasses::numOfColors;

    //PEN(DebugDrawings::Color((unsigned int)idx),1);
    //CIRCLE( edgel.point.x, edgel.point.y, 3+radius);

    int i_right = neighbors[i].right;
    while(i_right != -1 && cluster[i_right] == -1)
    {
      const ScanLineEdgelPercept::EdgelPair& edgel_one = getScanLineEdgelPercept().pairs[i_right];

      //CIRCLE( edgel_one.point.x, edgel_one.point.y, 3+radius);
      if(neighbors[i_right].w_right > threshold) {
        cluster[i_right] = k;
        lines[k].add(edgel_one);
        lines[k].inside_the_field = lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel_one.id].posInImage.y < edgel_one.point.y;

        i_right = neighbors[i_right].right;
      } else {
        break;
      }
    }

    int i_left = neighbors[i].left;
    while(i_left != -1 && cluster[i_left] == -1)
    {
      const ScanLineEdgelPercept::EdgelPair& edgel_one = getScanLineEdgelPercept().pairs[i_left];

      //CIRCLE( edgel_one.point.x, edgel_one.point.y, 3+radius);

      if(neighbors[i_left].w_left > threshold) {
        cluster[i_left] = k;
        lines[k].add(edgel_one);
        lines[k].inside_the_field = lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel_one.id].posInImage.y < edgel_one.point.y;

        i_left = neighbors[i_left].left;
      } else {
        break;
      }
    }
  }
  */
  
  /*
  // test: cluster the pairs
  double max_cluster_dist = 3.0;
  MODIFY("NeoLineDetector:max_cluster_dist",max_cluster_dist);
  std::sort(edgelPairs.begin(), edgelPairs.end(), compare_double_pair);
  for(size_t i = 0; i < edgelPairs.size(); i++)
  {
    const EdgelPair& pair = edgelPairs[i];

    // create a new cluster
    if(cluster[pair.left] == -1 && cluster[pair.right] == -1)
    {
      size_t k = lines.size();
      lines.push_back(EdgelCluster(k));
      lines[k].add(getScanLineEdgelPercept().pairs[pair.left]);
      lines[k].add(getScanLineEdgelPercept().pairs[pair.right]);
      lines[k].inside_the_field = true;
      cluster[pair.left] = k;
      cluster[pair.right] = k;

      int i_left = pair.left;
      int i_right = pair.right;

      while(i_left != -1 || i_right != -1)
      {
        int i_right_old = -1;
        int i_next = -1;
        double w_next = 0;
        if(i_right != -1 && edgelNeighbors[i_right].w_right > threshold) {
          w_next = edgelNeighbors[i_right].w_right;
          i_right_old = i_right;
          i_right = edgelNeighbors[i_right].right;
          i_next = i_right;
        } else {
          i_right = -1;
        }

        if(i_left != -1 && edgelNeighbors[i_left].w_left > threshold) {
          if(i_right == -1 || edgelNeighbors[i_left].w_left > w_next) {
            i_left = edgelNeighbors[i_left].left;
            i_next = i_left;
            i_right = i_right_old;
          }
        } else {
          i_left = -1;
        }

        if(i_next == -1) { break; }
        const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i_next];

        const Math::Line& line = lines[k].getLine();
        if(cluster[i_next] != -1 || line.minDistance(edgel.point) > max_cluster_dist) 
        {
          if(i_left == i_next) {
            i_left = -1;
          } else {
            i_right = -1;
          }
        } else {
          //lines[k].inside_the_field = true; //lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel.id].posInImage.y < edgel.point.y;
          lines[k].add(edgel);
          cluster[i_next] = k;
        }
      }
    }
  }
  */

  /*
  // test: extract chains from edgel pairs by exploring the neighbors
  double max_cluster_dist = 3.0;
  MODIFY("NeoLineDetector:max_cluster_dist",max_cluster_dist);
  std::sort(doubleEdgelPairs.begin(), doubleEdgelPairs.end(), compare_double_pair);
  for(size_t i = 0; i < doubleEdgelPairs.size(); i++)
  {
    const EdgelPair& pair = doubleEdgelPairs[i];

    if(cluster[pair.left] == -1 && cluster[pair.right] == -1)
    {
      size_t k = lines.size();
      lines.push_back(EdgelCluster(k));
      lines[k].add(getScanLineEdgelPercept().pairs[pair.left]);
      lines[k].add(getScanLineEdgelPercept().pairs[pair.right]);
      lines[k].inside_the_field = true;
      cluster[pair.left] = k;
      cluster[pair.right] = k;

      int i_left = pair.left;
      while(neighbors[i_left].left != -1 && neighbors[i_left].w_left > threshold)
      {
        i_left = neighbors[i_left].left;
        const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i_left];

        const Math::Line& line = lines[k].getLine();
        if(cluster[i_left] != -1 || line.minDistance(edgel.point) > max_cluster_dist) { break; }

        lines[k].inside_the_field = true; //lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel.id].posInImage.y < edgel.point.y;
        lines[k].add(edgel);
        cluster[i_left] = k;
      }

      int i_right = pair.right;
      while(neighbors[i_right].right != -1 && neighbors[i_right].w_right > threshold)
      {
        i_right = neighbors[i_right].right;
        const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i_right];

        const Math::Line& line = lines[k].getLine();
        if(cluster[i_right] != -1 || line.minDistance(edgel.point) > max_cluster_dist) { break; }

        lines[k].inside_the_field = true; //lines[k].inside_the_field || getScanLineEdgelPercept().endPoints[edgel.id].posInImage.y < edgel.point.y;
        lines[k].add(edgel);
        cluster[i_right] = k;
      }
    }
  }
  */


  /*
  // calculate the curvature
  for(size_t i = 0; i < lines.size(); i++) {
    EdgelCluster& line = lines[i];
    if(line.getEdgels().size() < 3) { continue; }
    
    Vector2d p0 = line.getEdgels()[0].point;
    Vector2d p1 = line.getEdgels()[1].point;

    double eta = 0;
    for(size_t j = 2; j < line.getEdgels().size(); j++) 
    {
      Vector2d p2 = line.getEdgels()[j].point;

      double angle_cos = (p2-p1).normalize().rotate(-((p1-p0).normalize().angle())).angle();
      //eta += 1.0 - angle_cos;
      eta += angle_cos;

      p0 = p1;
      p1 = p2;
    }

    CIRCLE_PX(ColorClasses::red, (int)p1.x,(int)p1.y, (int)(fabs(eta)*10+0.5));
  }
  */

/*
  std::vector<bool> line_deleted(lines.size(), false);

  double segmentSimThreshold = 0;
  MODIFY("NeoLineDetector:segmentSimThreshold",segmentSimThreshold);
  for(size_t i = 0; i < lines.size(); i++) {
    if(line_deleted[i] || lines[i].size() == 0) { continue; }
    //Math::Line line = lines[i].getLine();
    for(size_t j = i+1; j < lines.size(); j++) 
    {
      if(line_deleted[j] || lines[j].size() == 0) { continue; }
      const EdgelCluster& clusterTwo = lines[j];
      double li = (lines[i].front() - lines[i].back()).abs();
      double lj = (lines[j].front() - lines[j].back()).abs();
      if(lines[i].back().x < lines[j].front().x && (lines[j].front() - lines[i].back()).abs()*3 < li+lj) // j is right of i
      {
        Vector2d v = (lines[j].back() - lines[i].front()).normalize().rotateLeft();
        Vector2d vi = (lines[i].back() - lines[i].front());
        Vector2d vj = (lines[j].back() - lines[j].front());
        
        const ScanLineEdgelPercept::EdgelPair& pair = *lines[j].getEdgels().begin();
        const Edgel& begin = getScanLineEdgelPercept().edgels[pair.begin];
        const Edgel& end = getScanLineEdgelPercept().edgels[pair.end];
        double lineWidth = fabs(((double)(begin.point - end.point).y) * sin(Math::pi_2 - pair.direction.angle()));
        
        if(0.5*(fabs(v*vi) + fabs(v*vj)) < lineWidth*segmentSimThreshold) {
          lines[i].add(clusterTwo);
          line_deleted[j] = true;
        }
      }
      else if(lines[j].back().x < lines[j].front().x && (lines[i].front() - lines[j].back()).abs()*3 < li+lj) // i is right of j
      {
        Vector2d v = (lines[i].back() - lines[j].front()).normalize().rotateLeft();
        Vector2d vi = (lines[i].back() - lines[i].front());
        Vector2d vj = (lines[j].back() - lines[j].front());

        const ScanLineEdgelPercept::EdgelPair& pair = *lines[j].getEdgels().begin();
        const Edgel& begin = getScanLineEdgelPercept().edgels[pair.begin];
        const Edgel& end = getScanLineEdgelPercept().edgels[pair.end];
        double lineWidth = fabs(((double)(begin.point - end.point).y) * sin(Math::pi_2 - pair.direction.angle()));

        if(0.5*(fabs(v*vi) + fabs(v*vj)) < lineWidth*segmentSimThreshold) {
          lines[i].add(clusterTwo);
          line_deleted[j] = true;
        }
      }
    }
  }


  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:edgel_cluster",
    for(size_t i = 0; i < lines.size(); i++) {
      if(lines[i].inside_the_field && !line_deleted[i]){ lines[i].getLine(); lines[i].draw(); }
    }
  );

  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_lines",
    for(size_t i = 0; i < lines.size(); i++) 
    {
      Math::Line line = lines[i].getLine();
      int idx = lines[i].id() % (unsigned int)ColorClasses::numOfColors;
      const Vector2i frameUpperLeft(0,0);
      const Vector2i frameLowerRight(getCameraInfo().resolutionWidth-1, getCameraInfo().resolutionHeight-1);
      Vector2i p0 = line.projection(lines[i].front());
      Vector2i p1 = line.projection(lines[i].back());
      LINE_PX((ColorClasses::Color) (idx), p0.x,p0.y, p1.x, p1.y);
    }
  );
  */
}//end execute

double NeoLineDetector::edgelSim(const Vector2d& p1, double a1, const Vector2d& p2, double a2)
{
  Vector2d g1(1.0,0);
  g1.rotate(a1);

  Vector2d g2(1.0,0);
  g2.rotate(a2);

  Vector2d v = (p2 - p1).rotateRight().normalize();

  double s = 0.0;
  if(g1*g2 > 0) {
    s = 1-0.5*(fabs(g1*v) + fabs(g2*v));
  }

  return s;
}

double NeoLineDetector::edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2)
{
  double s = 0.0;
  if(e1.direction*e2.direction > 0) {
    Vector2d v = (e2.point - e1.point).rotateRight().normalize();
    s = 1.0-0.5*(fabs(e1.direction*v) + fabs(e2.direction*v));
  }

  return s;
}

double NeoLineDetector::segmentSim(const Vector2d& p0, const Vector2d& p1, const Vector2d& q0, const Vector2d& q1)
{
  Vector2d pm = (p0+p1)*0.5;
  Vector2d qm = (q0+q1)*0.5;
  Vector2d pv = (p0-p1).normalize();
  Vector2d qv = (q0-q1).normalize();
  Vector2d v = (qm - pm).normalize();
  return 1.0-(fabs(pv*v) + fabs(qv*v))*0.5;
}

void NeoLineDetector::calculatePairs(const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, std::vector<EdgelPair>& edgelPairs, double threshold) const
{
  edgelPairs.clear();
  for(unsigned int i = 0; i < edgels.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& edgelOne = edgels[i];
    int j_max = -1;
    double s_max = 0.0;
    for(unsigned int j = i+1; j < edgels.size(); j++)
    {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

      if(edgelTwo.id == edgelOne.id + 1 || edgelOne.id == edgelTwo.id + 1)
      {
        double s = edgelSim(edgelOne, edgelTwo);

        if(s > s_max) {
          s_max = s;
          j_max = (int)j;
        }
      }
    }//end for j

    if(j_max > -1 && s_max > threshold) {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j_max];
      if(edgelOne.id < edgelTwo.id) {
        edgelPairs.push_back(EdgelPair(i,j_max,s_max));
      } else {
        edgelPairs.push_back(EdgelPair(j_max,i,s_max));
      }
    }
  }//end for i
}


void NeoLineDetector::calculateNeigbors(const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, std::vector<Neighbors>& neighbors, double /*threshold*/) const
{
  neighbors.resize(edgels.size());

  // TODO: this is rather inefficient
  for(unsigned int i = 0; i < edgels.size(); i++) {
    neighbors[i].reset();
  }

  for(size_t i = 0; i < edgels.size(); i++) 
  {
    const ScanLineEdgelPercept::EdgelPair& edgelOne = edgels[i];

    for(size_t j = i+1; j < edgels.size(); j++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

      if(edgelTwo.id == edgelOne.id + 1 || edgelOne.id == edgelTwo.id + 1)
      {
        double s = edgelSim(edgelOne, edgelTwo);

        // update neigbors
        if(s > threshold) 
        {
          int idx_left = edgelTwo.id < edgelOne.id ? j : i;
          int idx_right = edgelTwo.id < edgelOne.id ? i : j;

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
  }//end for i
}


void NeoLineDetector::calculatePairsAndNeigbors(
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
    
    int j_max = -1;
    double s_max = 0.0;

    for(size_t j = i+1; j < edgels.size(); j++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = edgels[j];

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
          int idx_left = edgelTwo.id < edgelOne.id ? j : i;
          int idx_right = edgelTwo.id < edgelOne.id ? i : j;

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
