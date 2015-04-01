/*
* @file TeamMessageStatistics.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
* Statistics for the whole team
*/

#ifndef _TeamMessageStatistics_H
#define _TeamMessageStatistics_H

#include "RobotMessageStatistics.h"

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Modeling/TeamMessage.h"
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamMessageStatisticsModel.h"

#include "Tools/Math/Common.h"
#include <Tools/DataConversion.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include <list>
#include <map>

BEGIN_DECLARE_MODULE(TeamMessageStatistics)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  
  REQUIRE(FrameInfo)
  REQUIRE(TeamMessage)

  PROVIDE(TeamMessageStatisticsModel)
END_DECLARE_MODULE(TeamMessageStatistics)

class TeamMessageStatistics: public TeamMessageStatisticsBase
{

public:
 TeamMessageStatistics();
 virtual ~TeamMessageStatistics();

 std::map<unsigned int, RobotMessageStatistics*> robotMap; // Map from robot number to its statistics object

 virtual void execute();
 void reset();
 void removeRobotNumbers();
 inline double normalDensity(double x) {
   return 1.0/std::sqrt(2 * Math::pi * getTeamMessageStatisticsModel().varianceMsgInterval) * 
     std::exp(-std::pow(x - getTeamMessageStatisticsModel().avgMsgInterval, 2)/(2 * getTeamMessageStatisticsModel().varianceMsgInterval));
 };
 double probability(double lower, double upper);
 double exponentialDistribution(double x);
 double riemann_integral(double (TeamMessageStatistics::*func)(double), double a, double b, int amountOfRectangles);

private:
   class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamMessageStatistics") {
      PARAMETER_REGISTER(interpolation) = 0.0;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    double interpolation; //Determines the weighting of newly received message intervals (set to 0 for unweighted average)
    
    virtual ~Parameters() {
    }
  } parameters;

};

#endif  /* _TeamMessageStatistics_H */