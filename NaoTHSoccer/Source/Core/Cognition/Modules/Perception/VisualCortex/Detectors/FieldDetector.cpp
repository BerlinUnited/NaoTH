/* 
 * File:   FieldDetector.cpp
 * Author: Heinrich Mellmann
 * 
 */

#include "FieldDetector.h"

#include <Tools/Math/ConvexHull.h>

#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>

using namespace std;

FieldDetector::FieldDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:FieldDetector:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
}


FieldDetector::~FieldDetector()
{
}


void FieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  if(getScanLineEdgelPercept().endPoints.size() > 0)
  {
    vector<Vector2i > points(getScanLineEdgelPercept().endPoints.size());

    for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      points[i] = getScanLineEdgelPercept().endPoints[i].posInImage;
    }

    // move the outer points
    points.front().x = 0;
    points.back().x = getImage().width()-1;

    // lower image points
    points.push_back(Vector2i(0,getImage().height()-1));
    points.push_back(Vector2i(getImage().width()-1, getImage().height()-1));

    // calculate the convex hull
    vector<Vector2i > result = ConvexHull::convexHull(points);

    // create the polygon
    FieldPercept::FieldPoly fieldPoly;

    for(unsigned int i = 0; i < result.size(); i++)
    {
      fieldPoly.add(result[i]);
    }

    getFieldPercept().setField(fieldPoly, getArtificialHorizon());
    if(fieldPoly.getArea() >= 5600)
    {
      getFieldPercept().valid = true;
    }
    else
    {
      getFieldPercept().valid = false;
    }

    DEBUG_REQUEST( "Vision:Detectors:FieldDetector:mark_field_polygon",
      int idx = static_cast<int>(result.size())-1;
      ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::green : ColorClasses::red;
      for(int i = 0; i < (int)result.size(); i++)
      {
        LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
        idx = i;
      }
    );    
  }
}//end execute
