/* 
 * File:   FieldDetector.cpp
 * Author: Heinrich Mellmann
 * 
 */

#include "FieldDetector.h"

#include <Tools/Math/ConvexHull.h>
#include <Tools/Math/Polygon.h>
#include <vector>

#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugRequest.h"

FieldDetector::FieldDetector()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:mark_field", "mark polygonal boundary of the detected field on the image", false);
}


FieldDetector::~FieldDetector()
{
}


void FieldDetector::execute()
{
  getFieldPercept().reset();

  if(getScanLineEdgelPercept().endPoints.size() > 0)
  {
    vector<Vector2<int> > points(getScanLineEdgelPercept().endPoints.size());

    for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      points[i] = getScanLineEdgelPercept().endPoints[i].posInImage;
    }

    // move the outer points
    points.front().x = 0;
    points.back().x = getImage().cameraInfo.resolutionWidth-1;

    // lower image points
    points.push_back(Vector2<int>(0,getImage().cameraInfo.resolutionHeight-1));
    points.push_back(Vector2<int>(getImage().cameraInfo.resolutionWidth-1, getImage().cameraInfo.resolutionHeight-1));

    // calculate the convex hull
    vector<Vector2<int> > result = ConvexHull::convexHull(points);

    // create the polygon
    FieldPercept::FieldPoly fieldPoly;
    for(unsigned int i = 0; i < result.size(); i++)
    {
      fieldPoly.add(result[i]);
    }

    getFieldPercept().add(fieldPoly, getFieldPercept().getFullFieldRect());
    getFieldPercept().setValid(true);

    DEBUG_REQUEST( "ImageProcessor:FieldDetector:mark_field",
      int idx = result.size()-1;
      for(unsigned int i = 0; i < result.size(); i++)
      {
        LINE_PX(ColorClasses::green, result[idx].x, result[idx].y, result[i].x, result[i].y);
        idx = i;
      }
    );
  }
}//end execute
