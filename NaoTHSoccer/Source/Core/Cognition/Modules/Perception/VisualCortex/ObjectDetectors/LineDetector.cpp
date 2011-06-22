/**
* @file LineDetector.cpp
*
* Implementation of class LineDetector
*
*/

#include <vector>

#include "LineDetector.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"


LineDetector::LineDetector()
{

  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:expand_lines", "mark the pixels touched during the line extension", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:mark_line_segments", "mark the line candidates on the image", false);
 
  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:mark_lines", "mark the expanded line candidates on the image", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:estimate_corners", "...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:line_clusters", "mark the clustered edgels", false);

  // not finished ...
  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:mark_circle", "mark the middle circle in the image", false);
  
}


//////////////////////////////////////////////////////////
// edgels -> line segment -> circle & line segments ->  //
// expanded  lines -> merged lines                      //
//////////////////////////////////////////////////////////
void LineDetector::execute() 
{
  cameraBrighness = getCurrentCameraSettings().data[CameraSettings::Brightness];
  if(cameraBrighness <= 0 || cameraBrighness > 255)
  {
    edgelBrightnessLevel = BRIGHTNESS;
  }
  else
  {
    edgelBrightnessLevel = (unsigned int) (cameraBrighness / 2);
  }
  edgelBrightnessLevel = BRIGHTNESS;


  // TODO: copy the edgels (do we need it?)
  // maybe it should be in edgel detector
  vector<Edgel> edgelList;
  edgelList.reserve(MAX_NUMBER_OF_SCANLINE_EDGELS);
  for(unsigned int i = 0; i < getScanLineEdgelPercept().numOfSeenEdgels; i++)
  {
    if 
    (
      getScanLineEdgelPercept().scanLineEdgels[i].valid && 
      getScanLineEdgelPercept().scanLineEdgels[i].end.x != 0 && 
      getScanLineEdgelPercept().scanLineEdgels[i].end.y != 0
    )
    {
      edgelList.push_back(getScanLineEdgelPercept().scanLineEdgels[i]);
    }
  }//end for


  STOPWATCH_START("LineDetector ~ find lines");
  vector<LinePercept::FieldLineSegment> lineSegments;
  lineSegments = getLines(edgelList);
  STOPWATCH_STOP("LineDetector ~ find lines");

  DEBUG_REQUEST("ImageProcessor:LineDetector:mark_line_segments",
    for (unsigned int i = 0; i < lineSegments.size(); i++)
    {
      if (!lineSegments[i].valid) continue;
      LINE_PX(ColorClasses::red, (int) lineSegments[i].begin().x, (int) lineSegments[i].begin().y, (int) lineSegments[i].end().x, (int) lineSegments[i].end().y);
      CIRCLE_PX(ColorClasses::white, (int) lineSegments[i].begin().x, (int) lineSegments[i].begin().y, (int) ceil(lineSegments[i].thickness / 2));
      CIRCLE_PX(ColorClasses::gray, (int) lineSegments[i].end().x, (int) lineSegments[i].end().y, (int) ceil(lineSegments[i].thickness / 2));
      
      LINE_PX(ColorClasses::red,(2*i)+1,1,(2*i)+1,6);
    }//end for
  );
  
  STOPWATCH_START("LineDetector ~ expand lines");
  expandLines(lineSegments);
  STOPWATCH_STOP("LineDetector ~ expand lines");

  STOPWATCH_START("LineDetector ~ estimate corners");
  estimateCorners(lineSegments);
  STOPWATCH_STOP("LineDetector ~ estimate corners");

  STOPWATCH_START("LineDetector ~ classify intersections");
  classifyIntersections(lineSegments);
  STOPWATCH_STOP("LineDetector ~ classify intersections");

  // TEST
  //analyzeEndPoints();

  // copy the line segments to the percept
  setLinePercepts(lineSegments);


  // marc the lines surface in image
  DEBUG_REQUEST("ImageProcessor:LineDetector:mark_lines",
    for (unsigned int i = 0; i < getLinePercept().lines.size(); i++)
    {
      if (!getLinePercept().lines[i].valid) continue;
      LinePercept::FieldLineSegment line = getLinePercept().lines[i];
      
      Vector2<double> d(0.0, ceil(line.thickness / 2.0));
      //d.rotate(Math::pi_2 - line.angle);

      Vector2<int> lowerLeft(getLinePercept().lines[i].begin() - d);
      Vector2<int> upperLeft(getLinePercept().lines[i].begin() + d);
      Vector2<int> lowerRight(getLinePercept().lines[i].end() - d);
      Vector2<int> upperRight(getLinePercept().lines[i].end() + d);
      LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y);
      LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, upperLeft.x, upperLeft.y);
      LINE_PX(ColorClasses::green, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y);
      LINE_PX(ColorClasses::green, lowerRight.x, lowerRight.y, upperRight.x, upperRight.y);

      LINE_PX(ColorClasses::green,(2*i)+1,7,(2*i)+1,12);
    }//end for
  );

}//end execute

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void LineDetector::setLinePercepts(vector<LinePercept::FieldLineSegment>& lineSegments)
{
  //register valid lines in percept
  for (unsigned int i = 0; i < lineSegments.size(); i++)
  {
    if (lineSegments[i].valid)
    {
      getLinePercept().lines.push_back(lineSegments[i]);
    }
  }
}//end setLinePercepts


// TEST
void LineDetector::analyzeEndPoints()
{
  ScanLineEdgelPercept::EndPoint closestPoint;

  Vector2<int> startImage;
  Vector2<int> endImage;

  bool firstFounding = true;

  for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& endPoint = getScanLineEdgelPercept().endPoints[i];

    if(endPoint.color == ColorClasses::skyblue || endPoint.color == ColorClasses::yellow)
    {
      if(firstFounding)
      {
        startImage = endPoint.posInImage;
        closestPoint = endPoint;
        firstFounding = false;
      }
      else
      {
        endImage = endPoint.posInImage;
        if(closestPoint.posOnField.abs() > endPoint.posOnField.abs())
          closestPoint = endPoint;
      } 
    }//end if
  }//end for

  if((startImage - endImage).abs() == 0) return;
  
  Edgel edgelStart;
  Edgel edgelEnd;
  for(unsigned int i = 0; i < getScanLineEdgelPercept().numOfSeenEdgels; i++)
  {
    const Edgel& edgel = getScanLineEdgelPercept().scanLineEdgels[i];

    if(i == 0 || (edgel.center - startImage).abs() < (edgelStart.center - startImage).abs())
      edgelStart = edgel;

    if(i == 0 || (edgel.center - endImage).abs() < (edgelEnd.center - endImage).abs())
      edgelEnd = edgel;
  }//end for

  CIRCLE_PX(ColorClasses::black, edgelStart.center.x, edgelStart.center.y,5);
  CIRCLE_PX(ColorClasses::gray, edgelEnd.center.x, edgelEnd.center.y,5);
  
  FIELD_DRAWING_CONTEXT;
  PEN("0000FF",10);
  CIRCLE(closestPoint.posOnField.x, closestPoint.posOnField.y, 50);
}//end analyzeEndPoints


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void LineDetector::expandLines(vector<LinePercept::FieldLineSegment>& lineSegments)
{
  bool expandLines = false;
  DEBUG_REQUEST("ImageProcessor:LineDetector:expand_lines",
    expandLines = true;
  );
  // make smaller lines invalid 
  // if an near line is bigger and has nearly the same in direction
  // and if the difference of both hessian distances is smaller/equal than the line thickness
  for (unsigned int i = 0; i < lineSegments.size(); i++)
  {
    for(unsigned int j = 0; j < i; j++)
    {
      if (!lineSegments[j].valid)
      {
        continue;
      }
      Vector2<double> lineNormal = Vector2<double>(lineSegments[i].getDirection()).rotateLeft();
      double distanceDifference = fabs(lineNormal * Vector2<double>(lineSegments[j].getBase()) - lineNormal * Vector2<double>(lineSegments[i].getBase()));
      if(distanceDifference <= lineSegments[i].thickness && fabs(lineSegments[i].angle - lineSegments[j].angle) < MAX_LINE_ANGLE_DIFF)
      {
        double iLength = lineSegments[i].getLength();
        double jLength = lineSegments[j].getLength();
        if(iLength >= jLength)
        {
          lineSegments[j].valid = false;
        }
        else
        {
          lineSegments[i].valid = false;
        }
      }
    }//end for
  }
  //scan along inside the line to expand it
  for (unsigned int i = 0; i < lineSegments.size(); i++)
  {
    if (!lineSegments[i].valid)
    {
      continue;
    }
    Math::LineSegment line = lineSegments[i];

    Vector2<int> lineOnePointUpper
    (
      (int) line.end().x, 
      (int) (line.end().y + floor(lineSegments[i].thickness / 3))
    );
    Vector2<int> lineOnePointMiddle(line.end());
    Vector2<int> lineOnePointLower
    (
      (int) line.end().x, 
      (int) (line.end().y - floor(lineSegments[i].thickness / 3))
    );
    
    Vector2<int> lineTwoPointUpper
    (
      (int) line.begin().x, 
      (int) (line.begin().y + floor(lineSegments[i].thickness / 3))
    );
    Vector2<int> lineTwoPointMiddle(line.begin());
    Vector2<int> lineTwoPointLower
    (
      (int) line.begin().x, 
      (int) (line.begin().y - floor(lineSegments[i].thickness / 3))
    );
    
    Vector2<int> direction = lineOnePointMiddle - lineTwoPointMiddle;//(line.getDirection());
    
    BresenhamLineScan scanLineOneUpper(lineOnePointUpper, direction, getImage().cameraInfo);
    BresenhamLineScan scanLineOneMiddle(lineOnePointMiddle, direction, getImage().cameraInfo);
    BresenhamLineScan scanLineOneLower(lineOnePointLower, direction, getImage().cameraInfo);

    BresenhamLineScan scanLineTwoUpper(lineTwoPointUpper, -direction, getImage().cameraInfo);
    BresenhamLineScan scanLineTwoMiddle(lineTwoPointMiddle, -direction, getImage().cameraInfo);
    BresenhamLineScan scanLineTwoLower(lineTwoPointLower, -direction, getImage().cameraInfo);

    bool lineOneScanned = false;
    bool lineTwoScanned = false;

    ColorClasses::Color color;

    while (!lineOneScanned || !lineTwoScanned)
    { 
      if(!lineOneScanned) 
      {
        if(expandLines)
        {
          color = ColorClasses::pink;
        }
        else
        {
          color = ColorClasses::numOfColors;
        }
        if(lineSegments[i].thickness <= 3)
        {
          lineOneScanned = scanAlongLine(lineOnePointMiddle, scanLineOneMiddle, color);
        }
        else
        {
          lineOneScanned = 
          scanAlongLine(lineOnePointUpper, scanLineOneUpper, color) ||
          scanAlongLine(lineOnePointLower, scanLineOneLower, color);
        }
      }//endif
      if(!lineTwoScanned)
      {
        if(expandLines)
        {
          color = ColorClasses::pink;
        }
        else
        {
          color = ColorClasses::numOfColors;
        }
        if(lineSegments[i].thickness <= 3)
        {
          lineTwoScanned = scanAlongLine(lineTwoPointMiddle, scanLineTwoMiddle, color);
        }
        else
        {
          lineTwoScanned = 
          scanAlongLine(lineTwoPointUpper, scanLineTwoUpper, color) ||
          scanAlongLine(lineTwoPointLower, scanLineTwoLower, color);
        }
      }//end if
    }//end while
    if(lineSegments[i].thickness > 3)
    {
      lineTwoPointMiddle.x = lineTwoPointUpper.x;
      lineTwoPointMiddle.y = lineTwoPointLower.y + (lineTwoPointUpper.y - lineTwoPointLower.y) / 2;
      lineOnePointMiddle.x = lineOnePointUpper.x;
      lineOnePointMiddle.y = lineOnePointLower.y + (lineOnePointUpper.y - lineOnePointLower.y) / 2;
    }//end if
    lineSegments[i].set(getCameraMatrix(), getImage().cameraInfo,lineTwoPointMiddle, lineOnePointMiddle);
  }//end for
}//end expandLines

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
bool LineDetector::scanAlongLine(Vector2<int>& linePoint, BresenhamLineScan& scanLine, ColorClasses::Color markColor)
{
  if(scanLine.getNext(linePoint))
  {
    if( !getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).isInside(linePoint) )
    {
      scanLine.getLast(linePoint);
      return true;
    }
    Pixel pixel = getImage().get(linePoint.x,linePoint.y);
    ColorClasses::Color thisPixelColor = getColorTable64().getColorClass(pixel);
    int g = (int)(pixel.y - 0.3456 * (pixel.u - 128) - 0.71448 * (pixel.v - 128));
    if(g < 0) g = 0; else if(g > 255) g = 255;
    if((g > GREENAMOUNT && pixel.y < edgelBrightnessLevel) || thisPixelColor == ColorClasses::green)
    {
      return true;
    }
    if(markColor != ColorClasses::numOfColors)
    {
      POINT_PX(markColor, linePoint.x, linePoint.y);
    }
    return false;
  }
  return true;
}//scanAlongLine


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void LineDetector::estimateCorners(vector<LinePercept::FieldLineSegment>& lineSegments)
{
  bool estimate_corners = false;
  DEBUG_REQUEST("ImageProcessor:LineDetector:estimate_corners",
    estimate_corners = true;
  );

  // for corners we nee more than one line ...
  if(lineSegments.size() < 2) return;


  for (unsigned int k = 0; k < lineSegments.size(); k++)
  {
    if (!lineSegments[k].valid) 
    {
      continue;
    }
    const Math::LineSegment& lineOne = lineSegments[k];

    // intersect with remaining lines segments
    for (unsigned int i = k + 1; i < lineSegments.size(); i++)
    {
      if (!lineSegments[i].valid)
      {
        continue;
      }
      const Math::LineSegment& lineTwo = lineSegments[i];
      
      double tOneL = lineOne.Line::intersection(lineTwo);
      double tTwoL = lineTwo.Line::intersection(lineOne);

      // intersection between the theoretical lines
      Vector2<double> point = lineOne.Line::point(tOneL);
      Vector2<int> intersectionPoint((int)(point.x+0.5), (int)(point.y+0.5));
      
      // "projection" of the intersection to the segments
      Vector2<int> lineOnePoint(lineOne.point(tOneL));
      Vector2<int> lineTwoPoint(lineTwo.point(tTwoL));

      // project the intersection on te ground
      // TODO: do it somewhere else
      LinePercept::Intersection intersection(getCameraMatrix(), getImage().cameraInfo, point);


      // needed later ...
      double tOne = Math::clamp(tOneL, 0.0, lineOne.getLength());
      double tTwo = Math::clamp(tTwoL, 0.0, lineTwo.getLength());


      BresenhamLineScan scanLineOne(lineOnePoint, intersectionPoint);
      BresenhamLineScan scanLineTwo(lineTwoPoint, intersectionPoint);
      bool lineOneScanned = false;
      bool lineTwoScanned = false;
      while (!lineOneScanned || !lineTwoScanned)
      { 
        if(!lineOneScanned) 
        {
          if(estimate_corners)
          {
            lineOneScanned = scanAlongLine(lineOnePoint, scanLineOne, ColorClasses::orange);
          }
          else
          {
            lineOneScanned = scanAlongLine(lineOnePoint, scanLineOne, ColorClasses::numOfColors);
          }
        }//endif
        if(!lineTwoScanned)
        {
          if(estimate_corners)
          {
            lineTwoScanned = scanAlongLine(lineTwoPoint, scanLineTwo, ColorClasses::skyblue);
          }
          else
          {
            lineTwoScanned = scanAlongLine(lineTwoPoint, scanLineTwo, ColorClasses::numOfColors);
          }
        }//end if
      }//end while

      ////if(Geometry::imagePixelToFieldCoord(getCameraMatrix(), getImage().cameraInfo,
      ////  point.x, point.y, 0.0, intersection.pos))
      if
      (
        scanLineOne.pixelCount >= scanLineOne.numberOfPixels 
        && 
        scanLineTwo.pixelCount >= scanLineTwo.numberOfPixels
      )
      {
        if
        ( // TODO: is this necessary here?
          lineOnePoint.x >= 0 &&
          lineOnePoint.y >= 0 &&
          lineOnePoint.x < (int) getImage().cameraInfo.resolutionWidth &&
          lineOnePoint.y < (int) getImage().cameraInfo.resolutionHeight 
        )
        {    
          if(tOne < 1.5 * lineSegments[i].thickness)
          {
            lineSegments[k].set(getCameraMatrix(), getImage().cameraInfo, lineOnePoint,  lineSegments[k].end());
          }
          else if(tOne > lineSegments[k].getLength() - 1.5 * lineSegments[i].thickness)
          {
            lineSegments[k].set(getCameraMatrix(), getImage().cameraInfo, lineSegments[k].begin(), lineOnePoint);
          }
        }//end if
        if
        ( // TODO: is this necessary here?
          lineTwoPoint.x >= 0 &&
          lineTwoPoint.y >= 0 &&
          lineTwoPoint.x < (int) getImage().cameraInfo.resolutionWidth &&
          lineTwoPoint.y < (int) getImage().cameraInfo.resolutionHeight 
        )
        {
          if(tTwo < 1.5 * lineSegments[k].thickness)
          {
            lineSegments[i].set(getCameraMatrix(), getImage().cameraInfo, lineTwoPoint,  lineSegments[i].end());
          }
          else if(tTwo > lineSegments[i].getLength() - 1.5 * lineSegments[k].thickness)
          {
            lineSegments[i].set(getCameraMatrix(), getImage().cameraInfo, lineSegments[i].begin(), lineTwoPoint);
          }
        }//end if

        intersection.setSegments(k, i, tOne, tTwo);
        getLinePercept().intersections.push_back(intersection);
      }//end if
      ////lineOne = lineTwo;
    }//end for
  }//end for
}//end estimateCorners

void LineDetector::classifyIntersections(vector<LinePercept::FieldLineSegment>& lineSegments)
{

  vector<Vector2<double> > circlePoints;
  circlePoints.reserve(getLinePercept().intersections.size());

  vector<Vector2<double> > circleMiddlePoints;
  circleMiddlePoints.reserve(getLinePercept().intersections.size());

  // the drawing context is needed for furthrr drawings
  DEBUG_REQUEST("ImageProcessor:LineDetector:mark_circle",
    FIELD_DRAWING_CONTEXT;
  );

//  cout << endl << "----------------" << endl;

  for(unsigned int i = 0; i < getLinePercept().intersections.size(); i++)
  {
    Vector2<unsigned int> indices = getLinePercept().intersections[i].getSegmentIndices();
    LinePercept::FieldLineSegment segOne = lineSegments[indices[0]];
    LinePercept::FieldLineSegment segTwo = lineSegments[indices[1]];

    double angleDiff = fabs(Math::normalize(segOne.lineOnField.getDirection().angle() - segTwo.lineOnField.getDirection().angle()));

    if(angleDiff < MAX_LINE_ANGLE_DIFF)
    {
      getLinePercept().intersections[i].setType(Math::Intersection::E);
    }
    else if(angleDiff > MAX_LINE_ANGLE_DIFF * 2 && angleDiff < Math::pi_2 - MAX_FIELDLINE_ANGLE_DIFF )
    {
      getLinePercept().intersections[i].setType(Math::Intersection::C);
    }
    else if(angleDiff > Math::pi_2 - MAX_FIELDLINE_ANGLE_DIFF && angleDiff < Math::pi_2 + MAX_FIELDLINE_ANGLE_DIFF )
    {
      Math::Intersection::IntersectionType type = getLinePercept().intersections[i].getType();
      if
      (
        type != Math::Intersection::C &&
        type != Math::Intersection::L &&
        type != Math::Intersection::T &&
        type != Math::Intersection::X 
      )
      {
        Vector2<double> segmentDistancesToIntersection = getLinePercept().intersections[i].getSegmentsDistancesToIntersection();
        bool tIntersectsOne = segmentDistancesToIntersection[0] > segTwo.thickness && segmentDistancesToIntersection[0] < segOne.getLength() - segTwo.thickness;
        bool tIntersectsTwo = segmentDistancesToIntersection[1] > segOne.thickness && segmentDistancesToIntersection[1] < segTwo.getLength() - segOne.thickness;
//        cout << segmentDistancesToIntersection;
        if(tIntersectsOne && tIntersectsTwo)
        {
          //maybe X intersection, because if not its overwritten with L or so
          getLinePercept().intersections[i].setType(Math::Intersection::X);
        }else if(tIntersectsOne || tIntersectsTwo)
        {
          //should be an T intersection
          getLinePercept().intersections[i].setType(Math::Intersection::T);
//          cout << " T" << endl;
        }
        else
        {
          //only L intersection
          getLinePercept().intersections[i].setType(Math::Intersection::L);
//          cout << " L" << endl;
        }
      }
    }//end if
    int crossCount = 0;
    for(unsigned int j = 0; j < getLinePercept().intersections.size(); j++)
    {
      if(j == i) continue;
      if((getLinePercept().intersections[i].getPos() - getLinePercept().intersections[j].getPos()).abs() < 5)
      {
        crossCount++;
      }
    }//end for
    if(crossCount > 0)
    {
      getLinePercept().intersections[i].setType(Math::Intersection::T);
    }
    if(crossCount > 1)
    {
      getLinePercept().intersections[i].setType(Math::Intersection::X);
    }

    if( getLinePercept().intersections[i].getType() == Math::Intersection::C)
    {
      Vector2<double> middlePointOne = (segOne.lineOnField.begin() + segOne.lineOnField.end()) / 2;
      Vector2<double> middlePointTwo = (segTwo.lineOnField.begin() + segTwo.lineOnField.end()) / 2;

      Math::Line lineNormalOne(middlePointOne, segOne.lineOnField.getDirection().rotateLeft());
      Math::Line lineNormalTwo(middlePointTwo, segTwo.lineOnField.getDirection().rotateLeft());

      double p = lineNormalOne.intersection(lineNormalTwo);
      if(p != std::numeric_limits<double>::infinity())
      {
        Vector2<double> intersectPoint = lineNormalOne.point(p);
        DEBUG_REQUEST("ImageProcessor:LineDetector:mark_circle",
          PEN("FFFFFF", 5); 
          LINE(middlePointOne.x, middlePointOne.y, intersectPoint.x, intersectPoint.y);
          LINE(middlePointTwo.x, middlePointTwo.y, intersectPoint.x, intersectPoint.y);

          PEN("FF0000", 30); 
          CIRCLE(middlePointOne.x, middlePointOne.y, 10);
          CIRCLE(middlePointTwo.x, middlePointTwo.y, 10);
          CIRCLE(getLinePercept().intersections[i].getPosOnField().x, getLinePercept().intersections[i].getPosOnField().y, 10);
        );
        circlePoints.push_back(middlePointOne);
        circlePoints.push_back(middlePointTwo);
        circlePoints.push_back(getLinePercept().intersections[i].getPosOnField());
        circleMiddlePoints.push_back(intersectPoint);

        // mark the lines as circle lines
        Vector2<unsigned int> lineIdx = getLinePercept().intersections[i].getSegmentIndices();
        lineSegments[lineIdx[0]].partOfCircle = true;
        lineSegments[lineIdx[1]].partOfCircle = true;
      }//end if
    }//end if


    DEBUG_REQUEST("ImageProcessor:LineDetector:estimate_corners",
      //mark intersection in the image
      const Vector2<int>& point = getLinePercept().intersections[i].getPos();
      CIRCLE_PX((ColorClasses::Color) getLinePercept().intersections[i].getType(), point.x, point.y, 5);
    );
  }//end for


  STOPWATCH_START("LineDetector ~ detect circle");
  if(circlePoints.size() > 3)
  {
    Vector2<double> middle;
    for(unsigned int i = 0; i < circleMiddlePoints.size(); i++)
    {
      middle += circleMiddlePoints[i];
    }
    middle = middle / circleMiddlePoints.size();

    Math::Matrix_mxn<double> A(circlePoints.size(), 3);
    Math::Matrix_mxn<double> l(circlePoints.size(), 1);
    for(unsigned int i = 0; i < circlePoints.size(); i++)
    {
      /*
      A(i, 0) = circlePoints[i].abs2();
      A(i, 1) = 2 * circlePoints[i].x;
      A(i, 2) = 2 * circlePoints[i].y;
      l(i, 0) = -1;
      */
      A(i, 0) = circlePoints[i].x;
      A(i, 1) = circlePoints[i].y;
      A(i, 2) = 1.0;
      l(i, 0) = - circlePoints[i].abs2();
    }//end for
    
    Math::Matrix_mxn<double> AT(A.transpose());
    Math::Matrix_mxn<double> result(((AT * A).invert() * AT) * l);

    //Vector2<double> middle1(-result(1, 0) / result(0, 0), -result(2, 0) / result(0, 0));
    Vector2<double> middle1(-result(0, 0)*0.5, -result(1, 0)*0.5);
  
    if( (middle - middle1).abs() < 150) // 15cm
    {
      getLinePercept().middleCircleWasSeen = true;
      getLinePercept().middleCircleCenter = (middle + middle1) / 2;
    }//end if

    DEBUG_REQUEST("ImageProcessor:LineDetector:mark_circle",
      PEN("FF000099", 10);
      CIRCLE(middle.x, middle.y, 50);
      PEN("0000FF99", 10);
      CIRCLE(middle1.x, middle1.y, 50);
    );
  }//end if

  // TODO: make it more efficient
  // check if the center line is seen
  if(getLinePercept().middleCircleWasSeen)
  {
    for(unsigned int i = 0; i < lineSegments.size(); i++)
    {
      if(!lineSegments[i].partOfCircle)
      {
        double d = lineSegments[i].lineOnField.minDistance(getLinePercept().middleCircleCenter);
        if(d < 300.0)
        {
          lineSegments[i].seen_id = LinePercept::FieldLineSegment::center_line;
          // estimate the orientation of the circle
          // TODO: treat the case if there are several lines
          getLinePercept().middleCircleOrientationWasSeen = true;
          getLinePercept().middleCircleOrientation = lineSegments[i].lineOnField.getDirection();
        }
      }
    }//end for


    DEBUG_REQUEST("ImageProcessor:LineDetector:mark_circle",
      const Vector2<double>& center = getLinePercept().middleCircleCenter;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);

      if(getLinePercept().middleCircleOrientationWasSeen)
      {
        const Vector2<double> direction = getLinePercept().middleCircleOrientation*(getFieldInfo().centerCircleRadius+100);
        LINE(
          center.x + direction.x,
          center.y + direction.y,
          center.x - direction.x,
          center.y - direction.y
          );
      }//end if
    );
  }//end if
  STOPWATCH_STOP("LineDetector ~ detect circle");

}//end classifyIntersections


//////////////////////////////////////////////////////////
// clusters given edgels to line segments, verifies    //
// the segments and writes to given line array      //
//////////////////////////////////////////////////////////
vector<LinePercept::FieldLineSegment> LineDetector::getLines(vector<Edgel>& edgelList)
{
  vector<LinePercept::FieldLineSegment> lineSegments;
  
  if(edgelList.empty()) return lineSegments;
  
  vector<ClusteredLine> lineClusters;
  lineClusters.reserve(20);

  
  lineClusters.push_back(ClusteredLine(edgelList[0],0));
  DEBUG_REQUEST("ImageProcessor:LineDetector:line_clusters",
    CIRCLE_PX((ColorClasses::Color) (0) , edgelList[0].center.x, edgelList[0].center.y, 5);
  );

  for(unsigned int i = 1; i < edgelList.size(); i++)
  {
    if(edgelList[i].valid)
    {
      bool matchingClusterFound = false;
      unsigned int clusterIndex = 0;

      while(clusterIndex < lineClusters.size() && !matchingClusterFound)
      {
        if(lineClusters[clusterIndex].add(edgelList[i]))
        {
          matchingClusterFound = true;
          DEBUG_REQUEST("ImageProcessor:LineDetector:line_clusters",
            int idx = ((lineClusters[clusterIndex].id() ) % (unsigned int)ColorClasses::numOfColors);
            CIRCLE_PX(
              (ColorClasses::Color) (idx) , 
              edgelList[i].center.x, edgelList[i].center.y, 
              5 + idx);
          );
        }
        clusterIndex++;
      }//end while

      if(!matchingClusterFound)
      {
        ClusteredLine newCluster(edgelList[i], lineClusters.size());
        DEBUG_REQUEST("ImageProcessor:LineDetector:line_clusters",
          int idx = ((newCluster.id() ) % (unsigned int)ColorClasses::numOfColors);
          CIRCLE_PX(
            (ColorClasses::Color) (idx) , 
            edgelList[i].center.x, edgelList[i].center.y, 
            5 + idx);
        );
        lineClusters.push_back(newCluster);
      }//end if

    }//end if
  }//end for


  // create lines from clusters
  lineSegments.reserve(lineClusters.size());
  // distance between scanlines
  double delta_x = (double)(getImage().cameraInfo.resolutionWidth - 1) / (SCANLINE_COUNT);

  for(unsigned int i = 0; i < lineClusters.size(); i++ )
  {
    const ClusteredLine& cluster = lineClusters[i];

    // TODO: check this stuff
    // calculate the weght of the segment
    // length of the line segment
    Vector2<double> lineVector(cluster.end - cluster.start);
    double length = lineVector.abs();
    // 0 < weight <= 1
    // weight == 1, only if the segment has an edgel on every scannline
    double weight = (delta_x*cluster.count)/length;

    if(cluster.count > 1 && cluster.thickness > 1 && weight > 0.5)
    {
      LinePercept::FieldLineSegment lineSegment;
      lineSegment.set(getCameraMatrix(), getImage().cameraInfo, cluster.start, cluster.end);
      lineSegment.thickness = cluster.thickness; //(int) ceil(cluster.thickness / 2);
      lineSegment.valid = true;
      lineSegment.type = LinePercept::FieldLineSegment::unknown;
      lineSegment.angle = lineVector.angle();
      lineSegments.push_back(lineSegment);
    }
  }//end for

  return lineSegments;
}//end getLinesNew

