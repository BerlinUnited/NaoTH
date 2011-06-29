/**
* @file GoalModel.cpp
*
* @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</
* Implementation of class GoalModel
*
*/

#include "GoalModel.h"

const GoalModel::Goal& GoalModel::getTeamGoal(naoth::GameData::TeamColor teamColor) const
{
  switch (teamColor)
  {
  case GameData::red:
    return yellowGoal;
    break;
  case GameData::blue:
    return blueGoal;
    break;
  default:
    ASSERT(false);
  }
  return blueGoal;
}

GoalModel::Goal& GoalModel::getTeamGoal(naoth::GameData::TeamColor teamColor)
{
  switch (teamColor)
  {
  case GameData::red:
    return yellowGoal;
    break;
  case GameData::blue:
    return blueGoal;
    break;
  default:
    ASSERT(false);
  }
  return blueGoal;
}//end getTeamGoal

void GoalModel::print(ostream& stream) const
{
  stream<<"== Blue Goal ==\n";
  stream<<blueGoal.frameInfoWhenGoalLastSeen;
  stream<<"LPost = "<<blueGoal.leftPost<<'\n';
  stream<<"RPost = "<<blueGoal.rightPost<<'\n';

  stream<<"== Yellow Goal ==\n";
  stream<<yellowGoal.frameInfoWhenGoalLastSeen;
  stream<<"LPost = "<<yellowGoal.leftPost<<'\n';
  stream<<"RPost = "<<yellowGoal.rightPost<<'\n';
}//end print

void GoalModel::draw() const
{
  FIELD_DRAWING_CONTEXT;
  PEN("0000FF", 50);
  CIRCLE(blueGoal.leftPost.x, blueGoal.leftPost.y, 50);
  CIRCLE(blueGoal.rightPost.x, blueGoal.rightPost.y, 50);

  PEN("FFFF00", 50);
  CIRCLE(yellowGoal.leftPost.x, yellowGoal.leftPost.y, 50);
  CIRCLE(yellowGoal.rightPost.x, yellowGoal.rightPost.y, 50);
}//end draw

LocalGoalModel::LocalGoalModel() : someGoalWasSeen(false), seen_angle(0.0)
{

}

void GoalModel::calculateBlueByYellow(double xLength)
{
  Vector2<double> tmp = yellowGoal.rightPost;
  tmp -= yellowGoal.leftPost;
  tmp = tmp.normalize();

  Vector2<double> tmp2(tmp.y, -tmp.x);
  tmp2 *= xLength;
  blueGoal.leftPost = yellowGoal.rightPost;
  blueGoal.leftPost += tmp2;

  blueGoal.rightPost = yellowGoal.leftPost;
  blueGoal.rightPost += tmp2;
}//end calculateBlueByYellow

void GoalModel::calculateYellowByBlue(double xLength)
{
  Vector2<double> tmp = blueGoal.rightPost;
  tmp -= blueGoal.leftPost;
  tmp = tmp.normalize();
  Vector2<double> tmp2(tmp.y, -tmp.x);
  tmp2 *= xLength;

  yellowGoal.leftPost = blueGoal.rightPost;
  yellowGoal.leftPost += tmp2;

  yellowGoal.rightPost = blueGoal.leftPost;
  yellowGoal.rightPost += tmp2;
}//end calculateYellowByBlue


Pose2D GoalModel::calculatePose(ColorClasses::Color opponentGoalColor, const FieldInfo& fieldInfo) const
{
  Vector2<double> leftOpponentGoalPosition(fieldInfo.xPosOpponentGoal, fieldInfo.yPosLeftGoalpost);
  Vector2<double> rightOpponentGoalPosition(fieldInfo.xPosOpponentGoal, fieldInfo.yPosRightGoalpost);

  Vector2<double> leftModeledOpponentGoalPosition;
  Vector2<double> rightModeledOpponentGoalPosition;

  if (opponentGoalColor == ColorClasses::skyblue)
  {
    leftModeledOpponentGoalPosition = blueGoal.leftPost;
    rightModeledOpponentGoalPosition = blueGoal.rightPost;
  }else
  {
    leftModeledOpponentGoalPosition = yellowGoal.leftPost;
    rightModeledOpponentGoalPosition = yellowGoal.rightPost;
  }//end else

  double rotation = (leftModeledOpponentGoalPosition-rightModeledOpponentGoalPosition).angle() - Math::pi_2;
  rotation = Math::normalize(-rotation);

  Vector2<double> posLeft = leftOpponentGoalPosition - leftModeledOpponentGoalPosition.rotate(rotation);
  Vector2<double> posRight = rightOpponentGoalPosition - rightModeledOpponentGoalPosition.rotate(rotation);

  Pose2D pose;
  pose.translation = (posLeft + posRight)*0.5;
  pose.rotation = rotation;

  return pose;
}//end calculatePosition

void LocalGoalModel::print(ostream& stream) const
{
  stream<<"someGoalWasSeen = "<<(someGoalWasSeen?"true":"false")<< endl;
  stream<<"estimatedCenterOfGoal = " << seen_center << endl;
  stream<<"estimatedAngleToGoal = " << seen_angle << endl;

  GoalModel::print(stream);
}//end print

void SelfLocGoalModel::update(naoth::GameData::TeamColor ownColor, const Pose2D& robotPose, const FieldInfo& fieldInfo)
{

  Goal& ownGoal = getTeamGoal(ownColor);
  Goal& oppGoal = getTeamGoal(!ownColor);

  // transform the goal posts to the local coordinates according to the robotPose
  ownGoal.leftPost = robotPose/fieldInfo.ownGoalPostLeft;
  ownGoal.rightPost = robotPose/fieldInfo.ownGoalPostRight;

  oppGoal.leftPost = robotPose/fieldInfo.opponentGoalPostLeft;
  oppGoal.rightPost = robotPose/fieldInfo.opponentGoalPostRight;
}//end update

