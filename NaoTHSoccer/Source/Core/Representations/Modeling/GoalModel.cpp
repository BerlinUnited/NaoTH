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

const GoalModel::Goal& GoalModel::getOwnGoal(double global_angle) const
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    return goal;
}//end getOwnGoal

GoalModel::Goal& GoalModel::getOwnGoal(double global_angle)
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    return goal;
}//end getOwnGoal

const GoalModel::Goal& GoalModel::getOppGoal(double global_angle) const
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    return goal;
}//end getOppGoal

GoalModel::Goal& GoalModel::getOppGoal(double global_angle)
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    return goal;
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
  stream<<"== Seen Goal ==\n";
  stream<< goal.frameInfoWhenGoalLastSeen;
  stream<<"LPost = "<<goal.leftPost<<'\n';
  stream<<"RPost = "<<goal.rightPost<<'\n';

  /*Goal anotherGoal;
  calculateAnotherGoal(goal, anotherGoal, getFieldInfo().xLength);

  stream<<"== Calculated Goal ==\n";
  stream<<"LPost = "<<anotherGoal.leftPost<<'\n';
  stream<<"RPost = "<<anotherGoal.rightPost<<'\n';*/
}//end print

void GoalModel::draw() const
{
  FIELD_DRAWING_CONTEXT;
  PEN("FF0000", 50);
  CIRCLE(goal.leftPost.x, goal.leftPost.y, 50);
  CIRCLE(goal.rightPost.x, goal.rightPost.y, 50);

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

  Goal modeledOpponentGoal = getOppGoal(global_angle);

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

void SelfLocGoalModel::update(double global_angle, const Pose2D& robotPose, const FieldInfo& fieldInfo)
{
  GoalModel::Goal ownGoal = getOwnGoal(global_angle);
  GoalModel::Goal oppGoal = getOppGoal(global_angle);

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

