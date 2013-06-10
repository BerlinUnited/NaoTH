/* 
 * File:   NeoLineDetector.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "NeoLineDetector.h"

#include "Tools/CameraGeometry.h"

NeoLineDetector::NeoLineDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("NeoVision:NeoLineDetector:mark_edgels", "mark the edgels on the image", false);
}


NeoLineDetector::~NeoLineDetector(){}


void NeoLineDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  for(unsigned i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) 
  {
    const Edgel& edgelOne = getScanLineEdgelPercept().edgels[i];
    int j_max = -1;
    double s_max = 0.0;
    for(unsigned j = i+1; j < getScanLineEdgelPercept().edgels.size(); j++) 
    {
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j];

      Vector2d v = edgelTwo.point - edgelOne.point;
      if(v.abs2() > 100 && v.abs2() < 80*80)
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
          j_max = j;
        }
      }
    }//end for j

    if(j_max > -1) {
      const Edgel& edgelTwo = getScanLineEdgelPercept().edgels[j_max];
      LINE_PX(ColorClasses::red, edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
    }
  }//end for i
  

}//end execute
