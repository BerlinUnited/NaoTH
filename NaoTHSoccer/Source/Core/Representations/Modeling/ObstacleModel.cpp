/**
* @file ObstacleModel.cpp
*
* @author <a href="mailto:christian.heinemann@student.hu-berlin.de">Christian Heinemann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class ObstacleModel
*
*/

#include "ObstacleModel.h"


void ObstacleModel::print(ostream& stream) const
{
  stream << "== Obstacles ==" << endl;
  for(unsigned int i = 0; i < obstacles.size(); i++ )
  {
    stream << "Obstacle Nr." << i << endl;
    stream << obstacles[i].frameInfoObstacleWasSeen << endl;
    stream << "Distance = " << obstacles[i].distance << endl;
  }
}//end print

LocalObstacleModel::LocalObstacleModel() : someObstacleWasSeen(false)
{
  obstacles.reserve(20);
}




//void LocalObstacleModel::calculateBlueByYellow(double xLength)
//{
//  Vector2<double> tmp = yellowObstacle.rightPost;
//  tmp -= yellowObstacle.leftPost;
//  tmp = tmp.normalize();
//
//  Vector2<double> tmp2(tmp.y, -tmp.x);
//  tmp2 *= xLength;
//  blueObstacle.leftPost = yellowObstacle.rightPost;
//  blueObstacle.leftPost += tmp2;
//
//  blueObstacle.rightPost = yellowObstacle.leftPost;
//  blueObstacle.rightPost += tmp2;
//}//end calculateBlueByYellow
//
//void LocalObstacleModel::calculateYellowByBlue(double xLength)
//{
//  Vector2<double> tmp = blueObstacle.rightPost;
//  tmp -= blueObstacle.leftPost;
//  tmp = tmp.normalize();
//  Vector2<double> tmp2(tmp.y, -tmp.x);
//  tmp2 *= xLength;
//
//  yellowObstacle.leftPost = blueObstacle.rightPost;
//  yellowObstacle.leftPost += tmp2;
//
//  yellowObstacle.rightPost = blueObstacle.leftPost;
//  yellowObstacle.rightPost += tmp2;
//}//end calculateYellowByBlue
//
//
//Pose2D LocalObstacleModel::calculatePose(ColorClasses::Color opponentObstacleColor, const FieldInfo& fieldInfo) const
//{
//  Vector2<double> leftOpponentObstaclePosition(fieldInfo.xPosOpponentObstacle, fieldInfo.yPosLeftObstacle);
//  Vector2<double> rightOpponentObstaclePosition(fieldInfo.xPosOpponentObstacle, fieldInfo.yPosRightObstacle);
//
//  Vector2<double> leftModeledOpponentObstaclePosition;
//  Vector2<double> rightModeledOpponentObstaclePosition;
//
//  if (opponentObstacleColor == ColorClasses::skyblue)
//  {
//    leftModeledOpponentObstaclePosition = blueObstacle.leftPost;
//    rightModeledOpponentObstaclePosition = blueObstacle.rightPost;
//  }else
//  {
//    leftModeledOpponentObstaclePosition = yellowObstacle.leftPost;
//    rightModeledOpponentObstaclePosition = yellowObstacle.rightPost;
//  }//end else
//
//  double rotation = (leftModeledOpponentObstaclePosition-rightModeledOpponentObstaclePosition).angle() - Math::pi_2;
//  rotation = Math::normalize(-rotation);
//
//  Vector2<double> posLeft = leftOpponentObstaclePosition - leftModeledOpponentObstaclePosition.rotate(rotation);
//  Vector2<double> posRight = rightOpponentObstaclePosition - rightModeledOpponentObstaclePosition.rotate(rotation);
//
//  Pose2D pose;
//  pose.translation = (posLeft + posRight)*0.5;
//  pose.rotation = rotation;
//
//  return pose;
//}//end calculatePosition

void LocalObstacleModel::print(ostream& stream) const
{
  stream<<"someObstacleWasSeen = "<<(someObstacleWasSeen?"true":"false")<<'\n';
  ObstacleModel::print(stream);
}//end print

//void SelfLocObstacleModel::update(PlayerInfo::TeamColor ownColor, const Pose2D& robotPose, const FieldInfo& fieldInfo)
//{
//  Obstacle& ownObstacle = getTeamObstacle(ownColor);
//  Obstacle& oppObstacle = getTeamObstacle(!ownColor);
//
//  // transform the Obstacle posts to the local coordinates according to the robotPose
//  ownObstacle.leftPost = robotPose/fieldInfo.ownObstaclePostLeft;
//  ownObstacle.rightPost = robotPose/fieldInfo.ownObstaclePostRight;
//
//  oppObstacle.leftPost = robotPose/fieldInfo.opponentObstaclePostLeft;
//  oppObstacle.rightPost = robotPose/fieldInfo.opponentObstaclePostRight;
//}
