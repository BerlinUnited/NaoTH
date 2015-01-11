/**
* @file StrategyTools.cpp
*
* @author Tobias Hübner</a>
* 
* Declaration of the class StrategyTools
*/

#ifndef _StrategyTools_h_
#define _StrategyTools_h_

#include <vector>
#include <Tools/Math/Vector2.h>

class StrategyTools
{
private:
  StrategyTools(){} // there shouldn't be any instances of it

public:
   static void arrangeRobots(
    const std::vector<Vector2d >& robots, 
    const std::vector<Vector2d >& places, 
    std::vector<int>& placesToRobots,
    int numberOfIterations = 50);
};

#endif //_StrategyTools_h_