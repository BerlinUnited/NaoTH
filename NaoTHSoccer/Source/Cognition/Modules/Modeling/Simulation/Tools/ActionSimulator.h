/**
* @file ActionSimulator.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class ActionSimulator
*/

#ifndef _ActionSimulator_H
#define _ActionSimulator_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
//#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/KickActionModel.h"
//#include "Representations/Modeling/ObstacleModel.h"

//Tools
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Line.h>

#include <Tools/Math/Probabilistics.h>
//#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
//#include "Tools/Filters/AssymetricalBoolFilter.h"


// Debug
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"
#include <Tools/Debug/DebugRequest.h>


BEGIN_DECLARE_MODULE(ActionSimulator)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
//  REQUIRE(ObstacleModel)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  //REQUIRE(SelfLocGoalModel)

  PROVIDE(KickActionModel)
END_DECLARE_MODULE(ActionSimulator)

class ActionSimulator: public ActionSimulatorBase
{
public:
  ActionSimulator();
  ~ActionSimulator();

  virtual void execute();

  class ActionParams
  {
    public:
      ActionParams():
        speed(0.0),
        speed_std(0.0),
        angle(0.0),
        angle_std(0.0)
      {}
    public:
      double speed;
      double speed_std;
      double angle;
      double angle_std;
  };

   /** parameters for the module */
  class Parameters: public ParameterList
  {
  public:
	
    Parameters() : ParameterList("ActionParameters")
    {
      PARAMETER_REGISTER(sidekick_right.speed) = 750;
      PARAMETER_REGISTER(sidekick_right.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_right.angle) = -89.657943335302260;
      PARAMETER_REGISTER(sidekick_right.angle_std) = 10.553726275058064;

      PARAMETER_REGISTER(sidekick_left.speed) = 750;
      PARAMETER_REGISTER(sidekick_left.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_left.angle) = 86.170795364136380;
      PARAMETER_REGISTER(sidekick_left.angle_std) = 10.669170653645670;

      PARAMETER_REGISTER(kick_short.speed) = 1280;
      PARAMETER_REGISTER(kick_short.speed_std) = 150;
      PARAMETER_REGISTER(kick_short.angle) = 8.454482265522328;
      PARAMETER_REGISTER(kick_short.angle_std) = 6.992268841997358;

      //PARAMETER_REGISTER(kick_long.speed) = 1020;
      //PARAMETER_REGISTER(kick_long.speed_std) = 150;
      //PARAMETER_REGISTER(kick_long.angle) = 8.454482265522328;
      //PARAMETER_REGISTER(kick_long.angle_std) = 6.992268841997358;

      PARAMETER_REGISTER(friction) = 0.0275;

      PARAMETER_REGISTER(good_threshold_percentage) = 0.85;
      PARAMETER_REGISTER(numParticles) = 30;
      PARAMETER_REGISTER(minGoalParticles) = 9;
      
      PARAMETER_REGISTER(obstacleFilter.g0) = 0.01;
      PARAMETER_REGISTER(obstacleFilter.g1) = 0.1;

      syncWithConfig();
    }
    
    struct ObstacleFilter {
      double g0;
      double g1;
    } obstacleFilter;

    ActionParams sidekick_right;
    ActionParams sidekick_left;
    ActionParams kick_short;
    ActionParams kick_long;
    double friction;
    double good_threshold_percentage;
    double numParticles; //should be size_t
    double minGoalParticles;

  } theParameters;


  class Action
  {
  private:
    KickActionModel::ActionId _id;
    std::string _name;

    double action_speed;
    double action_speed_std;
    double action_angle;
    double action_angle_std;
    double friction;
    
  public:
    Action(KickActionModel::ActionId _id, const ActionParams& params, double friction) : 
		  _id(_id), 
      _name(KickActionModel::getName(_id)),
      action_speed(params.speed),
      action_speed_std(params.speed_std),
      action_angle(params.angle),
      action_angle_std(params.angle_std),
      friction(friction)
	  {
    }

	Vector2d predict(const Vector2d& ball, bool noise) const;
    KickActionModel::ActionId id() const { return _id; }
    const std::string& name() const { return _name; }
  };
  
  enum BallPositionCategory
  {
    INFIELD,
    OPPOUT,
    OWNOUT,
    LEFTOUT,
    RIGHTOUT,
    OPPGOAL,
    OWNGOAL,
    COLLISION,
    NUMBER_OF_BallPositionCategory
  };

  class CategorizedBallPosition
  {
    private:
      Vector2d ballPosition;
      BallPositionCategory category;
    public:
      CategorizedBallPosition(const Vector2d& position, BallPositionCategory cat):
        ballPosition(position),
        category(cat)
      {}
      BallPositionCategory cat() const {return category;} 
      const Vector2d& pos() const {return ballPosition;} 
  };

  class ActionResults 
  {
  public:
    typedef std::vector<CategorizedBallPosition> Positions;
  private:
    Positions ballPositions;
    // histogram of categories
    std::vector<int> cat_histogram;

  public:
    ActionResults() 
      : cat_histogram(NUMBER_OF_BallPositionCategory+1)
    {
    }

    const Positions& positions() const {
      return ballPositions;
    }

    int categorie(BallPositionCategory cat) const {
      return cat_histogram[cat];
    }

    double likelihood(BallPositionCategory cat) const {
      ASSERT(cat_histogram[NUMBER_OF_BallPositionCategory] > 0);
      return static_cast<double>(cat_histogram[cat]) / static_cast<double>(cat_histogram[NUMBER_OF_BallPositionCategory]);
    }

    void reset() {
      ballPositions.clear();
      cat_histogram.clear();
      cat_histogram.resize(NUMBER_OF_BallPositionCategory+1);
    }

    void add(const Vector2d& position, BallPositionCategory cat) {
      ballPositions.push_back(CategorizedBallPosition(position, cat));
      cat_histogram[cat]++;
      cat_histogram[NUMBER_OF_BallPositionCategory]++;
    }
  };

private:
  void simulateAction( const Action& action, ActionResults& result ) const;
  bool calculateCollision(const std::vector<Math::LineSegment>& lines, const Vector2d& start, const Vector2d& end, Vector2d& result) const;

  BallPositionCategory classifyBallPosition( const Vector2d& ballPosition ) const;

  double evaluateAction(const Vector2d& a) const;
  double evaluateAction(const ActionResults& results) const;

private: // debug
  void draw_potential_field() const;

private:
  inline double exp256(const double& x) const
  {
    // exp(x) = lim(n->inf) (1 + x/n)^n
    // for n=256 about 10x faster than exp but around 2.5 % off on x in [-10, 10]
    double y = 1.0 + x / 256.0;
    y *= y;
    y *= y;
    y *= y;
    y *= y;
    y *= y;
    y *= y;
    y *= y;
    y *= y;
    return y;
  }

  inline double gaussian(const double& x, const double& y, const double& muX, const double& muY, const double& sigmaX, const double& sigmaY) const
  {
    double facX = (x - muX) * (x - muX) / (2.0 * sigmaX * sigmaX);
    double facY = (y - muY) * (y - muY) / (2.0 * sigmaY * sigmaY);
    return exp256(-1.0 * (facX + facY));
  }

  inline double slope(const double& x, const double& y, const double& slopeX, const double& slopeY) const
  {
    return slopeX * x + slopeY * y;
  }
};

#endif  /* _ActionSimulator_H */
