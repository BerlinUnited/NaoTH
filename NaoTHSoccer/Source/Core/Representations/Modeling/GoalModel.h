#ifndef __GoalModel_h_
#define __GoalModel_h_


class GoalModel : public Printable
{
public:
  class Goal
  {
  public:
    Goal(){};
    ~Goal(){};

    Vector2<double> leftPost;
    Vector2<double> rightPost;
    FrameInfo frameInfoWhenGoalLastSeen;

    Vector2<double> calculateCenter() const
    {
      return (leftPost + rightPost)*0.5;
    }//end calculateCenter

  };//end class Goal

  Goal blueGoal;
  Goal yellowGoal;

  //const Goal& getTeamGoal(PlayerInfo::TeamColor teamColor) const;
  //Goal& getTeamGoal(PlayerInfo::TeamColor teamColor);

  virtual void print(ostream& stream) const;
  //void draw() const;

  //void calculateBlueByYellow(double xLength);
  //void calculateYellowByBlue(double xLength);
  //Pose2D calculatePose(ColorClasses::Color opponentGoalColor, const FieldInfo& fieldInfo) const;
};//end class Goal

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
/*class SelfLocGoalModel : public GoalModel
{
public:
  void update(PlayerInfo::TeamColor ownColor, const Pose2D& robotPose, const FieldInfo& fieldInfo);
};


class SensingGoalModel : public GoalModel
{
public:
  //indicates whether a complete goal was seen in the last frame
  bool someGoalWasSeen;
};*/

//REPRESENTATION_INTERFACE(LocalGoalModel);
//REPRESENTATION_INTERFACE(SelfLocGoalModel);
//REPRESENTATION_INTERFACE(SensingGoalModel);

#endif // __GoalModel_h_
