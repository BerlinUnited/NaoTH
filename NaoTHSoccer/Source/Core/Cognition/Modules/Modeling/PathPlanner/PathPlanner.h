/**
* @file PathPlanner.h
* 
* DEclaration of class AStarSearch
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Yasinovskiy Kirill</a>
*/

#ifndef PathPlanner_h_
#define PathPlanner_h_

// include headers
// nao-th framework
// module
#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/RadarGrid.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Motion/MotionStatus.h"

// module specific headers
#include "AStarSearch.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PathPlanner)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(ObstacleModel)
  REQUIRE(RadarGrid)
  REQUIRE(PlayerInfo)
  REQUIRE(BallModel)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(LocalGoalModel)
  REQUIRE(PlayersModel)
  REQUIRE(RobotPose)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(PathPlanner)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PathPlanner: public PathPlannerBase
{
public:
public:
  /** functions */
  PathPlanner();
  ~PathPlanner(){}

  /** executes the module */
  void execute();


  /** data */

  /** create a pointer to AStarSearch */
  AStarSearch* theAStarSearch;

protected:
private:
};

#endif // PathFinder_h_
