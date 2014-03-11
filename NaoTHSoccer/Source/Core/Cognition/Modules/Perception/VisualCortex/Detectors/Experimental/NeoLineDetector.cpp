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

NeoLineDetector::NeoLineDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:edgel_pairs", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:double_edgel_pairs", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_neighbors", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_double_neighbors", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_neighbors_field", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:edgel_cluster", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NeoLineDetector:draw_lines", "draw the esimated lines in the image", false);
}


NeoLineDetector::~NeoLineDetector(){}


void NeoLineDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  double threshold = 0.8;
  MODIFY("NeoLineDetector:threshold",threshold);

  std::vector<Neighbors> edgelNeighbors(getScanLineEdgelPercept().edgels.size());
  std::vector<EdgelPair> edgelPairs;
  for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) 
  {
    const Edgel& edgelOne = getScanLineEdgelPercept().edgels[i];
    int j_max = -1;
    double s_max = 0.0;
    for(size_t j = i+1; j < getScanLineEdgelPercept().edgels.size(); j++)
    {
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j];

      if(abs(edgelOne.point.x - edgelTwo.point.x) > 5 && abs(edgelOne.point.x - edgelTwo.point.x) - 5 < 20)
      {
        double s = edgelSim(edgelOne.point, edgelOne.direction.angle(), edgelTwo.point, edgelTwo.direction.angle());

        if(s > s_max) {
          s_max = s;
          j_max = (int)j;
        }

        // update neigbors
        if(edgelOne.point.x < edgelTwo.point.x) { // edgelTwo (j_max) is left of edgelOne (i)
        
          if(edgelNeighbors[i].left == -1 || edgelNeighbors[i].w_left < s) { // set the left neighbor for i
            edgelNeighbors[i].left = j;
            edgelNeighbors[i].w_left = s;
          }

          if(edgelNeighbors[j].right == -1 || edgelNeighbors[j].w_right < s) { // set the right neighbor for j_max
            edgelNeighbors[j].right = i;
            edgelNeighbors[j].w_right = s;
          }

        } else { // edgelTwo (j_max) is right of edgelOne (i)
        
          if(edgelNeighbors[i].right == -1 || edgelNeighbors[i].w_right < s) { // set the right neighbor for i
            edgelNeighbors[i].right = j;
            edgelNeighbors[i].w_right = s;
          }

          if(edgelNeighbors[j].left == -1 || edgelNeighbors[j].w_left < s) { // set the left neighbor for j_max
            edgelNeighbors[j].left = i;
            edgelNeighbors[j].w_left = s;
          }
        }
      }
    }//end for j

    if(j_max > -1 && s_max > threshold) {
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j_max];
      if(edgelOne.point.x < edgelTwo.point.x) {
        edgelPairs.push_back(EdgelPair(i,j_max,s_max));
      } else {
        edgelPairs.push_back(EdgelPair(j_max,i,s_max));
      }
    }
  }//end for i


  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:edgel_pairs",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < edgelPairs.size(); i++) 
    {
      const EdgelPair& pair = edgelPairs[i];
      const Edgel& edgelOne = getScanLineEdgelPercept().edgels[pair.left];
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[pair.right];

      PEN("FF0000",0.1);
      //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
      LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      CIRCLE( edgelOne.point.x, edgelOne.point.y, 3);
    }
  );

  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_neighbors",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < edgelNeighbors.size(); i++)
    {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      bool draw_node = false;

      if(edgelNeighbors[i].left != -1 && edgelNeighbors[i].w_left > threshold) {
        const Edgel& edgel_left = getScanLineEdgelPercept().edgels[edgelNeighbors[i].left];
        PEN("000000",0.1);
        LINE(edgel.point.x, edgel.point.y, edgel_left.point.x, edgel_left.point.y);
        draw_node = true;
      }

      if(edgelNeighbors[i].right != -1 && edgelNeighbors[i].w_right > threshold) {
        const Edgel& edgel_right = getScanLineEdgelPercept().edgels[edgelNeighbors[i].right];
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





  

  std::vector<Neighbors> neighbors(getScanLineEdgelPercept().pairs.size());
  std::vector<EdgelPair> doubleEdgelPairs;
  for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
  {
    const ScanLineEdgelPercept::EdgelPair& edgelOne = getScanLineEdgelPercept().pairs[i];
    int j_max = -1;
    double s_max = 0.0;
    for(size_t j = i+1; j < getScanLineEdgelPercept().pairs.size(); j++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[j];

      if(edgelTwo.id == edgelOne.id + 1 || edgelOne.id == edgelTwo.id + 1)
      {
        double s = edgelSim(edgelOne, edgelTwo);

        if(s > s_max) {
          s_max = s;
          j_max = (int)j;
        }

        // update neigbors
        if(edgelTwo.id < edgelOne.id) { // edgelTwo (j_max) is left of edgelOne (i)
        
          if(neighbors[i].left == -1 || neighbors[i].w_left < s) { // set the left neighbor for i
            neighbors[i].left = j;
            neighbors[i].w_left = s;
          }

          if(neighbors[j].right == -1 || neighbors[j].w_right < s) { // set the right neighbor for j_max
            neighbors[j].right = i;
            neighbors[j].w_right = s;
          }

        } else { // edgelTwo (j_max) is right of edgelOne (i)
        
          if(neighbors[i].right == -1 || neighbors[i].w_right < s) { // set the right neighbor for i
            neighbors[i].right = j;
            neighbors[i].w_right = s;
          }

          if(neighbors[j].left == -1 || neighbors[j].w_left < s) { // set the left neighbor for j_max
            neighbors[j].left = i;
            neighbors[j].w_left = s;
          }
        }
      }
    }//end for j

    if(j_max > -1 && s_max > threshold) {
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[j_max];
      if(edgelOne.id < edgelTwo.id) {
        doubleEdgelPairs.push_back(EdgelPair(i,j_max,s_max));
      } else {
        doubleEdgelPairs.push_back(EdgelPair(j_max,i,s_max));
      }
    }
  }//end for i


  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:double_edgel_pairs",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < doubleEdgelPairs.size(); i++) 
    {
      const EdgelPair& pair = doubleEdgelPairs[i];
      const ScanLineEdgelPercept::EdgelPair& edgelOne = getScanLineEdgelPercept().pairs[pair.left];
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[pair.right];

      PEN("FF0000",0.1);
      //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
      LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      CIRCLE( edgelOne.point.x, edgelOne.point.y, 3);
    }
  );

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
  

  DEBUG_REQUEST("Vision:Detectors:NeoLineDetector:draw_double_neighbors",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    for(size_t i = 0; i < neighbors.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
      bool draw_node = false;

      if(neighbors[i].left != -1 && neighbors[i].w_left > threshold) {
        const ScanLineEdgelPercept::EdgelPair& edgel_left = getScanLineEdgelPercept().pairs[neighbors[i].left];
        PEN("000000",0.1);
        LINE(edgel.point.x, edgel.point.y, edgel_left.point.x, edgel_left.point.y);
        draw_node = true;
      }

      if(neighbors[i].right != -1 && neighbors[i].w_right > threshold) {
        const ScanLineEdgelPercept::EdgelPair& edgel_right = getScanLineEdgelPercept().pairs[neighbors[i].right];
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
    for(size_t i = 0; i < neighbors.size(); i++)
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

      if(neighbors[i].left != -1 && neighbors[i].w_left > threshold) {
        const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[neighbors[i].left];

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

      if(neighbors[i].right != -1 && neighbors[i].w_right > threshold) {
        const ScanLineEdgelPercept::EdgelPair& edgel_s = getScanLineEdgelPercept().pairs[neighbors[i].right];
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


  std::vector<int> cluster(getScanLineEdgelPercept().pairs.size(),-1);
  std::vector<EdgelCluster> lines;

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
  
  // test: cluster the pairs
  double max_cluster_dist = 3.0;
  MODIFY("NeoLineDetector:max_cluster_dist",max_cluster_dist);
  std::sort(doubleEdgelPairs.begin(), doubleEdgelPairs.end(), compare_double_pair);
  for(size_t i = 0; i < doubleEdgelPairs.size(); i++)
  {
    const EdgelPair& pair = doubleEdgelPairs[i];

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
        if(i_right != -1 && neighbors[i_right].w_right > threshold) {
          w_next = neighbors[i_right].w_right;
          i_right_old = i_right;
          i_right = neighbors[i_right].right;
          i_next = i_right;
        } else {
          i_right = -1;
        }

        if(i_left != -1 && neighbors[i_left].w_left > threshold) {
          if(i_right == -1 || neighbors[i_left].w_left > w_next) {
            i_left = neighbors[i_left].left;
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
  int numberOfPoints = getScanLineEdgelPercept().pairs.size();
  int numberOfLabels = lines.size() + 1;

  if (false && numberOfPoints > 1 && numberOfLabels > 1)
  {
    // prepare the data cost
    std::vector<GCoptimization::EnergyTermType> data_cost(numberOfPoints*numberOfLabels, 0);
    for(size_t i = 0; i < cluster.size(); i++) {
        if(cluster[i] >= 0) {
          data_cost[i*numberOfLabels + cluster[i]] = -1;
        } else {
          data_cost[i*numberOfLabels + (numberOfLabels-1)] = -1;
        }
    }

    SmoothLineEdgelEnergy smoothCost(getScanLineEdgelPercept().pairs);

    GCoptimizationGeneralGraph gc(numberOfPoints, numberOfLabels);
    gc.setDataCost(data_cost.data());
    gc.setSmoothCostFunctor(&smoothCost);

    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
      for(size_t j = i+1; j < getScanLineEdgelPercept().pairs.size(); j++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgelOne = getScanLineEdgelPercept().pairs[i];
      const ScanLineEdgelPercept::EdgelPair& edgelTwo = getScanLineEdgelPercept().pairs[j];
      //double sim = edgelSim(edgelOne.center, edgelOne.center_angle, edgelTwo.center, edgelTwo.center_angle);
      int a = (edgelTwo.ScanLineID == edgelOne.ScanLineID + 1 || edgelOne.ScanLineID == edgelTwo.ScanLineID + 1);
      gc.setNeighbors(i,j, a);
    }

    gc.expansion(2);

    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
    {
      const ScanLineEdgelPercept::EdgelPair& edgel = getScanLineEdgelPercept().pairs[i];
      int label = gc.whatLabel(i);
      int idx = label % (unsigned int)ColorClasses::numOfColors;
      CIRCLE_PX((ColorClasses::Color) (idx), edgel.center.x, edgel.center.y, 5);
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


  /*
  std::vector<int> cluster(edgelPairs.size(),-1);
  std::vector<EdgelCluster> lines;

  for(size_t i = 0; i < doubleEdgelPairs.size(); i++) 
  {
    const EdgelPair& pairOne = doubleEdgelPairs[i];
    const ScanLineEdgelPercept::EdgelPair& edgelOneX = getScanLineEdgelPercept().pairs[pairOne.i0];
    const ScanLineEdgelPercept::EdgelPair& edgelOneY = getScanLineEdgelPercept().pairs[pairOne.i1];

    if(cluster[i] == -1) {
      size_t k = lines.size();
      lines.push_back(EdgelCluster(k));
      lines[k].add(edgelOneX.center, edgelOneX.center_angle);
      lines[k].add(edgelOneY.center, edgelOneY.center_angle);
      cluster[i] = k;
    }
    
    for(size_t j = i+1; j < doubleEdgelPairs.size(); j++) 
    {
      if(cluster[j] != -1) {
        continue;
      }

      const EdgelPair& pairTwo = doubleEdgelPairs[j];
      const ScanLineEdgelPercept::EdgelPair& edgelTwoX = getScanLineEdgelPercept().pairs[pairTwo.i0];
      const ScanLineEdgelPercept::EdgelPair& edgelTwoY = getScanLineEdgelPercept().pairs[pairTwo.i1];

      Vector2d g1(1.0,0);
      g1.rotate(edgelOneX.center_angle);
      g1.rotateRight();

      Vector2d g2(1.0,0);
      g2.rotate(edgelTwoX.center_angle);
      g2.rotateRight();

      if(g1*g2 < 0) {
        continue;
      }

      double dXX = (edgelOneX.center - edgelTwoX.center).abs2();
      double dXY = (edgelOneX.center - edgelTwoY.center).abs2();
      double dYY = (edgelOneY.center - edgelTwoY.center).abs2();
      double dYX = (edgelOneY.center - edgelTwoX.center).abs2();

      Vector2d A,B,C,D;

      if(dXX >= dXY && dXX >= dYY && dXX >= dYX) {
        A = edgelOneX.center; B = edgelTwoX.center;
        C = edgelOneY.center; D = edgelTwoY.center;
      } else if(dXY >= dXX && dXY >= dYY && dXY >= dYX) {
        A = edgelOneX.center; B = edgelTwoY.center;
        C = edgelOneY.center; D = edgelTwoX.center;
      } else if(dYY >= dXY && dYY >= dXX && dYY >= dYX) {
        A = edgelOneY.center; B = edgelTwoY.center;
        C = edgelOneX.center; D = edgelTwoX.center;
      } else if(dYX >= dXY && dYX >= dYY && dYX >= dXX) {
        A = edgelOneY.center; B = edgelTwoX.center;
        C = edgelOneX.center; D = edgelTwoY.center;
      } else {
        assert(false);
      }

      Vector2d n = (B-A).rotateLeft().normalize();

      double e1 = (C-A)*n;//(C.x - A.x)*(A.y-B.y)+(C.y-A.y)*(B.x-A.x);
      double e2 = (D-B)*n;//(D.x - A.x)*(A.y-B.y)+(D.y-A.y)*(B.x-A.x);
      double dAB = (A-B).abs2();

      // senity check
      if(dAB < (A-D).abs2() || dAB < (C-D).abs2() || dAB < (C-B).abs2()) {
        int x = 0;
      }
     
      double maxT = 1.0;
      MODIFY("Vision:Detectors:NeoLineDetector:maxT", maxT);
      if(0.5*(fabs(e1)+fabs(e2)) < maxT ) {
        cluster[j] = cluster[i];
        lines[cluster[i]].add(edgelTwoX.center, edgelTwoX.center_angle);
        lines[cluster[i]].add(edgelTwoY.center, edgelTwoY.center_angle);
      }
    }
  }
  */

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
  
  /*
  for(size_t i = 0; i < edgelPairs.size(); i++) 
  {
    const Vector2i& pairOne = edgelPairs[i];
    const Edgel& edgelOneX = getScanLineEdgelPercept().edgels[pairOne.x];
    const Edgel& edgelOneY = getScanLineEdgelPercept().edgels[pairOne.y];

    assert(cluster[i] > -1);
    int idx = cluster[i] % (unsigned int)ColorClasses::numOfColors;
    LINE_PX((ColorClasses::Color) (idx), edgelOneX.point.x, edgelOneX.point.y, edgelOneY.point.x, edgelOneY.point.y);
  }
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
