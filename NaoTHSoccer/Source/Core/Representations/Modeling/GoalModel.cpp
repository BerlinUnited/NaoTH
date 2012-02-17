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

const GoalModel::Goal& GoalModel::getTeamGoal(double global_angle) const
{
  
}//end getTeamGoal

GoalModel::Goal& GoalModel::getTeamGoal(double global_angle)
{
  
}//end getTeamGoal

const GoalModel::Goal& GoalModel::getOppGoal(double global_angle) const
{
  
}//end getOppGoal

GoalModel::Goal& GoalModel::getOppGoal(double global_angle)
{
  
}//end getOppGoal

/*
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
*/

void GoalModel::print(ostream& stream) const
{
  stream<<"== Goal One ==\n";
  stream<<goalOne.frameInfoWhenGoalLastSeen;
  stream<<"LPost = "<<goalOne.leftPost<<'\n';
  stream<<"RPost = "<<goalOne.rightPost<<'\n';

  stream<<"== Goal Two ==\n";
  stream<<goalTwo.frameInfoWhenGoalLastSeen;
  stream<<"LPost = "<<goalTwo.leftPost<<'\n';
  stream<<"RPost = "<<goalTwo.rightPost<<'\n';
}//end print

void GoalModel::draw() const
{
  FIELD_DRAWING_CONTEXT;
  PEN("FFFF00", 50);
  CIRCLE(goalOne.leftPost.x, goalOne.leftPost.y, 50);
  CIRCLE(goalOne.rightPost.x, goalOne.rightPost.y, 50);

  PEN("FF00FF", 50);
  CIRCLE(goalTwo.leftPost.x, goalTwo.leftPost.y, 50);
  CIRCLE(goalTwo.rightPost.x, goalTwo.rightPost.y, 50);
}//end draw

LocalGoalModel::LocalGoalModel() 
  : 
  someGoalWasSeen(false), 
  opponentGoalIsValid(false),
  ownGoalIsValid(false),
  seen_angle(0.0)
{

}

void GoalModel::calculateAnotherGoal(const GoalModel::Goal& one, GoalModel::Goal& another, double distance)
{
  Vector2<double> normal = one.rightPost - one.leftPost;
  normal.normalize(distance);
  normal.rotateRight();

  another.leftPost = one.rightPost + normal;
  another.rightPost = one.leftPost + normal;
}//end calculateAnotherGoal



Pose2D GoalModel::calculatePose(double global_angle, const FieldInfo& fieldInfo) const
{
  const Vector2<double>& leftOpponentGoalPosition = fieldInfo.opponentGoalPostLeft;
  const Vector2<double>& rightOpponentGoalPosition = fieldInfo.opponentGoalPostRight;

  Goal modeledOpponentGoal = getOppGoal(..);

  // TODO: make it nicer with Vector2 operations
  double rotation = (modeledOpponentGoal.leftPost-modeledOpponentGoal.rightPost).angle() - Math::pi_2;
  rotation = Math::normalize(-rotation);

  // TODO: introduce const method Vector2<>.rotate()
  Vector2<double> posLeft = leftOpponentGoalPosition - modeledOpponentGoal.leftPost.rotate(rotation);
  Vector2<double> posRight = rightOpponentGoalPosition - modeledOpponentGoal.rightPost.rotate(rotation);

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

void SelfLocGoalModel::update(const Pose2D& robotPose, const FieldInfo& fieldInfo)
{
  Goal& ownGoal = goalOne; //getTeamGoal(ownColor);
  Goal& oppGoal = goalTwo; //getTeamGoal(!ownColor);

  // transform the goal posts to the local coordinates according to the robotPose
  ownGoal.leftPost = robotPose/fieldInfo.ownGoalPostLeft;
  ownGoal.rightPost = robotPose/fieldInfo.ownGoalPostRight;

  oppGoal.leftPost = robotPose/fieldInfo.opponentGoalPostLeft;
  oppGoal.rightPost = robotPose/fieldInfo.opponentGoalPostRight;
}//end update

SensingGoalModel::SensingGoalModel()
  : 
  someGoalWasSeen(false)
{
}

