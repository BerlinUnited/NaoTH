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
  if(getLinePercept().lines.size() > 1)
    correct_the_line_percept(getLinePercept().lines[0].lineInImage.segment, getLinePercept().lines[1].lineInImage.segment);

}//end execute


void PerceptProjector::correct_the_line_percept(
  const Math::LineSegment& lineOne,
  const Math::LineSegment& lineTwo) const
{
  Vector2<double> offset;
  const double epsylon = 1e-8;

  // 
  const double minStepSize = Math::fromDegrees(0.1);

  // 
  for (int i = 0; i < 10; i++)
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
    if(w == -1) return;

    ASSERT(w > 0 && !Math::isNan(w));

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = dg * (-w / (dg * dg));
    offset += z_GN;

    // correction step too small
    if(z_GN.abs() < minStepSize) break;
  }//end for


  // apply the correction to the posts
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateX(offset.x)
       .rotateY(offset.y);

  
  DEBUG_REQUEST("PerceptProjector:DrawCorrectedLinePercept",
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

}//end correct_the_line_percept



double PerceptProjector::projectionErrorLine(
  double offsetX,
  double offsetY,
  const Math::LineSegment& lineOne,
  const Math::LineSegment& lineTwo) const
{
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateX(offsetX)
       .rotateY(offsetY);

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

  // just for emergency cases
  if(dirOne.abs() < 100 || dirTwo.abs() < 100) return -1;

  double cos_angle = dirOne*dirTwo;
  double error = Math::sqr(cos_angle);

  return error;
}//end projectionErrorLine

