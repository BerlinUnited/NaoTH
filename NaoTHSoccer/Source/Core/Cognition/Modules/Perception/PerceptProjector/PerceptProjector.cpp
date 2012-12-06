/**
* @file PerceptProjector.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class PerceptProjector
*/

#include "PerceptProjector.h"

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/CameraGeometry.h"
#include "Tools/Math/Line.h"

PerceptProjector::PerceptProjector()
{
  DEBUG_REQUEST_REGISTER("PerceptProjector:DrawCorrectedLinePercept", " ", false);
}

PerceptProjector::~PerceptProjector()
{
}

void PerceptProjector::execute()
{
  Vector2<double> offset(getCameraMatrixOffset().offsetByGoalModel);
//  bool updated = false;
  
  if(false && getLinePercept().lines.size() > 1)
  {
    // take two random lines
    // (if they are the same no correction is made)
    int idx1 = Math::random((int)getLinePercept().lines.size());
    int idx2 = Math::random((int)getLinePercept().lines.size());
    
    if(idx1 != idx2 &&
       getLinePercept().lines[idx1].type != LinePercept::C &&
       getLinePercept().lines[idx2].type != LinePercept::C)
    {
      // TODO: select the lines randomized
      Vector2<double> offset_line(offset);
      if(correct_the_line_percept(
          offset_line,
          getLinePercept().lines[idx1].lineInImage.segment, 
          getLinePercept().lines[idx2].lineInImage.segment
          ))
      {
        offset = offset_line;
//        updated = true;
      }
    }
  }
  
  /*
  if(getSensingGoalModel().someGoalWasSeen || updated)
    offsetBuffer.add(getCameraMatrixOffset().offset+offset);
    */

  getCameraMatrixOffset().offset += offset*0.1;

  PLOT("PerceptProjector:offset.x", offset.x);
  PLOT("PerceptProjector:offset.y", offset.y);

  Vector2<double> averageOffset = offsetBuffer.getAverage();

  PLOT("PerceptProjector:averageOffset.x", getCameraMatrixOffset().offset.x);
  PLOT("PerceptProjector:averageOffset.y", getCameraMatrixOffset().offset.y);

  // the buffer is full
  
  if(offsetBuffer.getNumberOfEntries() == offsetBuffer.getMaxEntries())
  {
    offset = averageOffset;
    getCameraMatrixOffset().offset = offset;
  }

  // apply the correction to the percepts
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateY(offset.y)
       .rotateX(offset.x);

  // project the goal posts
  const CameraInfo& cameraInfo = getCameraInfo();

  // apply correction to goal post percept
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
  {
    GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
    CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      post.basePoint.x,
      post.basePoint.y,
      0.0,
      post.position);
  }//end for


  // apply correction to the line percept
  for (int i = 0; i < (int)getLinePercept().lines.size(); i++)
  {
    LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

    Vector2<double> lineBegin;
    CameraGeometry::imagePixelToFieldCoord(
      tmpCM, cameraInfo, 
      linePercept.lineInImage.segment.begin(), 
      0.0, lineBegin);
    
    Vector2<double> lineEnd;
    CameraGeometry::imagePixelToFieldCoord(
      tmpCM, cameraInfo, 
      linePercept.lineInImage.segment.end(), 
      0.0, lineEnd);

    linePercept.lineOnField = Math::LineSegment(lineBegin, lineEnd);
  }//end for


  // apply correction to the corners percept
  for(int i=0; i < (int)getLinePercept().intersections.size(); i++)
  {
    //mark intersection on the field
    LinePercept::Intersection& intersection = getLinePercept().intersections[i];
    
    Vector2<double> pointOnField;
    CameraGeometry::imagePixelToFieldCoord(
      tmpCM, cameraInfo, 
      intersection.getPos(),
      0.0, pointOnField);

    intersection.setPosOnField(pointOnField);
  }//end for

  // apply correction to the ball percept
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM, cameraInfo, 
      getBallPercept().centerInImage, 
      getFieldInfo().ballRadius, 
      getBallPercept().bearingBasedOffsetOnField);

}//end execute



bool PerceptProjector::correct_the_line_percept(
  Vector2<double>& offset,
  const Math::LineSegment& lineOne,
  const Math::LineSegment& lineTwo) const
{
  const double epsylon = 1e-8;

  // 
  const double minStepSize = Math::fromDegrees(0.1);

  // 
  for (int i = 0; i < 100; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = projectionErrorLine(offset.x + epsylon, offset.y, lineOne, lineTwo);
    double dg12 = projectionErrorLine(offset.x - epsylon, offset.y, lineOne, lineTwo);
    double dg1 = (dg11 - dg12) / (2 * epsylon);

    double dg21 = projectionErrorLine(offset.x, offset.y + epsylon, lineOne, lineTwo);
    double dg22 = projectionErrorLine(offset.x, offset.y - epsylon, lineOne, lineTwo);
    double dg2 = (dg21 - dg22) / (2 * epsylon);

    // Dg(x)^T*Dg(x)
    Vector2<double> dg(dg1, dg2);

    // derivative too small steps
    if (dg.abs() < epsylon) break;

    // g(x) - target
    double w = projectionErrorLine(offset.x, offset.y, lineOne, lineTwo);

    // error while calculating projectionErrorLine
    if(w == -1) return false;

    ASSERT(w > 0 && !Math::isNan(w));

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = dg * (-w / (dg * dg));
    offset += z_GN;

    // correction step too small
    if(z_GN.abs() < minStepSize) break;
  }//end for

  
  DEBUG_REQUEST("PerceptProjector:DrawCorrectedLinePercept",
    CameraMatrix tmpCM(getCameraMatrix());

    tmpCM.rotateY(offset.y)
         .rotateX(offset.x);

    const CameraInfo& cameraInfo = getImage().cameraInfo;

    for (unsigned int i = 0; i < getLinePercept().lines.size(); i++)
    {
      const Math::LineSegment& linePercept = getLinePercept().lines[i].lineInImage.segment;

      Vector2<double> lineBegin;
      CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, linePercept.begin(), 0.0, lineBegin);
      Vector2<double> lineEnd;
      CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, linePercept.end(), 0.0, lineEnd);

      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 50);
      LINE(lineBegin.x, lineBegin.y, lineEnd.x, lineEnd.y);
    }//end for
  );

  return true;
}//end correct_the_line_percept



double PerceptProjector::projectionErrorLine(
  double offsetX,
  double offsetY,
  const Math::LineSegment& lineOne,
  const Math::LineSegment& lineTwo) const
{
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateY(offsetY)
       .rotateX(offsetX);

  // project the goal posts
  const CameraInfo& cameraInfo = getImage().cameraInfo;

  Vector2<double> lineOneBegin;
  CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, lineOne.begin(), 0.0, lineOneBegin);
  Vector2<double> lineOneEnd;
  CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, lineOne.end(), 0.0, lineOneEnd);

  Vector2<double> lineTwoBegin;
  CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, lineTwo.begin(), 0.0, lineTwoBegin);
  Vector2<double> lineTwoEnd;
  CameraGeometry::imagePixelToFieldCoord(tmpCM, cameraInfo, lineTwo.end(), 0.0, lineTwoEnd);

  Vector2<double> dirOne = lineOneEnd-lineOneBegin;
  Vector2<double> dirTwo = lineTwoEnd-lineTwoBegin;

  // just for emergency cases: do not use short lines
  // dirOne.abs2() < 40000 <=> dirOne.abs() < 200
  if(dirOne.abs2() < 50000 || dirTwo.abs2() < 50000) return -1;

  double cos_angle = dirOne*dirTwo;
  double error = Math::sqr(cos_angle);

  double cos_angle_orth = dirOne*dirTwo.rotateRight();;
  double error_orth = Math::sqr(cos_angle_orth);

  return min(error, error_orth);
}//end projectionErrorLine

