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
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:TopCam:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:BottomCam:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
}


FieldDetector::~FieldDetector()
{
}


void FieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
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

    getFieldPercept().setPoly(fieldPoly, getArtificialHorizon());
    if(fieldPoly.getArea() >= 5600)
    {
      getFieldPercept().setValid(true);
    }
    else
    {
      getFieldPercept().setValid(false);
    }

    DEBUG_REQUEST( "ImageProcessor:FieldDetector:TopCam:mark_field_polygon",
      if(cameraID == CameraInfo::Top)
      {
        int idx = result.size()-1;
        ColorClasses::Color color = getFieldPercept().isValid() ? ColorClasses::green : ColorClasses::red;
        for(unsigned int i = 0; i < result.size(); i++)
        {
          TOP_LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
          idx = i;
        }
      }
    );
    DEBUG_REQUEST( "ImageProcessor:FieldDetector:BottomCam:mark_field_polygon",
      if(cameraID == CameraInfo::Bottom)
      {
        int idx = result.size()-1;
        ColorClasses::Color color = getFieldPercept().isValid() ? ColorClasses::green : ColorClasses::red;
        for(unsigned int i = 0; i < result.size(); i++)
        {
          LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
          idx = i;
        }
      }
    );    
  }
}//end execute
