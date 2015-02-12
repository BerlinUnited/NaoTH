/*
* @file RobotMessageStatistics.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
* Statistics for a single robot
*/

#ifndef _RobotMessageStatistics_H
#define _RobotMessageStatistics_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Modeling/TeamMessage.h"
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamMessageStatisticsModel.h"

#include "Tools/Math/Common.h"

#include <list>
#include <map>

class RobotMessageStatistics
{

public:
  RobotMessageStatistics(unsigned int robotNumber);
  ~RobotMessageStatistics();

  const unsigned int robotNumber;
  bool valid;

  unsigned int amountOfMessages;
  unsigned int lastMessageReceived; //Time, at which the last message was received from this robot
  unsigned int lastMessageReceived_sender; //Time stamp sent by this robot
  unsigned int lastUpdate; //Time of last update of the probability, relative to "lastMessageReceived"
  double probability_messageReceived_upToNow;
  double avgMsgInterval; // Average interval between a pair of messages sent by this robot
  double varianceMsgInterval; // Variance of the interval between a pair of messages sent by this robot
  double expectation_xSquared; // Sum of squares of the message intervals

  void reset();
  void messageReceived(unsigned int message_receiveTime, double interpolation=0.0);
  void update(unsigned int currentTime);
  inline double normalDensity(double x) {
    return 1.0/std::sqrt(2 * Math::pi * varianceMsgInterval) * std::exp(-std::pow(x - avgMsgInterval, 2)/(2 * varianceMsgInterval));
  };
  double exponentialDistribution(double x);
  double probability(double lower, double upper);
  double riemann_integral(double (RobotMessageStatistics::*func)(double), double a, double b, int amountOfRectangles);

};

#endif  /* _TeamMessageStatistics_H */