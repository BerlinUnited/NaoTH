/**
* @file OdometrySymbols2012.cpp
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Implementation of class OdometrySymbols2012
*/

#include "OdometrySymbols2012.h"
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugDrawings.h>

OdometrySymbols2012* OdometrySymbols2012::theInstance = NULL;

void OdometrySymbols2012::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("odometry.x", &getOdometry_x);
  engine.registerDecimalInputSymbol("odometry.y", &getOdometry_y);
  engine.registerDecimalInputSymbol("odometry.rotation", &getOdometry_angle);

  engine.registerDecimalOutputSymbol("odometry.relative_point.x", &relativePoint.x);
  engine.registerDecimalOutputSymbol("odometry.relative_point.y", &relativePoint.y);


  engine.registerDecimalInputSymbol("odometry.preview.x", &getOdometryPreview_x);
  engine.registerDecimalInputSymbolDecimalParameter("odometry.preview.x", "odometry.preview.x.x", &previewParameter.x);
  engine.registerDecimalInputSymbolDecimalParameter("odometry.preview.x", "odometry.preview.x.y", &previewParameter.y);

  engine.registerDecimalInputSymbol("odometry.preview.y", &getOdometryPreview_y);
  engine.registerDecimalInputSymbolDecimalParameter("odometry.preview.y", "odometry.preview.y.x", &previewParameter.x);
  engine.registerDecimalInputSymbolDecimalParameter("odometry.preview.y", "odometry.preview.y.y", &previewParameter.y);


  DEBUG_REQUEST_REGISTER("OdometrySymbols2012:relativePoint", "draw the relativePoint", false);
}//end registerSymbols


void OdometrySymbols2012::execute()
{
  Pose2D odometryDelta = lastRobotOdometry - odometryData;
  lastRobotOdometry = odometryData;

  relativePoint = odometryDelta * relativePoint;

  DEBUG_REQUEST("OdometrySymbols2012:relativePoint",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);
    FILLOVAL(relativePoint.x, relativePoint.y, 25, 25);
  );
}//end update

double OdometrySymbols2012::getOdometry_x()
{
  return theInstance->odometryData.translation.x;
}//end get x translation

double OdometrySymbols2012::getOdometry_y()
{
  return theInstance->odometryData.translation.y;
}//end get y translation

double OdometrySymbols2012::getOdometry_angle()
{
  return theInstance->odometryData.rotation;
}//end get rotation

double OdometrySymbols2012::getOdometryPreview_x()
{
  return (theInstance->motionStatus.plannedMotion.hip/theInstance->previewParameter).x;
}//end get getOdometryPreview_x

double OdometrySymbols2012::getOdometryPreview_y()
{
  return (theInstance->motionStatus.plannedMotion.hip/theInstance->previewParameter).y;
}//end get getOdometryPreview_y

