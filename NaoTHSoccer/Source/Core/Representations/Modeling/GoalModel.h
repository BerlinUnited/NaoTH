/**
* @file GoalModel.h
*
* @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GoalModel
*/

#ifndef __GoalModel_h_
#define __GoalModel_h_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Pose2D.h>
#include <Tools/DataStructures/Printable.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"

class GoalModel : public naoth::Printable
{
public:
  class Goal
  {
  public:
    Goal(){};
    virtual ~Goal(){};

    Vector2<double> leftPost;
    Vector2<double> rightPost;
    FrameInfo frameInfoWhenGoalLastSeen;
    
    /* color of the goal */
    ColorClasses::Color color;

    Vector2<double> calculateCenter() const
    {
      return (leftPost + rightPost)*0.5;
    }//end calculateCenter

  };//end class Goal

  Goal goalOne;
  Goal goalTwo;

  const Goal& getTeamGoal(double global_angle) const;
  Goal& getTeamGoal(double global_angle);
  const Goal& getOppGoal(double global_angle) const;
  Goal& getOppGoal(double global_angle);


  // TODO: deprecated?!
  //const Goal& getTeamGoal(naoth::GameData::TeamColor teamColor) const;
  //Goal& getTeamGoal(naoth::GameData::TeamColor teamColor);

  virtual void print(ostream& stream) const;

  void draw() const;

  static void calculateAnotherGoal(const GoalModel::Goal& one, GoalModel::Goal& another, double distance);
  Pose2D calculatePose(double global_angle, const FieldInfo& fieldInfo) const;
};//end class GoalModel


/** goal model in robot's local coordinates based on pure observations of the goal posts */
class LocalGoalModel : public GoalModel
{
public:
  LocalGoalModel();

  /** indicates whether a complete goal was seen in the last frame */
  bool someGoalWasSeen;

  bool opponentGoalIsValid;
  bool ownGoalIsValid;

  virtual void print(ostream& stream) const;

  Vector2<double> seen_center;
  double seen_angle;

  FrameInfo frameWhenOpponentGoalWasSeen;
  FrameInfo frameWhenOwnGoalWasSeen;
};

/** goal model in robot's local coordinates, it is updated by robot's pose */
class SelfLocGoalModel : public GoalModel
{
public:
  void update(const Pose2D& robotPose, const FieldInfo& fieldInfo);

  // convention: goalOne is own, goalTwo is opponent
  inline const Goal& getOppGoal() const { return goalOne; };
  inline const Goal& getOwnGoal() const { return goalTwo; };
};


class SensingGoalModel : public GoalModel
{
public:
  SensingGoalModel();

  //indicates whether a complete goal was seen in the last frame
  bool someGoalWasSeen;
};

#endif // __GoalModel_h_
