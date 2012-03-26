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

GoalModel::Goal GoalModel::getOwnGoal(const CompassDirection& compassDirection, const FieldInfo& fieldInfo) const
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    //if (abs(compassDirection.angle) > Math::pi_2 && abs(goal.calculateCenter().angle()) < Math::pi_2) {
    if(fabs(compassDirection.angle + goal.calculateCenter().angle()) > Math::pi_2){
      return goal;
    } else {
      return calculateAnotherGoal(goal, fieldInfo.xLength);
    }
}//end getOwnGoal

GoalModel::Goal GoalModel::getOppGoal(const CompassDirection& compassDirection, const FieldInfo& fieldInfo) const
{
    //TODO check this decision, compare with robotPose.rotation
    //18.02.2012
    if(fabs(compassDirection.angle + goal.calculateCenter().angle()) < Math::pi_2){
      return goal;
    } else {
      return calculateAnotherGoal(goal, fieldInfo.xLength);
    }
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

GoalModel::Goal GoalModel::calculateAnotherGoal(const GoalModel::Goal& goal, double distance)
{
  Vector2<double> normal = goal.rightPost - goal.leftPost;
  GoalModel::Goal another;

  normal.normalize(distance);
  normal.rotateRight();

  // swich the posts
  another.leftPost = goal.rightPost + normal;
  another.rightPost = goal.leftPost + normal;

  return another;
}//end calculateAnotherGoal



Pose2D GoalModel::calculatePose(const CompassDirection& compassDirection, const FieldInfo& fieldInfo) const
{
  const Vector2<double>& leftOpponentGoalPosition = fieldInfo.opponentGoalPostLeft;
  const Vector2<double>& rightOpponentGoalPosition = fieldInfo.opponentGoalPostRight;

  Goal modeledOpponentGoal = getOppGoal(compassDirection, fieldInfo);

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

void SelfLocGoalModel::update(const CompassDirection& compassDirection, const Pose2D& robotPose, const FieldInfo& fieldInfo)
{
  GoalModel::Goal ownGoal = getOwnGoal(compassDirection, fieldInfo);
  GoalModel::Goal oppGoal = getOppGoal(compassDirection, fieldInfo);

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

