/*
* @file TeamCommReceiveEmulator.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
* Generates artificial messages for the purpose of testing the TeamMessageStatistics module.
*/

#ifndef _TeamCommReceiveEmulator_H
#define _TeamCommReceiveEmulator_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Modeling/TeamMessage.h"
#include <Representations/Infrastructure/TeamMessageData.h>

#include "Tools/Math/Common.h"
#include "Tools/Math/Probabilistics.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include <list>
#include <map>

BEGIN_DECLARE_MODULE(TeamCommReceiveEmulator)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  
  REQUIRE(FrameInfo)
  PROVIDE(TeamMessage)
END_DECLARE_MODULE(TeamCommReceiveEmulator)

class TeamCommReceiveEmulator: public TeamCommReceiveEmulatorBase
{

public:
 TeamCommReceiveEmulator();
 virtual ~TeamCommReceiveEmulator();

 virtual void execute();
 void reset();
 void removeRobotNumbers();

private:
  unsigned int nextMessageTime; //Time, at which the next message is to be received

  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamCommReceiveEmulator") {
      PARAMETER_REGISTER(normalDistribution) = true;
      PARAMETER_REGISTER(mean) = 500.0;
      PARAMETER_REGISTER(standardDeviation) = 50.0;
      PARAMETER_REGISTER(uniformDistribution) = false;
      PARAMETER_REGISTER(uniformMin) = 413;
      PARAMETER_REGISTER(uniformMax) = 586;
      PARAMETER_REGISTER(randomPerturbations) = false;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }
    
    bool normalDistribution; //Should the message intervals be drawn from a normal distribution?
    bool uniformDistribution; //Should the message intervals be drawn from a uniform distribution?
    bool randomPerturbations; //If true, message intervals will be drawn according to the chosen distribution at times, 
                              //but, from time to time, no messages will be received for a longer time period
    double mean; //Mean message interval (in ms)
    double standardDeviation; //Standard deviation of message intervals (in ms)
    int uniformMin; //Smallest number in the uniform distribution (in ms)
    int uniformMax; //Largest number in the uniform distribution (in ms)
    
    virtual ~Parameters() {
    }
  } parameters;

};

#endif  /* _TeamCommReceiveEmulator_H */