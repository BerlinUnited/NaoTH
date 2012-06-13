/**
* @file AStartSearchParameters.h
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
* Declaration of class AStartSearchParameters
*/

#ifndef AStarSearchParameters_h_
#define AStarSearchParameters_h_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>
/**
* @class AStarSearcParameters
*
* A class for all parameters for A* search
*/
class AStarSearchParameters: public ParameterList, public naoth::Printable
{
public: 
  AStarSearchParameters();
  ~AStarSearchParameters();

  /** The minimum branching factor*/
  double minBranchingFactor;
  /** The maximum branching factor*/
  double maxBranchingFactor;
  /** The minimum expansion radius*/
  double minExpansionRadius;
  /** The maximum expansion radius*/
  double maxExpansionRadius;
  /** The minimum distance to the goal*/
  double distanceToTarget;
  /** The distance to own penalty area*/
  double distanceToOwnPenaltyArea;
  /** The radius of obstacle */
  double obstacleRadius;
  /** The radius of a robot */
  double robotRadius;
  /** The minimum distance for enlarging expansion radius and reducing branching factor*/
  double endOfNear;
  /** The maximum distance for enlarging expansion radius and reducing branching factor*/
  double endOfFar;
  /** The length of a generated vector*/
  double standardGradientLength;
  /** The number of consecutive calls of the A*-search */
  double numberOfCalls;
  /** The max number of nodes, the could be expanded*/
  double maxNumberOfNodes;
  
  virtual void print(std::ostream& stream) const {}

};


#endif // AStarSearchParameters_h_