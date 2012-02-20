/**
 * @file WholeGoalLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
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
  getSensingGoalModel().someGoalWasSeen = false;

  // negative odometry
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  lastRobotOdometry = getOdometryData();

  // update by odometry
  getSensingGoalModel().goal.leftPost = odometryDelta * getSensingGoalModel().goal.leftPost;
  getSensingGoalModel().goal.rightPost = odometryDelta * getSensingGoalModel().goal.rightPost;

  // TODO: check this condition
  //check for GoalPercept distance not near 30cm (for wrong projections)
  bool getOut = false;
  for (unsigned int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
  {
    if (getGoalPercept().getPost(i).position.abs() < 300) // TODO: check 300
    {
      getOut = true;
      break;
    }
  }//end for

  if (!getOut)
  {
    for (unsigned int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
    {
      //18.02.2012 - commented because of same colored goal
      //introduce a switch
      //TODO for diff colored goal games
      // two posts of the same color are seen
      /*if(i+1 < getGoalPercept().getNumberOfSeenPosts() &&
         getGoalPercept().getPost(i).color == getGoalPercept().getPost(i+1).color)
      {
         checkAndInsertSingleGoal(getGoalPercept().getPost(i), getGoalPercept().getPost(i+1));
         //i++;
      }else if(getGoalPercept().getPost(i).type != GoalPercept::GoalPost::unknownPost)
      {
        // ...
      }*/
        if(i+1 < getGoalPercept().getNumberOfSeenPosts())
        {
            checkAndInsertSingleGoal(getGoalPercept().getPost(i), getGoalPercept().getPost(i+1));
            std::cout << "check " << std::endl;

            i++;
        }
    }//end for
  }//end if


  //Draw GoalModel
  DEBUG_REQUEST("WholeGoalLocator:drawGoalModel",
    FIELD_DRAWING_CONTEXT;
    Vector2<double> centerGoal = getSensingGoalModel().goal.calculateCenter();
    PEN("0000FF", 50);
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


void WholeGoalLocator::checkAndInsertSingleGoal(
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
    return;


  GoalPercept::GoalPost postLeft =
    (post1.type == GoalPercept::GoalPost::leftPost) ? post1 : post2;
  GoalPercept::GoalPost postRight =
    (post1.type == GoalPercept::GoalPost::rightPost) ? post1 : post2;

  // one left and one right post?
  if(postLeft.type != GoalPercept::GoalPost::leftPost ||
     postRight.type != GoalPercept::GoalPost::rightPost)
     return;


  // correct the posts
  DEBUG_REQUEST("WholeGoalLocator:correct_the_goal_percept",
    correct_the_goal_percept(postLeft, postRight);
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

  // both goal posts should have the same color...
    /*18.02.2012
      because of same colored goals
if(postLeft.color == ColorClasses::yellow && postRight.color == ColorClasses::yellow )
  {
    if(getFieldSidePercept().facedFieldSide == FieldSidePercept::own)
    {
      getSensingGoalModel().yellowGoal.leftPost = postLeft.position;
      getSensingGoalModel().yellowGoal.rightPost = postRight.position;
      getSensingGoalModel().calculateBlueByYellow(getFieldInfo().xLength);
      getSensingGoalModel().yellowGoal.frameInfoWhenGoalLastSeen = getFrameInfo();
      getSensingGoalModel().someGoalWasSeen = true;
    }
    //FIXME never reached?
    else if(getFieldSidePercept().facedFieldSide == FieldSidePercept::opponent)
    {
      getSensingGoalModel().blueGoal.leftPost = postLeft.position;
      getSensingGoalModel().blueGoal.rightPost = postRight.position;
      getSensingGoalModel().calculateYellowByBlue(getFieldInfo().xLength);
      getSensingGoalModel().blueGoal.frameInfoWhenGoalLastSeen = getFrameInfo();
      getSensingGoalModel().someGoalWasSeen = true;
    }
    else
    {
      // TODO: what?!
    }

  }//end if
*/

    getSensingGoalModel().goal.leftPost  = postLeft.position;
    getSensingGoalModel().goal.rightPost = postRight.position;
    getSensingGoalModel().someGoalWasSeen = true;

}//end checkAndInsertSingleGoal



void WholeGoalLocator::correct_the_goal_percept(
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

  Vector2<double> offset;
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

  tmpCM.rotateX(offset.x)
       .rotateY(offset.y);

  // project the goal posts
  const CameraInfoParameter& cameraInfo = Platform::getInstance().theCameraInfo;

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

  tmpCM.rotateX(offsetX)
       .rotateY(offsetY);

  // project the goal posts
  const CameraInfoParameter& cameraInfo = Platform::getInstance().theCameraInfo;

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
  

  /*
  //TODO: IN PROGRESS
  // percived position
  double rotation = (leftPosition-rightPosition).angle() - Math::pi_2;
  rotation = Math::normalize(-rotation);

  Vector2<double> posLeft = getFieldInfo().opponentGoalPostLeft - leftPosition.rotate(rotation);
  Vector2<double> posRight = getFieldInfo().opponentGoalPostRight - rightPosition.rotate(rotation);

  Pose2D pose;
  pose.translation = (posLeft + posRight)*0.5;
  pose.rotation = rotation;

  //
  double error = 
    (getFieldInfo().opponentGoalPostLeft - pose*leftPosition).abs() +
    (getFieldInfo().opponentGoalPostRight - pose*rightPosition).abs();
  */

  return error;
}//end projectionError
