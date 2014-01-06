/* 
 * File:   LineClusterProvider.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. März 2011, 14:22
 */

#include "LineClusterProvider.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/NaoTHAssert.h"

LineClusterProvider::LineClusterProvider()
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:LineClusterProvider:line_clusters", "", false);
}


LineClusterProvider::~LineClusterProvider()
{
}


void LineClusterProvider::execute()
{

  Vector2d edgelOnField_e;
  Vector2d edgelOnField_f;

  for(unsigned int i = 0; i < getScanLineEdgelPercept().numOfSeenEdgels; i++)
  {
    const DoubleEdgel& e = getScanLineEdgelPercept().scanLineEdgels[i];
    int k = -1;
    double distance = Math::fromDegrees(5);//getImage().height()*getImage().width();
    //distance *= distance;

    if(!CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        e.center.x,
        e.center.y,
        0.0,
        edgelOnField_e)) continue;

    for(unsigned int j = i+1; j < getScanLineEdgelPercept().numOfSeenEdgels; j++)
    {
      const DoubleEdgel& f = getScanLineEdgelPercept().scanLineEdgels[j];
      if(!CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        f.center.x,
        f.center.y,
        0.0,
        edgelOnField_f)) continue;
      
      double a = (e.center - f.center).angle();
      double d = (fabs(Math::normalize(a - e.begin_angle)) + fabs(Math::normalize(a - f.begin_angle)))/2.0;

      //double d = (edgelOnField_e - edgelOnField_f).abs2();
      if(d < distance && (f.center - e.center).abs2() < 30*30)
      {
        distance = d;
        k = j;
      }
    }//end for


    if(k > 0)
    {
      const DoubleEdgel& f = getScanLineEdgelPercept().scanLineEdgels[k];

      DEBUG_REQUEST("Vision:Detectors:LineClusterProvider:line_clusters",
        LINE_PX(ColorClasses::red,
          e.center.x, e.center.y,
          f.center.x, f.center.y);
      );
    }
  }//end for
}//end execute
