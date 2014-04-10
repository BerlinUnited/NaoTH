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
  DEBUG_REQUEST_REGISTER("Vision:Detectors:FieldDetector:mark_corrected_field_polygon", "mark polygonal boundary of the detected field cutted by horizon on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:FieldDetector:setHoleImageAsField", "mark hole image as if field were detected", false);
}


FieldDetector::~FieldDetector()
{
}


void FieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  bool simulateField = false;

  DEBUG_REQUEST("Vision:Detectors:FieldDetector:setHoleImageAsField",
    simulateField = true;
  );

  if(simulateField)
  {
    vector<Vector2i > result;
    result.push_back(Vector2i(0,0));
    result.push_back(Vector2i(0,getImage().height()-1));
    result.push_back(Vector2i(getImage().width()-1, getImage().height()-0));
    result.push_back(Vector2i(getImage().width()-1,0));
    result.push_back(Vector2i(0,0));

    // create the polygon
    FieldPercept::FieldPoly fieldPoly;
    for(size_t i = 0; i < result.size(); i++)
    {
      fieldPoly.add(result[i]);
    }
    Math::LineSegment horizon(Vector2i(0, 0), Vector2i(getImage().width() - 1, 0));

    getFieldPercept().setField(fieldPoly, horizon);
    if(fieldPoly.getArea() >= 5600)
    {
      getFieldPercept().valid = true;
    }

    DEBUG_REQUEST( "Vision:Detectors:FieldDetector:mark_field_polygon",
      size_t idx = 0;
      ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::green : ColorClasses::red;
      for(size_t i = 1; i < result.size(); i++)
      {
        LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
        idx = i;
      }
    );    
  }
  else if(getScanLineEdgelPercept().endPoints.size() > 0)
  {
    vector<Vector2i > points;//(getScanLineEdgelPercept().endPoints.size());

    for(size_t i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      const Vector2i& p = getScanLineEdgelPercept().endPoints[i].posInImage;
      if(p.y < (int) getImage().height() - 6)
      {
        points.push_back(p);
      }
      //points[i] = getScanLineEdgelPercept().endPoints[i].posInImage;
    }

    // move the outer points
    //points.front().x = 0;
    //points.back().x = getImage().width()-1;

    // lower image points
    //points.push_back(Vector2i(0,getImage().height()-1));
    //points.push_back(Vector2i(getImage().width()-1, getImage().height()-1));

    if(points.size() > 1)
    {
      Vector2i p1 = points.front();
      p1.y = getImage().height() - 1;
      Vector2i p2 = points.back();
      p2.y = getImage().height() - 1;

      points.push_back(p1);
      points.push_back(p2);

      if(getScanLineEdgelPercept().endPoints.front().posInImage.y < (int) getImage().height() - 6)
      {
        points.push_back(Vector2i(0, getScanLineEdgelPercept().endPoints.front().posInImage.y));
        points.push_back(Vector2i(0, getImage().height() - 1));
      }

      if(getScanLineEdgelPercept().endPoints.back().posInImage.y < (int) getImage().height() - 6)
      {
        points.push_back(Vector2i(getImage().width() - 1, getScanLineEdgelPercept().endPoints.back().posInImage.y));
        points.push_back(Vector2i(getImage().width() - 1, getImage().height() - 1));
      }

    }
    // calculate the convex hull
    vector<Vector2i > result = ConvexHull::convexHull(points);

    // create the polygon
    FieldPercept::FieldPoly fieldPoly;

    for(size_t i = 0; i < result.size(); i++)
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
      size_t idx = 0;
      ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::black : ColorClasses::red;
      for(size_t i = 1; i < result.size(); i++)
      {
        LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
        idx = i;
      }
    );    

    DEBUG_REQUEST( "Vision:Detectors:FieldDetector:mark_corrected_field_polygon",
      int idx = 0;
      ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::green : ColorClasses::red;
      const FieldPercept::FieldPoly& fieldpoly = getFieldPercept().getValidField();
      for(int i = 1; i < fieldpoly.length; i++)
      {
        LINE_PX(color, fieldpoly[idx].x, fieldpoly[idx].y, fieldpoly[i].x, fieldpoly[i].y);
        idx = i;
      }
    );    
  }
}//end execute
