/**
* @file StrategyTools.cpp
*
* @author Tobias Hübner</a>
* 
* Declaration of the class StrategyTools
*/

#ifndef _StrategyTools_h_
#define _StrategyTools_h_

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

using namespace std;

class StrategyTools
{
private:
  StrategyTools(){} // there shouldn't be any instances of it

public:
   void arrangeRobots(
    const std::vector<Vector2d >& robots, 
    const std::vector<Vector2d >& places, 
    std::vector<int>& placesToRobots );
};

#endif //_StrategyTools_h_