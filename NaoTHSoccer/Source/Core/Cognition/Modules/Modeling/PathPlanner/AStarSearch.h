/**
* @file AStarSearch.h
* 
* Definition of class AStarSearch
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Yasinovskiy Kirill</a>
*/

#ifndef _AStartSearch_h_
#define _AStartSearch_h_

// include standard library headers
#include <algorithm>
#include <set>
#include <vector>
#include <limits>

// naoth framework headers
#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Line.h"
#include "Tools/Debug/Stopwatch.h"


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
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
// Percepts
#include "Representations/Perception/ScanLineEdgelPercept.h"

// class specific headers
#include "AStarSearchParameters.h"

#define EPSILON 0.00000001


/**
* @class AStarNode
*
* A class describing a node in an A* search tree
*/
class AStarNode
{
public:
  /**Default constructor */
  AStarNode()
    :
  position(0.0,0.0)
  { 
    fValue = 0;
    gValue = 0;
    hValue = 0;
    parentNode = 0;
    expanded = false; 
    attractionValue = 0;
    expansionRadius = 0;
  }
  /** Constructor with Vector2d as parameter
  * @param position Position on a node
  */
  AStarNode(const Vector2d nodePosition)
  {
    position = nodePosition;
    fValue = 0;
    gValue = 0;
    hValue = 0;
    parentNode = 0;
    expanded = false; 
    attractionValue = 0;
    expansionRadius = 0;
  }

  /** Copy-Constructor
  * @param node Another node to copy data from
  */
  AStarNode(const AStarNode& node)
  {
    position = node.position;
    fValue = node.fValue;
	  gValue = node.gValue;
    hValue = node.hValue;
    parentNode = node.parentNode;  
    expanded = node.expanded;
    attractionValue = node.attractionValue;
    expansionRadius = node.expansionRadius;
  }

  /** Sets the index of the parent node of this node
  * @param parentNode The index of the parent node
  */
	void setParentNode(unsigned int parentNode)
  { this->parentNode = parentNode;}

  /** Returns the index of the parent node
  * @return The parent node
  */
	unsigned int getParentNode() const 
  { return parentNode;}

  /** Sets the function values
  * @param g The costs of the path to this node
  * @param h The heuristic costs from this node to the goal
  */
  void setFunctionValues(double g, double h)
  { gValue = g; hValue = h;fValue = g+h;}

  /** Returns the value of this node
  * @return The value f
  */
	double f() const
  { return fValue;}

  /** Returns the costs of the path to this node
  * @return The path costs g
  */
  double g() const
  { return gValue;}

    /** Returns the heuristics of the node
  * @return The heuristic h
  */
  double h() const
  { return hValue;}

  
  /** Sets the position of this node
  * @param position The position
  */
  void setPosition(const Vector2d& position) 
  { this->position = position;}

  /** Returns the position of this node
  * @return The position
  */
  Vector2d getPosition() const
  { return position;}

  /** The most important function: expands the current node by generating
  * successor nodes. 
  * @param searchTree The search tree of A* search, new nodes will be inserted
  * @param expandedNodes A list of previously expanded nodes
  * @param goal The goal of the search
  * @param parameterSet The parameter set for the search
  * @param ownNodeNum The index of this node (in the seachTree)
  */
	void successor(std::vector<AStarNode>& searchTree,
              const std::vector<unsigned int>& expandedNodes,
              const AStarNode& start,
              const AStarNode& goal,
              const AStarSearchParameters& parameterSet,
              const std::vector<Vector2d>& obstacles,
              unsigned int ownNodeNum,
              const RobotPose& rp, const PlayerInfo& pi, const FieldInfo& fi);

  /** Checks if this node has been expanded
  * @return true, if the node has been expanded
  */
  bool hasBeenExpanded() const 
  { return expanded;}

  /** Checks if this node has reached the goal
  * @param goal The goal
  * @param parameterSet The parameter set
  * @return true, if the node has reached the goal
  */
  bool hasReached(const AStarNode& goal,
                  const AStarSearchParameters& parameterSet) const;

  /** Computes the distance to another node
  * @param other The other node
  * @return The distance
  */
  double distToOtherNode(const AStarNode& other)
  { return (position - other.getPosition()).abs();}

protected:
  /** The position of the node*/
	Vector2d position;
  /** The value of the node*/
  double fValue;
  /** The path costs to the node*/
	double gValue;
  /** The heuristic of the node */
  double hValue;
  /** The index of the parent node*/
  unsigned int parentNode;  
  /** Flag: true, if the node has been expanded*/
  bool expanded;
  /** The attraction value at the position of the node
  * could be the potential field value or just some value 
  * that describes how attractive is this point for the
  * search algorithm (it is used with the cost - function)
  */
  double attractionValue;
  /** The expansion radius of the node*/
  double expansionRadius;

  /** Returns the expansion radius of the node
  * @return The expansion radius
  */
  double getExpansionRadius() const
  { return expansionRadius;}

  /** Computes the path costs to another node
  * @param node The other node
  * @param parameterSet The parameter set
  * @return The path costs
  */
  double computeCostsTo(const AStarNode& node) const;

  /** Computes the heuristic costs between two positions
  * @param p1 The first position
  * @param p2 The second position
  * @param currentBranchingFactor The current branching factor
  * @return The heuristic costs
  */
  double computeHeuristicBetween(const Vector2d& p1, const Vector2d& p2) const;

  /** Computes current parameters for expansion
  * @param currentExpansionRadius The current expansion radius
  * @param currentBranchingFactor The current branching factor
  * @param parameterSet The parameter set
  */
  void computeCurrentParameters(double& currentBranchingFactor,
                                const AStarNode& start,
                                const AStarSearchParameters& parameterSet);

  /** Checks if a new node is too close to another previously expanded node
  * @param searchTree The search tree
  * @param expandedNodes A list containing all previously expanded nodes
  * @param position The position of the new node
  * @return true, if the position is too close
  */
  bool tooCloseToAnotherNode(std::vector<AStarNode>& searchTree,
                             const std::vector<unsigned int>& expandedNodes,
                             const Vector2d& position) const;

  /** Tests if any node is too close to some goal
  * @param obstacles The obstacles to test
  * @param parameterSet The parameter set
  * @return true, if the node is close to some obstacle
  */
  bool tooCloseToObstacle(const std::vector<Vector2d>& obstacles, Vector2d& obstaclePosition, const AStarSearchParameters& parameterSet) const;


  bool collidesWithField(const RobotPose& rp, const PlayerInfo& pi, const FieldInfo& fi, const AStarSearchParameters& parameterSet);

};// end class AStarNode

/** For sorting the heap the STL needs compare function that lets us compare
* the f value of two nodes
*/
class HeapCompare
{
public:
  bool operator() ( const AStarNode *first, const AStarNode *second ) const
  {
    return first->f() > second->f();
  }
}; // end class HeapCompare

/**
* @class AStarSearch
*
* An abstract implementation of the A* search algorithm.
*/
class AStarSearch
{
public:
  /** Constructor 
  * @param minCacheSize The number of nodes to allocate memory for
  * @param maxTreeSize The maximum number of nodes to expand
  * @param parameterSet A parameter set to check the maximum number of nodes to expand
  */
  AStarSearch(const RobotPose& rp, const FieldInfo& fi, const PlayerInfo& pi)
    :
  theRobotPose(rp),
  theFieldInfo(fi),
  thePlayerInfo(pi),
  indexOfBestNode(0)
  {
    this->obstacles = obstacles;
  }// end constructor

  /** The main search function
  * @param start The starting node
  * @param goal The goal node
  * @param parameterSet The current parameter set
  * @param pathLength Returns the length of the path to the goal
  * @return The next node to go to
  */
	AStarNode search(const AStarNode& start, const AStarNode& goal, double& pathLength, std::vector<Vector2d>& obstacles, bool& pathFound, bool& noNodeExpandable)
	{
    // set obstacles, goal, start
    this->obstacles = obstacles;
    this->myStart.setPosition(start.getPosition());
    this->myGoal.setPosition(goal.getPosition());
    checkWhetherGoalIsInObstacle();

    if(start.hasReached(this->myGoal, parameterSet))
    {
      return goal;
    }
    searchTree.clear();
    expandedNodes.clear();
    // push back the start node in vector
    searchTree.push_back(this->myStart);
    // sort the heap
    //push_heap(searchTree.begin(), searchTree.end(), HeapCompare()); // we don't use in current version...
    indexOfBestNode = 0;
    unsigned int nextNodeToExpand = 0;
    while(true)
    {
      // find next node to expand (the one with
      // the minimal function value)
      nextNodeToExpand = findNextNodeToExpand();
      // can we expand other nodes, or should we stop (memory bounding...)
      if(searchTree.size() >= (unsigned int)parameterSet.maxNumberOfNodes)
      {
        indexOfBestNode = nextNodeToExpand;
        break;
      }
      else
      {
        unsigned int sizeBeforeExpand(searchTree.size());
        searchTree[nextNodeToExpand].successor(searchTree, expandedNodes, this->myStart,
                                            this->myGoal, this->parameterSet, this->obstacles, nextNodeToExpand,
                                            theRobotPose, thePlayerInfo, theFieldInfo);
        expandedNodes.push_back(nextNodeToExpand);
        unsigned int sizeAfterExpand(searchTree.size());
        // have we expanded any nodes?
        if (sizeAfterExpand == sizeBeforeExpand)
        {
          indexOfBestNode = nextNodeToExpand; //findPartiallyPath();
          noNodeExpandable = true;
          break;
        }
        int result(testNewNodesAgainstGoal(sizeBeforeExpand, sizeAfterExpand));
        if(result != -1)
        {
          indexOfBestNode = result;
          pathFound = true;
          break;
        }
      }
    }
    this->myGoal.setParentNode(indexOfBestNode);
    AStarNode result = backtraceNode(indexOfBestNode, pathLength);
    // return the node
    return result;
  }

  /**
  * DebugDrawings:
  * draw Path in Field or Image Context
  */
  void drawAllNodesField();
  void drawPathFieldLocal();
  void drawPathFieldGlobal();
  void drawObstaclesLocal();
  void drawObstaclesGlobal();


private:
  /** A container for all nodes*/
	std::vector<AStarNode> searchTree;
  /** Indices of all expanded nodes*/
  std::vector<unsigned int> expandedNodes;
  /** The parameter set*/
  AStarSearchParameters parameterSet;

  // obstacles positions
  std::vector<Vector2d> obstacles;

  // robot pose
  const RobotPose& theRobotPose;

  // field info
  const FieldInfo& theFieldInfo;

  // player info
  const PlayerInfo& thePlayerInfo;

  // start and goal
  AStarNode myStart;
  AStarNode myGoal;

  // the best node
  unsigned int indexOfBestNode;

  /** 
  * Finds the next node to be expanded
  * @return The index of the node
  */
  unsigned int findNextNodeToExpand()
  {
    unsigned int expandableNode(0);
    // find first unexpanded node
    while(searchTree[expandableNode].hasBeenExpanded())
    {
      expandableNode++;
    }
    // store his function value (cost+heuristic)
    double f = searchTree[expandableNode].f();
    for(unsigned int i=(expandableNode+1); i<searchTree.size(); i++)
    {
      if((!searchTree[i].hasBeenExpanded()) && (searchTree[i].f() < f))
      {
        f = searchTree[i].f();
        expandableNode = i;
      }
    }
    return expandableNode;
  }

  unsigned int findPartiallyPath()
  {
    unsigned int result = 0;
    // store his function value (cost+heuristic)
    double f = (myStart.getPosition() - myGoal.getPosition()).abs();
    for(unsigned int i=result; i<searchTree.size(); i++)
    {
      if(searchTree[i].f() < f)
      {
        f = searchTree[i].f();
        result = i;
      }
    }
    return result;
  }


  /** Tests if any node has reached the goal
  * @param firstNode The first node to test
  * @param lastNode The last node to test
  * @return -1, if no node has reached the goal, the number of the node otherwise
  */
  int testNewNodesAgainstGoal(unsigned int firstNode, unsigned int lastNode)
  {
    double maxDistance(std::numeric_limits<double>::infinity());
    double temp(0.0);
    int result(-1);
    for(unsigned int i = firstNode; i < lastNode; i++)
    {
      if(searchTree[i].hasReached(myGoal, parameterSet))
      {
        temp = searchTree[i].distToOtherNode(myGoal);
        if (temp < maxDistance)
        {
          maxDistance = temp;
          result = i;
        }
      }
    }
    return result;
  }


  /** Back-traces the best node found to the first node after the start node
  * @param indexOfBestNode The index of the best node found
  * @param pathLength Returns the length of the back-traced path
  * @return The root node of the path to the node
  */
  AStarNode backtraceNode(unsigned int indexOfBestNode, double& pathLength)
  {
    unsigned int currentNode(indexOfBestNode);
    pathLength = searchTree[currentNode].distToOtherNode(searchTree[searchTree[currentNode].getParentNode()]);
    while(searchTree[currentNode].getParentNode() != 0)
    {
      currentNode = searchTree[currentNode].getParentNode();
      pathLength += searchTree[currentNode].distToOtherNode(searchTree[searchTree[currentNode].getParentNode()]);
    }
    return searchTree[currentNode];
  }


  /** Check whether the Goal is in obstacle
  * if it is so, the position of the goal is 
  * changed to the closest point to previous goal
  */
  void checkWhetherGoalIsInObstacle()
  {
    
    for(unsigned int i = 0; i < this->obstacles.size(); i++)
    {
      if ((this->myGoal.getPosition() - obstacles[i]).abs() <= this->parameterSet.obstacleRadius + this->parameterSet.robotRadius)
      {
        Math::LineSegment line(this->myStart.getPosition(), obstacles[i]);
        Vector2d projection = line.projection(obstacles[i]);
        double angleToObstacle  = (projection - this->myStart.getPosition()).angle();
        double distanceToObstacle = (projection - this->myStart.getPosition()).abs();
        Vector2d nodePosition((distanceToObstacle - this->parameterSet.obstacleRadius - this->parameterSet.robotRadius), 0.0);
        nodePosition.rotate(angleToObstacle);
        this->myGoal.setPosition(nodePosition);
      }//end if
    }// end for
  } // end checkWhetherGoalIsInObstacle
};// end class AStartSearch


#endif //AStartSaerch_h_
