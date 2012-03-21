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
  unsigned int ownNodeNum)
{
  expanded = true;
  // create new node
  AStarNode newNode;
  // compute new pseudo random params
  // for new node
  double currentBranchingFactor;
  computeCurrentParameters(expansionRadius, currentBranchingFactor, start,parameterSet);
  Vector2d newPosition;
  double newG, newH;
  // angle between nodes: determines the angle distance between nodes
  // in some sector (could be the whole circle as well)
  double angleBetweenNodes(Math::pi / currentBranchingFactor);
  Vector2d parentPosition(searchTree[parentNode].getPosition());
  double angleToParent((position - parentPosition).angle());
  Vector2d nodePosition(expansionRadius, 0.0);
  nodePosition.rotate(angleToParent-Math::pi_2);
  Vector2d goalPosition(goal.getPosition());
  for(int i = 0; i <= currentBranchingFactor; i++)
  {
    // estimate new position of the node
    newPosition = position;
    newPosition += nodePosition;
    // some constraints
    if(!tooCloseToAnotherNode(searchTree, expandedNodes, newPosition))
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
      // and sort the heap
      //push_heap(searchTree.begin(), searchTree.end(), HeapCompare());
    }
    nodePosition.rotate(angleBetweenNodes);
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
//   double angleToP2((p1-p2).angle());
//   double angleBetween(Math::pi / currentBranchingFactor);
//   double angleRatio(angleToP2 / angleBetween);
//   if((angleRatio - floor(angleRatio)) < EPSILON)
//   {
    double distance((p2 - p1).abs());
    return distance;
//   }
//   else
//   {
//     double smallerAngle(0.0), largerAngle(0.0);
//     if(angleToP2 > 0)
//     {
//       while(largerAngle < angleToP2)
//       {
//         largerAngle += angleBetween;
//       }
//       smallerAngle = largerAngle - angleBetween;
//     }
//     else
//     {
//       while(smallerAngle > angleToP2)
//       {
//         smallerAngle -= angleBetween;
//       }
//       largerAngle = smallerAngle + angleBetween;
//     }
//     Vector2d vs(1.0,0.0);
//     Vector2d vl(1.0,0.0);
//     vs.rotate(smallerAngle);
//     vl.rotate(largerAngle);
//     Vector2d pDiff(p1-p2);
//     double n((pDiff.x*vl.y - pDiff.y*vl.x) / (vs.x*vl.y - vs.y*vl.x));
//     Vector2d intersectionPos(vs);
//     intersectionPos *= n;
//     intersectionPos += p2;
//     double dist1((p1-intersectionPos).abs());
//     double dist2((p2-intersectionPos).abs());
//     return (dist1/expansionRadius*expansionRadius +
//      dist2/expansionRadius)*expansionRadius;
//   }
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
    currentBranchingFactor = parameterSet.minBranchingFactor;
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

bool AStarNode::tooCloseToAnotherNode( std::vector<AStarNode>& searchTree, const std::vector<unsigned int>& expandedNodes, const Vector2d position ) const
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

