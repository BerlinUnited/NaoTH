/**
 * @file WholeGoalLocator.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class WholeGoalLocator
 */

#ifndef _GoalModelCorrector_h_
#define _GoalModelCorrector_h_

// debug
//#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugDrawings.h"

#include "Tools/CameraGeometry.h"

// Representations
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/CameraInfo.h"

class GoalModelCorrector
{

public:
  void correct_the_goal_percept_on_field(
    const CameraMatrix& cameraMatrix,
    const naoth::CameraInfo& cameraInfo,
    const double goalWidth,
    Vector2<double>& postOnField1,
    Vector2<double>& postOnField2,
    Vector2<double>& offset);

  void correct_the_goal_percept_in_image(
    const CameraMatrix& cameraMatrix,
    const naoth::CameraInfo& cameraInfo,
    const double goalWidth,
    const Vector2<double>& postInImage1,
    const Vector2<double>& postInImage2,
    Vector2<double>& offset);

private:
  double projectionError(
    double offsetX,
    double offsetY,
    const double goalWidth,
    const CameraMatrix& cameraMatrix,
    const naoth::CameraInfo& cameraInfo,
    const Vector2<double>& postInImage1,
    const Vector2<double>& postInImage2);
};

#endif //_GoalModelCorrector_h_
