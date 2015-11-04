/* 
 * File:   FieldDetector.cpp
 * Author: Heinrich Mellmann
 * 
 */

#include "FieldDetector.h"

#include <Tools/Math/ConvexHull.h>

using namespace std;

FieldDetector::FieldDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_corrected_field_polygon", "mark polygonal boundary of the detected field cutted by horizon on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldDetector:setHoleImageAsField", "mark hole image as if field were detected", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_field_polygon_old", "mark polygonal boundary of the detected field before outlier detection", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_field_polygon_new", "mark new polygonal boundary of the detected field after outlier detection", false);

  getDebugParameterList().add(&theParameters);
}


FieldDetector::~FieldDetector()
{}


void FieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  bool simulateField = false;

  DEBUG_REQUEST("Vision:FieldDetector:setHoleImageAsField",
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

    DEBUG_REQUEST( "Vision:FieldDetector:mark_field_polygon",
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
    vector<Vector2i> result = ConvexHull::convexHull(points);
    
    // create the polygon
    FieldPercept::FieldPoly fieldPoly;

    for(size_t i = 0; i < result.size(); i++)
    {
      fieldPoly.add(result[i]);
    }
    DEBUG_REQUEST("Vision:FieldDetector:mark_field_polygon_old",
      int idx = 0;
      for(int i = 1; i < fieldPoly.length; i++)
      {
        LINE_PX(ColorClasses::red, fieldPoly[idx].x, fieldPoly[idx].y, fieldPoly[i].x, fieldPoly[i].y);
        idx = i;
      }
    );    

    // sort points by x value
    sort(points.begin(), points.end(), this->myVecCompareX);

    // remove points on the edge of the BodyContour
    std::vector<size_t> badPoints;
    for(size_t i = 2; i+2 < points.size(); i++)
    {
      if(points[i].y > 0)
      {
        Vector2i dummyPoint = points[i];
        dummyPoint.y += 6;
        if(getBodyContour().isOccupied(dummyPoint))
        {
          badPoints.push_back(i);
        }
      }
    }
    if(badPoints.size() > 0)
    {
      for(size_t i = 0; i < badPoints.size(); i++)
      {
        // badPoints are ordered so the small indices are removed first
        points.erase(points.begin() + (badPoints[i] - i));
      }
    }
    // check outliers but keep first and last point in any case
    for(size_t nLoop = 0; nLoop < 5; nLoop++)
    {
      badPoints.clear();
      for(size_t i = 2; i+2 < points.size(); i++)
      {
        std::vector<Vector2i> pointsCheck = points;
        pointsCheck.erase(pointsCheck.begin()+i);
        
        vector<Vector2i> resultCheck = ConvexHull::convexHull(pointsCheck);
        
        FieldPercept::FieldPoly fieldPolyCheck;
        for(size_t j = 0; j < resultCheck.size(); j++)
        {
          fieldPolyCheck.add(resultCheck[j]);
        }
        if(fieldPolyCheck.getArea() / fieldPoly.getArea() < theParameters.pruneThresholdArea)
        {
          badPoints.push_back(i);
        }
      }
      // remove outliers
      if(badPoints.size() > 0)
      {
        for(size_t i = 0; i < badPoints.size(); i++)
        {
          // badPoints are ordered so the small indices are removed first
          points.erase(points.begin() + badPoints[i] - i);
        }
        result = ConvexHull::convexHull(points);
        // clear old polygon
        fieldPoly = FieldPercept::FieldPoly();
        for(size_t i = 0; i < result.size(); i++)
        {
          fieldPoly.add(result[i]);
        }
      } else
      {
        break;
      }
    }
    DEBUG_REQUEST("Vision:FieldDetector:mark_field_polygon_new",
      int idx = 0;
      for(int i = 1; i < fieldPoly.length; i++)
      {
        LINE_PX(ColorClasses::green, fieldPoly[idx].x, fieldPoly[idx].y, fieldPoly[i].x, fieldPoly[i].y);
        idx = i;
      }
    );    

    // add field to percept
    getFieldPercept().setField(fieldPoly, getArtificialHorizon());
    // check result
    if(fieldPoly.getArea() >= 5600)
    {
      getFieldPercept().valid = true;
    }
    else
    {
      getFieldPercept().valid = false;
    }

    DEBUG_REQUEST( "Vision:FieldDetector:mark_field_polygon",
      size_t idx = 0;
      ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::black : ColorClasses::red;
      for(size_t i = 1; i < result.size(); i++)
      {
        LINE_PX(color, result[idx].x, result[idx].y, result[i].x, result[i].y);
        idx = i;
      }
    );    

    DEBUG_REQUEST( "Vision:FieldDetector:mark_corrected_field_polygon",
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

bool FieldDetector::myVecCompareX(const Vector2i &first, const Vector2i &second) 
{ 
  return (first.x<second.x); 
}
