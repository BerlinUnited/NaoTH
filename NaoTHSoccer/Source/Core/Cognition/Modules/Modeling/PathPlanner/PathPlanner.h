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
#include "Representations/Modeling/Path.h"
#include "Representations/Motion/MotionStatus.h"
#include "XabslEngine/XabslSymbols.h"
// Percepts
#include "Representations/Perception/ScanLineEdgelPercept.h"

// module specific headers
#include "AStarSearch.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PathPlanner)
  // representations
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(LocalObstacleModel)
  REQUIRE(RadarGrid)
  REQUIRE(PlayerInfo)
  REQUIRE(BallModel)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(LocalGoalModel)
  REQUIRE(PlayersModel)
  REQUIRE(RobotPose)
  REQUIRE(MotionStatus)
  // percepts
  REQUIRE(ScanLineEdgelPercept)
  // provide path
  PROVIDE(Path)
END_DECLARE_MODULE(PathPlanner)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PathPlanner: public PathPlannerBase
{
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

  Vector2d goalPoint;

  // obstacle positions
  std::vector<Vector2d> obstaclesPositions;
};

#endif // PathFinder_h_