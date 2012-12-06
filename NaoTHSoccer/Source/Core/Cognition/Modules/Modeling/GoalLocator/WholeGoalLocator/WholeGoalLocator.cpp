/**
 * @file WholeGoalLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of class WholeGoalLocator
 */

#include "Tools/Math/Pose2D.h"


#include "WholeGoalLocator.h"
#include "Cognition/Modules/Perception/VisualCortex/ObjectDetectors/FieldDetector.h"

WholeGoalLocator::WholeGoalLocator()
{
  DEBUG_REQUEST_REGISTER("WholeGoalLocator:drawPost", "draw the modelled post on the field", false);
  DEBUG_REQUEST_REGISTER("WholeGoalLocator:drawGoalModel", "draw the modelled goal on the field", false);
  DEBUG_REQUEST_REGISTER("WholeGoalLocator:drawRobotPose", "draw the robots position estimated by the goal", false);

  DEBUG_REQUEST_REGISTER("WholeGoalLocator:correct_the_goal_percept", "correct the camera matrix when the whole goal is seen", true);
}

void WholeGoalLocator::execute()
{
  // reset the model
  getSensingGoalModel().someGoalWasSeen = false;
  getCameraMatrixOffset().offsetByGoalModel = Vector2<double>();

  // negative odometry
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  lastRobotOdometry = getOdometryData();

  // update by odometry
  getSensingGoalModel().goal.leftPost = odometryDelta * getSensingGoalModel().goal.leftPost;
  getSensingGoalModel().goal.rightPost = odometryDelta * getSensingGoalModel().goal.rightPost;

  // TODO: check this condition
  //check for GoalPercept distance not near 30cm (for wrong projections)
  

  // tray to find a pair of posts which forma goal
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts()-1; i++)
  {
    for (int j = i+1; j < getGoalPercept().getNumberOfSeenPosts(); j++)
    {
      if(checkAndCalculateSingleGoal(getGoalPercept().getPost(i), getGoalPercept().getPost(j)))
      {
        break; // a goal was found
      }
    }//end for
  }//end for


  //Draw GoalModel
  DEBUG_REQUEST("WholeGoalLocator:drawGoalModel",
    FIELD_DRAWING_CONTEXT;
    Vector2<double> centerGoal = getSensingGoalModel().goal.calculateCenter();
    PEN(ColorClasses::colorClassToHex(getSensingGoalModel().goal.color), 50);
    CIRCLE(getSensingGoalModel().goal.leftPost.x, getSensingGoalModel().goal.leftPost.y, 50);
    CIRCLE(getSensingGoalModel().goal.rightPost.x, getSensingGoalModel().goal.rightPost.y, 50);
    PEN("0000AA", 50);
    CIRCLE(centerGoal.x, centerGoal.y, 50);
  );

  DEBUG_REQUEST("WholeGoalLocator:drawRobotPose",
    FIELD_DRAWING_CONTEXT;
    PEN(ColorClasses::yellow, 50);
    Pose2D pose = getSensingGoalModel().calculatePose(getCompassDirection(), getFieldInfo());
    ROBOT(pose.translation.x,
          pose.translation.y,
          pose.rotation);
  );
}//end execute


bool WholeGoalLocator::checkAndCalculateSingleGoal(
  const GoalPercept::GoalPost& post1,
  const GoalPercept::GoalPost& post2)
{
  /**************************************
   *  sanity check
   **************************************/
  // check field position
  if( post1.position.abs() > (getFieldInfo().xFieldLength * 1.50) ||
      post2.position.abs() > (getFieldInfo().xFieldLength * 1.50) ||
  // only reliable posts for now
      !post1.positionReliable ||
      !post2.positionReliable ||
  // HACK: there is a bug in goal detector!!!
      (post1.position - post2.position).abs() < 300)
    return false;

  // posts have the same color
  if(post1.color != post2.color)
    return false;

  GoalPercept::GoalPost postLeft =
    (post1.type == GoalPercept::GoalPost::leftPost) ? post1 : post2;
  GoalPercept::GoalPost postRight =
    (post1.type == GoalPercept::GoalPost::rightPost) ? post1 : post2;

  // one left and one right post?
  if(postLeft.type != GoalPercept::GoalPost::leftPost ||
     postRight.type != GoalPercept::GoalPost::rightPost)
     return false;


  // correct the posts
  DEBUG_REQUEST("WholeGoalLocator:correct_the_goal_percept",
    correct_the_goal_percept(getCameraMatrixOffset().offsetByGoalModel, postLeft, postRight);
  );

  DEBUG_REQUEST("WholeGoalLocator:drawPost",
    FIELD_DRAWING_CONTEXT;
    PEN(ColorClasses::colorClassToHex(post1.color), 50);
    CIRCLE(post1.position.x, post1.position.y, 50);
    PEN(ColorClasses::colorClassToHex(post2.color), 50);
    CIRCLE(post2.position.x, post2.position.y, 50);
  );

  /**************************************
   *  update goal model
   **************************************/

  // set the goal model
  getSensingGoalModel().goal.color = post1.color;
  getSensingGoalModel().goal.leftPost  = postLeft.position;
  getSensingGoalModel().goal.rightPost = postRight.position;
  getSensingGoalModel().someGoalWasSeen = true;

  return true;
}//end checkAndInsertSingleGoal



void WholeGoalLocator::correct_the_goal_percept(
  Vector2<double>& offset,
  GoalPercept::GoalPost& post1,
  GoalPercept::GoalPost& post2)
{
  // calibrate the camera matrix
  // currently it is in test-mode, the correction parameter
  // are stored as static members of CameraMatrixCalculator

  if (getGoalPercept().getNumberOfSeenPosts() < 2)
    return;

  if(!post1.positionReliable || !post2.positionReliable)
    return;

  //Vector2<double> offset;
  double epsylon = 1e-8;

  // make only one step
  for (int i = 0; i < 10; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = projectionError(offset.x + epsylon, offset.y, post1, post2);
    double dg12 = projectionError(offset.x - epsylon, offset.y, post1, post2);
    double dg1 = (dg11 - dg12) / (2 * epsylon);

    double dg21 = projectionError(offset.x, offset.y + epsylon, post1, post2);
    double dg22 = projectionError(offset.x, offset.y - epsylon, post1, post2);
    double dg2 = (dg21 - dg22) / (2 * epsylon);

    // Dg(x)^T*Dg(x)
    Vector2<double> dg(dg1, dg2);

    if (dg.abs() < epsylon) break;

    // g(x) - target
    double w = projectionError(offset.x, offset.y, post1, post2);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = dg * (-w / (dg * dg));
    offset += z_GN;
  }//end for


  // apply the correction to the posts
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateY(offset.y)
       .rotateX(offset.x);

  // project the goal posts
  const CameraInfo& cameraInfo = getCameraInfo();

  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      post1.basePoint.x,
      post1.basePoint.y,
      0.0,
      post1.position);

  Vector2<double> rightPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      post2.basePoint.x,
      post2.basePoint.y,
      0.0,
      post2.position);

}//end correct_the_goal_percept


double WholeGoalLocator::projectionError(
  double offsetX,
  double offsetY,
  const GoalPercept::GoalPost& post1,
  const GoalPercept::GoalPost& post2)
{
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateY(offsetY)
       .rotateX(offsetX);

  // project the goal posts
  const CameraInfo& cameraInfo = getCameraInfo();

  Vector2<double> leftPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      post1.basePoint.x,
      post1.basePoint.y,
      0.0,
      leftPosition);

  Vector2<double> rightPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM,
      cameraInfo,
      post2.basePoint.x,
      post2.basePoint.y,
      0.0,
      rightPosition);

  
  double goal_width = (getFieldInfo().opponentGoalPostLeft-getFieldInfo().opponentGoalPostRight).abs2();
  double seen_width = (leftPosition-rightPosition).abs2();
  double error = Math::sqr(goal_width - seen_width);

  return error;
}//end projectionError
