/**
* @file AStarSearch.cpp
* 
* Implementation of class AStarSearch
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Yasinovskiy Kirill</a>
*/

#include "AStarSearch.h"


//debugDrawings
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Math/Line.h"

//debug
#include "Tools/Debug/NaoTHAssert.h"


void AStarNode::successor(std::vector<AStarNode>& searchTree,
  const std::vector<unsigned int>& expandedNodes,
  const AStarNode& start,
  const AStarNode& goal,
  const AStarSearchParameters& parameterSet,
  const std::vector<Vector2d>& obstacles,
  unsigned int ownNodeNum,
  const RobotPose& rp, const PlayerInfo& pi, const FieldInfo& fi)
{
  expanded = true;
  // create new node
  AStarNode newNode;
  // compute new pseudo random params
  // for new node
  double currentBranchingFactor;
  computeCurrentParameters(expansionRadius, currentBranchingFactor, start, parameterSet);
  Vector2d newPosition;
  double newG, newH;
  // angle between nodes: determines the angle distance between nodes
  // in some sector (could be the whole circle as well)
  double angleBetweenNodes(Math::pi / currentBranchingFactor);
  Vector2d parentPosition(searchTree[parentNode].getPosition());
  Vector2d goalPosition(goal.getPosition());
  double angleToParent((position-parentPosition).angle());
  double angleToGoal((goalPosition-position).angle());
  Vector2d nodePosition(expansionRadius, 0.0);
  nodePosition.rotate(angleToParent - Math::pi_2);
  for(int i = 0; i <= currentBranchingFactor; i++)
  {
    // estimate new position of the node
    newPosition = position;
    newPosition += nodePosition;
    // some constraints
    Vector2d obstaclePosition;
    newNode.setPosition(newPosition);
    if(!newNode.tooCloseToAnotherNode(searchTree, expandedNodes, newPosition) && 
      !newNode.tooCloseToObstacle(obstacles, obstaclePosition, parameterSet) &&
      !newNode.collidesWithField(rp, pi, fi, parameterSet))
    {
      // ..parent
      newNode.setParentNode(ownNodeNum);
      // ..expansion radius
      newNode.expansionRadius = expansionRadius;
      // ..heuristic and cost function
      newG = gValue + computeCostsTo(newNode);
      newH = computeHeuristicBetween(goalPosition, newPosition);
      newNode.setFunctionValues(newG, newH);
      // push back new node to the tree...
      searchTree.push_back(newNode);
    }
    nodePosition.rotate(angleBetweenNodes);
  }
  //make sure, that we added the closest to goal node as well
  nodePosition.x = expansionRadius;
  nodePosition.y = 0;
  nodePosition.rotate(angleToGoal);
  newPosition = position;
  newPosition += nodePosition;
  newNode.setPosition(newPosition);
  Vector2d obstaclePosition;
  if(!newNode.tooCloseToAnotherNode(searchTree, expandedNodes, nodePosition) &&
    !newNode.tooCloseToObstacle(obstacles, obstaclePosition, parameterSet) &&
    !newNode.collidesWithField(rp, pi, fi, parameterSet))
  {
    // ..parent
    newNode.setParentNode(ownNodeNum);
    // ..expansion radius
    newNode.expansionRadius = expansionRadius;
    // ..heuristic and cost function
    newG = gValue + computeCostsTo(newNode);
    newH = computeHeuristicBetween(newPosition, goalPosition);
    newNode.setFunctionValues(newG, newH);
    // push back new node to the tree...
    searchTree.push_back(newNode);
  }
} // end expand


bool AStarNode::hasReached(
  const AStarNode& goal,
  const AStarSearchParameters& parameterSet) const
{
  double distanceToGoal((goal.getPosition() - position).abs());
  return ((distanceToGoal <= parameterSet.distanceToTarget) || 
    (distanceToGoal <= expansionRadius));
}


inline double AStarNode::computeCostsTo(const AStarNode& node) const
{
  double valDiff(node.attractionValue - this->attractionValue);
  if(valDiff <= 0.0)
  {
    return expansionRadius;
  }
  else
  {
    return expansionRadius + valDiff;
  }
}


inline double AStarNode::computeHeuristicBetween(
  const Vector2d& p1, const Vector2d& p2) const
{
    double distance((p2 - p1).abs());
    return distance;
}


inline void AStarNode::computeCurrentParameters(
  double& currentExpansionRadius, 
  double& currentBranchingFactor,
  const AStarNode& start,
  const AStarSearchParameters& parameterSet) const
{
  double dist((position - start.getPosition()).abs());
  if(dist < parameterSet.endOfNear)
  {
    currentExpansionRadius = parameterSet.minExpansionRadius;
    currentBranchingFactor = parameterSet.maxBranchingFactor;
  }
  else if(dist > parameterSet.endOfFar)
  {
    currentExpansionRadius = parameterSet.maxExpansionRadius;
    currentBranchingFactor = parameterSet.maxBranchingFactor;
  }
  else
  {
    double distPercentage((dist-parameterSet.endOfNear)/(parameterSet.endOfFar - parameterSet.endOfNear));
    currentExpansionRadius = parameterSet.minExpansionRadius + distPercentage*
      (parameterSet.maxExpansionRadius - parameterSet.minExpansionRadius);
    currentBranchingFactor = parameterSet.maxBranchingFactor - (unsigned int)(floor(distPercentage*
      (double)(parameterSet.maxBranchingFactor - parameterSet.minBranchingFactor)));
  }
}

bool AStarNode::tooCloseToAnotherNode( std::vector<AStarNode>& searchTree, const std::vector<unsigned int>& expandedNodes, const Vector2d& position ) const
{
  for(unsigned int i=0; i<expandedNodes.size(); i++)
  {
    if((searchTree[expandedNodes[i]].getPosition() - position).abs() < 
      searchTree[expandedNodes[i]].getExpansionRadius()*0.95)
    {
      return true;
    }
  }
  return false;
}

bool AStarNode::tooCloseToObstacle(const std::vector<Vector2d>& obstacles, Vector2d& obstaclePosition, 
                                    const AStarSearchParameters& parameterSet) const
{
  double currentDistance = parameterSet.obstacleRadius + parameterSet.robotRadius;
  bool foundObstacle = false;
  double distance;
  for(unsigned int i = 0; i < obstacles.size(); i++)
  {
    distance = (position - obstacles[i]).abs();
    if(distance < currentDistance && obstacles[i] != position) // finds the nearest obstacle to the current position
    {
      obstaclePosition = obstacles[i];
      currentDistance = distance;
      foundObstacle = true;
    }
  }
  return foundObstacle;
}

bool AStarNode::collidesWithField(const RobotPose& rp, const PlayerInfo& pi, const FieldInfo& fi, const AStarSearchParameters& parameterSet)
{
  bool collides = false;
  int playerNumber = pi.gameData.playerNumber;
  Vector2d globalPose = rp.translation + position;
  // path should be around the own penalty area for avoiding illegal defenders (Only for field players)
  if(globalPose.x < fi.xPosOwnPenaltyArea + parameterSet.distanceToOwnPenaltyArea && 
     std::abs(globalPose.y) < fi.yPosLeftPenaltyArea + parameterSet.distanceToOwnPenaltyArea && playerNumber != 1) 
  {
    collides = true;
  }
  return collides;
}



void AStarSearch::drawAllNodesField()
{
  FIELD_DRAWING_CONTEXT;
  std::vector<AStarNode>::const_iterator it = searchTree.begin();
  for (;it != searchTree.end(); it++)
  {
    PEN("434343", 3);
    CIRCLE(it->getPosition().x, it->getPosition().y, 5);
  }
}

void AStarSearch::drawPathFieldLocal()
{
  FIELD_DRAWING_CONTEXT;
  if (pathFound)
  {
    unsigned int currentNode = indexOfBestNode;
    // draw target
    PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
    CIRCLE(myGoal.getPosition().x, myGoal.getPosition().y, parameterSet.robotRadius);
    PEN(ColorClasses::colorClassToHex(ColorClasses::blue), 4);
    LINE(myGoal.getPosition().x, myGoal.getPosition().y, searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y);
    
    while(searchTree[currentNode].getParentNode() != 0)
    {
      PEN(ColorClasses::colorClassToHex(ColorClasses::orange), 4);
      unsigned int nextNode = searchTree[currentNode].getParentNode();
      LINE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, searchTree[nextNode].getPosition().x, searchTree[nextNode].getPosition().y);
      PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
      CIRCLE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, parameterSet.robotRadius);
      currentNode = nextNode;
    }
    LINE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, myStart.getPosition().x, myStart.getPosition().y);
  }
}

void AStarSearch::drawPathFieldGlobal()
{
  FIELD_DRAWING_CONTEXT;
  if (pathFound)
  {
    unsigned int currentNode = indexOfBestNode;
    // draw target
    PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
    CIRCLE(myGoal.getPosition().x + theRobotPose.translation.x, myGoal.getPosition().y + theRobotPose.translation.y, parameterSet.robotRadius);
    PEN(ColorClasses::colorClassToHex(ColorClasses::blue), 4);
    LINE(myGoal.getPosition().x + theRobotPose.translation.x, myGoal.getPosition().y + theRobotPose.translation.y, searchTree[currentNode].getPosition().x + theRobotPose.translation.x, searchTree[currentNode].getPosition().y + theRobotPose.translation.y);

    while(searchTree[currentNode].getParentNode() != 0)
    {
      PEN(ColorClasses::colorClassToHex(ColorClasses::orange), 4);
      unsigned int nextNode = searchTree[currentNode].getParentNode();
      LINE(searchTree[currentNode].getPosition().x + theRobotPose.translation.x, searchTree[currentNode].getPosition().y + theRobotPose.translation.y, searchTree[nextNode].getPosition().x + theRobotPose.translation.x, searchTree[nextNode].getPosition().y + theRobotPose.translation.y);
      PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
      CIRCLE(searchTree[currentNode].getPosition().x + theRobotPose.translation.x, searchTree[currentNode].getPosition().y + theRobotPose.translation.y, parameterSet.robotRadius);
      currentNode = nextNode;
    }
    LINE(searchTree[currentNode].getPosition().x + theRobotPose.translation.x, searchTree[currentNode].getPosition().y + theRobotPose.translation.y, myStart.getPosition().x + theRobotPose.translation.x, myStart.getPosition().y + theRobotPose.translation.y);
  }
}


void AStarSearch::drawObstaclesLocal()
{
  FIELD_DRAWING_CONTEXT;
  for (unsigned int i = 0; i < obstacles.size(); i++)
  {
    PEN(ColorClasses::colorClassToHex(ColorClasses::black), 10);
    CIRCLE(obstacles[i].x, obstacles[i].y, 10);
    PEN(ColorClasses::colorClassToHex(ColorClasses::red), 5);
    CIRCLE(obstacles[i].x, obstacles[i].y, parameterSet.obstacleRadius);
  }
}


void AStarSearch::drawObstaclesGlobal()
{
  FIELD_DRAWING_CONTEXT;
  for (unsigned int i = 0; i < obstacles.size(); i++)
  {
    PEN(ColorClasses::colorClassToHex(ColorClasses::black), 10);
    CIRCLE(obstacles[i].x + theRobotPose.translation.x, obstacles[i].y + theRobotPose.translation.y, 10);
    PEN(ColorClasses::colorClassToHex(ColorClasses::red), 5);
    CIRCLE(obstacles[i].x + theRobotPose.translation.x, obstacles[i].y + theRobotPose.translation.y, parameterSet.obstacleRadius);
  }
}
