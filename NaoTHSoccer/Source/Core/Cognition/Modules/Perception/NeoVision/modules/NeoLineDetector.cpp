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

NeoLineDetector::NeoLineDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("NeoVision:NeoLineDetector:edgel_pairs", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("NeoVision:NeoLineDetector:edgel_cluster", "mark the edgels on the image", false);
}


NeoLineDetector::~NeoLineDetector(){}


void NeoLineDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  std::vector<Vector2<int> > edgelPairs;

  for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) 
  {
    const Edgel& edgelOne = getScanLineEdgelPercept().edgels[i];
    int j_max = -1;
    double s_max = 0.0;
    for(size_t j = i+1; j < getScanLineEdgelPercept().edgels.size(); j++) 
    {
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j];

      Vector2d v = edgelTwo.point - edgelOne.point;
      if(v.abs2() > 100 && v.abs2() < 20*20*2.6*2.6)
      {
        v.normalize();
        
        Vector2d g1(1.0,0);
        g1.rotate(edgelOne.angle);
        g1.rotateRight();

        Vector2d g2(1.0,0);
        g2.rotate(edgelTwo.angle);
        g2.rotateRight();

        double s = 0.0;
        if(g1*g2 > 0) {
          s = 1 - 0.5*(fabs(g1*v) + fabs(g2*v));
        }

        if(s > s_max) {
          s_max = s;
          j_max = (int)j;
        }
      }
    }//end for j

    if(j_max > -1) {
      
      DEBUG_REQUEST("NeoVision:NeoLineDetector:edgel_pairs",
        const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j_max];
        LINE_PX(ColorClasses::red, edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      );
      edgelPairs.push_back(Vector2<int>((int)i,j_max));
    }
  }//end for i
  
  std::vector<int> cluster(edgelPairs.size(),-1);
  std::vector<EdgelCluster> lines;

  int k = 0;
  for(size_t i = 0; i < edgelPairs.size(); i++) 
  {
    const Vector2<int>& pairOne = edgelPairs[i];
    const Edgel& edgelOneX = getScanLineEdgelPercept().edgels[pairOne.x];
    const Edgel& edgelOneY = getScanLineEdgelPercept().edgels[pairOne.y];

    if(cluster[i] == -1) {
      lines.push_back(EdgelCluster(k));
      lines[k].add(edgelOneX);
      lines[k].add(edgelOneY);
      cluster[i] = k++;
    }
    
    for(size_t j = i+1; j < edgelPairs.size(); j++) 
    {
      if(cluster[j] != -1) {
        continue;
      }

      const Vector2<int>& pairTwo = edgelPairs[j];
      const Edgel& edgelTwoX = getScanLineEdgelPercept().edgels[pairTwo.x];
      const Edgel& edgelTwoY = getScanLineEdgelPercept().edgels[pairTwo.y];

      Vector2d g1(1.0,0);
      g1.rotate(edgelOneX.angle);
      g1.rotateRight();

      Vector2d g2(1.0,0);
      g2.rotate(edgelTwoX.angle);
      g2.rotateRight();

      if(g1*g2 < 0) {
        continue;
      }

      double dXX = (edgelOneX.point - edgelTwoX.point).abs2();
      double dXY = (edgelOneX.point - edgelTwoY.point).abs2();
      double dYY = (edgelOneY.point - edgelTwoY.point).abs2();
      double dYX = (edgelOneY.point - edgelTwoX.point).abs2();

      Vector2<int> A,B,C,D;

      if(dXX >= dXY && dXX >= dYY && dXX >= dYX) {
        A = edgelOneX.point; B = edgelTwoX.point;
        C = edgelOneY.point; D = edgelTwoY.point;
      } else if(dXY >= dXX && dXY >= dYY && dXY >= dYX) {
        A = edgelOneX.point; B = edgelTwoY.point;
        C = edgelOneY.point; D = edgelTwoX.point;
      } else if(dYY >= dXY && dYY >= dXX && dYY >= dYX) {
        A = edgelOneY.point; B = edgelTwoY.point;
        C = edgelOneX.point; D = edgelTwoX.point;
      } else if(dYX >= dXY && dYX >= dYY && dYX >= dXX) {
        A = edgelOneY.point; B = edgelTwoX.point;
        C = edgelOneX.point; D = edgelTwoY.point;
      } else {
        assert(false);
      }


      double e1 = (C.x - A.x)*(A.y-B.y)+(C.y-A.y)*(B.x-A.x);
      double e2 = (D.x - A.x)*(A.y-B.y)+(D.y-A.y)*(B.x-A.x);
      double dAB = (A-B).abs2();

      /*
      // senity check
      if(dAB < (A-D).abs2() || dAB < (C-D).abs2() || dAB < (C-B).abs2()) {
        int x = 0;
      }
      */
      double maxT = 1.0;
      MODIFY("NeoVision:NeoLineDetector:maxT", maxT);
      if(0.5*(fabs(e1)+fabs(e2))/sqrt(dAB) < maxT ) {
        cluster[j] = cluster[i];
        lines[cluster[i]].add(edgelTwoX);
        lines[cluster[i]].add(edgelTwoY);
      }
    }
  }

  DEBUG_REQUEST("NeoVision:NeoLineDetector:edgel_cluster",
    for(size_t i = 0; i < lines.size(); i++) 
    {
      if(lines[i].size() > 2 && sqrt(lines[i].length()) / static_cast<double>(lines[i].size()) < 20) {
        lines[i].draw();
      }
    }
  );

  /*
  for(size_t i = 0; i < edgelPairs.size(); i++) 
  {
    const Vector2<int>& pairOne = edgelPairs[i];
    const Edgel& edgelOneX = getScanLineEdgelPercept().edgels[pairOne.x];
    const Edgel& edgelOneY = getScanLineEdgelPercept().edgels[pairOne.y];

    assert(cluster[i] > -1);
    int idx = cluster[i] % (unsigned int)ColorClasses::numOfColors;
    LINE_PX((ColorClasses::Color) (idx), edgelOneX.point.x, edgelOneX.point.y, edgelOneY.point.x, edgelOneY.point.y);
  }
  */



}//end execute
