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
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:mark_field_polygon_top", "mark polygonal boundary of the detected field on the image", false);
}


FieldDetector::~FieldDetector()
{
}


void FieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  if(getScanLineEdgelPercept_().endPoints.size() > 0)
  {
    vector<Vector2<int> > points(getScanLineEdgelPercept_().endPoints.size());

    for(unsigned int i = 0; i < getScanLineEdgelPercept_().endPoints.size(); i++)
    {
      points[i] = getScanLineEdgelPercept_().endPoints[i].posInImage;
    }

    // move the outer points
    points.front().x = 0;
    points.back().x = getImage_().cameraInfo.resolutionWidth-1;

    // lower image points
    points.push_back(Vector2<int>(0,getImage_().cameraInfo.resolutionHeight-1));
    points.push_back(Vector2<int>(getImage_().cameraInfo.resolutionWidth-1, getImage_().cameraInfo.resolutionHeight-1));

    // calculate the convex hull
    vector<Vector2<int> > result = ConvexHull::convexHull(points);

    // create the polygon
    FieldPercept::FieldPoly fieldPoly;

    for(unsigned int i = 0; i < result.size(); i++)
    {
      fieldPoly.add(result[i]);
    }

    getFieldPercept_().setPoly(fieldPoly, getArtificialHorizon_());
    if(fieldPoly.getArea() >= 5600)
    {
      getFieldPercept_().setValid(true);
    }
    else
    {
      getFieldPercept_().setValid(false);
    }

    if(cameraID == CameraInfo::Top)
    {
      DEBUG_REQUEST( "ImageProcessor:FieldDetector:mark_field_polygon_top",
        int idx = result.size()-1;
        ColorClasses::Color color = getFieldPercept_().isValid() ? ColorClasses::green : ColorClasses::red;
        for(unsigned int i = 0; i < result.size(); i++)
        {
          LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
          idx = i;
        }
      );
    }
    else
    {
      DEBUG_REQUEST( "ImageProcessor:FieldDetector:mark_field_polygon",
        int idx = result.size()-1;
        ColorClasses::Color color = getFieldPercept_().isValid() ? ColorClasses::green : ColorClasses::red;
        for(unsigned int i = 0; i < result.size(); i++)
        {
          LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
          idx = i;
        }
      );
    }
  }
}//end execute
