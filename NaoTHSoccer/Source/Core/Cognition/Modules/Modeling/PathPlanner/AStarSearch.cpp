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
  unsigned int ownNodeNum)
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
    if(!tooCloseToAnotherNode(searchTree, expandedNodes, newPosition) && !tooCloseToObstacle(obstacles, newPosition, parameterSet))
    {
      // set node params:
      // ..position
      newNode.setPosition(newPosition);
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
  if(!tooCloseToAnotherNode(searchTree, expandedNodes, nodePosition) && !tooCloseToObstacle(obstacles, newPosition, parameterSet))
  {
    // set node params:
    // ..position
    newNode.setPosition(newPosition);
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
  return ((distanceToGoal <= parameterSet.distanceToGoal) || 
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

bool AStarNode::tooCloseToObstacle(const std::vector<Vector2d>& obstacles, const Vector2d& position,const AStarSearchParameters& parameterSet) const
{
  {
    for(unsigned int i = 0; i < obstacles.size(); i++)
    {
      if ((position - obstacles[i]).abs() <= parameterSet.obstacleRadius + parameterSet.robotRadius)
      {
        return true;
      }
    }
    return false;
  }
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

void AStarSearch::drawPathFiled()
{
  FIELD_DRAWING_CONTEXT;
  PEN(ColorClasses::colorClassToHex(ColorClasses::orange), 4);
  if (pathFound)
  {
    unsigned int currentNode = goal.getParentNode();
    LINE(goal.getPosition().x, goal.getPosition().y, searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y);
    PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
    CIRCLE(goal.getPosition().x, goal.getPosition().y, parameterSet.robotRadius);
    while(searchTree[currentNode].getParentNode() != 0)
    {
      PEN(ColorClasses::colorClassToHex(ColorClasses::orange), 4);
      unsigned int nextNode = searchTree[currentNode].getParentNode();
      LINE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, searchTree[nextNode].getPosition().x, searchTree[nextNode].getPosition().y);
      PEN(ColorClasses::colorClassToHex(ColorClasses::green), 3);
      CIRCLE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, parameterSet.robotRadius);
      currentNode = nextNode;
    }
    PEN(ColorClasses::colorClassToHex(ColorClasses::orange), 4);
    LINE(searchTree[currentNode].getPosition().x, searchTree[currentNode].getPosition().y, start.getPosition().x, start.getPosition().y);
  }
}

void AStarSearch::drawHeuristic()
{
  FIELD_DRAWING_CONTEXT;
  std::vector<AStarNode>::const_iterator it = searchTree.begin();
  for (;it != searchTree.end(); it++)
  {
    TEXT_DRAWING(it->getPosition().x, it->getPosition().y, it->h());
  }
}

void AStarSearch::drawCost()
{
  FIELD_DRAWING_CONTEXT;
  std::vector<AStarNode>::const_iterator it = searchTree.begin();
  for (;it != searchTree.end(); it++)
  {
    TEXT_DRAWING(it->getPosition().x, it->getPosition().y, it->g());
  }
}

void AStarSearch::drawFunction()
{
  FIELD_DRAWING_CONTEXT;
  std::vector<AStarNode>::const_iterator it = searchTree.begin();
  for (;it != searchTree.end(); it++)
  {
    TEXT_DRAWING(it->getPosition().x, it->getPosition().y, it->f());
  }
}

void AStarSearch::drawObstacles()
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

