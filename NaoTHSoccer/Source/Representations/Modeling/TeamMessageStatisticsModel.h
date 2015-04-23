
/*
* @file TeamMessageStatisticsModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
* Representation of message statistics
*/

#ifndef __TeamMessageStatisticsModel_H
#define __TeamMessageStatisticsModel_H

#include <Tools/DataStructures/Printable.h>

#include <list>
#include <map>

class TeamMessageStatisticsModel : public naoth::Printable
{

public:
  
  unsigned int amountOfMessages; // Total amount of messages received
  double avgMsgInterval; // Average interval between a pair of messages sent by the same robot
  double varianceMsgInterval; // Variance of the interval between a pair of messages sent by the same robot
  double expectation_xSquared; // Sum of squares of the message intervals
  bool valid;
  
  std::map<unsigned int, double> amountsOfMessages; // Map from a robot's number to its current amount of messages
  std::map<unsigned int, double> averages; // Map from a robot's number to its current average message interval
  std::map<unsigned int, double> variances; // Map from a robot's number to its current failure probability
  std::map<unsigned int, double> failureProbabilities; // Map from a robot's number to its current failure probability

  TeamMessageStatisticsModel()
    :
      amountOfMessages(0),
      avgMsgInterval(std::numeric_limits<double>::max()),
      varianceMsgInterval(std::numeric_limits<double>::max()),
      expectation_xSquared(std::numeric_limits<double>::max())
    {}

  virtual ~TeamMessageStatisticsModel(){}

  virtual void print(std::ostream& stream) const
  {
    stream << "Overall Statistics:" << std::endl;
    stream << " Amount of Messages =                       " << amountOfMessages << std::endl;
    stream << " Average Message Interval =                 " << avgMsgInterval << std::endl;
    stream << " Stddev of Message Intervals =              " << std::sqrt(varianceMsgInterval) << std::endl << std::endl;
    for (std::map<unsigned int, double>::const_iterator iter = amountsOfMessages.begin(); iter != amountsOfMessages.end(); iter++) {
      unsigned int robotNumber = iter->first;
      stream << "Statistics for robot number " << robotNumber << ":" << std::endl;
      stream << " Amount of Messages =                       " << iter->second << std::endl;
      stream << " Average Message Interval =                 " << averages.at(robotNumber) << std::endl;
      stream << " Stddev of Message Intervals =              " << std::sqrt(variances.at(robotNumber)) << std::endl;
    }
  }//end print
};

#endif// __TeamMessageStatistics_H


