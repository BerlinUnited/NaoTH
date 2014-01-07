
#include "GoalModelCorrector.h"


using namespace naoth;

void GoalModelCorrector::correct_the_goal_percept_on_field(
  const CameraMatrix& cameraMatrix,
  const CameraInfo& cameraInfo,
  const double goalWidth,
  Vector2d& postOnField1,
  Vector2d& postOnField2,
  Vector2<double>& offset)
{
  // project the goal postst to image with the given camera matrix
  // NOTE: return if no projection possible
  Vector2<double> postInImage1;
  if(!CameraGeometry::relativePointToImage(
    cameraMatrix,cameraInfo,
    Vector3d(postOnField1.x,postOnField1.y,0.0), postInImage1))
  {
    return;
  }

  Vector2<double> postInImage2;
  if(!CameraGeometry::relativePointToImage(
    cameraMatrix,cameraInfo,
    Vector3d(postOnField2.x,postOnField2.y,0.0), postInImage2))
  {
    return;
  }

  //
  correct_the_goal_percept_in_image(
    cameraMatrix,cameraInfo,goalWidth,
    postInImage1,postInImage2,offset);


  // apply the correction to the posts
  CameraMatrix tmpCM(cameraMatrix);

  tmpCM.rotateY(offset.y)
       .rotateX(offset.x);

  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      postInImage1,
      0.0,
      postOnField1);

  Vector2<double> rightPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      postInImage2,
      0.0,
      postOnField2);
}


void GoalModelCorrector::correct_the_goal_percept_in_image(
  const CameraMatrix& cameraMatrix,
  const CameraInfo& cameraInfo,
  const double goalWidth,
  const Vector2<double>& postInImage1,
  const Vector2<double>& postInImage2,
  Vector2<double>& offset)
{
  // calibrate the camera matrix

  double epsylon = 1e-8;

  // make only one step
  for (int i = 0; i < 100; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = projectionError(offset.x + epsylon, offset.y, goalWidth, cameraMatrix,cameraInfo, postInImage1, postInImage2);
    double dg12 = projectionError(offset.x - epsylon, offset.y, goalWidth, cameraMatrix,cameraInfo, postInImage1, postInImage2);
    double dg1 = (dg11 - dg12) / (2 * epsylon);

    double dg21 = projectionError(offset.x, offset.y + epsylon, goalWidth, cameraMatrix,cameraInfo, postInImage1, postInImage2);
    double dg22 = projectionError(offset.x, offset.y - epsylon, goalWidth, cameraMatrix,cameraInfo, postInImage1, postInImage2);
    double dg2 = (dg21 - dg22) / (2 * epsylon);

    // Dg(x)^T*Dg(x)
    Vector2<double> dg(dg1, dg2);

    if (dg.abs() < epsylon) break;

    // g(x) - target
    double w = projectionError(offset.x, offset.y, goalWidth, cameraMatrix,cameraInfo, postInImage1, postInImage2);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = dg * (-w / (dg * dg));
    offset += z_GN;
  }//end for
}//end correct_the_goal_percept


double GoalModelCorrector::projectionError(
  double offsetX,
  double offsetY,
  const double goalWidth,
  const CameraMatrix& cameraMatrix,
  const CameraInfo& cameraInfo,
  const Vector2<double>& postInImage1,
  const Vector2<double>& postInImage2)
{
  CameraMatrix tmpCM(cameraMatrix);

  tmpCM.rotateY(offsetY)
       .rotateX(offsetX);

  // project the goal posts
  Vector2<double> postPosition1;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      postInImage1,
      0.0,
      postPosition1);

  Vector2<double> postPosition2;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      postInImage2,
      0.0,
      postPosition2);

  
  double goal_width = goalWidth*goalWidth;
  double seen_width = (postPosition1-postPosition2).abs2();
  double error = Math::sqr(goal_width - seen_width);

  return error;
}//end projectionError