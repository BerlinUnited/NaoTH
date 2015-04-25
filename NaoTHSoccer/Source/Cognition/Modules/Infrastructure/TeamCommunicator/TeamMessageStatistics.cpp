/**
* @file TeamMessageStatistics.cpp
*/

#include "TeamMessageStatistics.h"

TeamMessageStatistics::TeamMessageStatistics()
{
  getDebugParameterList().add(&parameters);
}

TeamMessageStatistics::~TeamMessageStatistics()
{
  getDebugParameterList().remove(&parameters);
}

void TeamMessageStatistics::execute() {   
  //Check, from which robots we have received a message, and update the corresponding statistics
  for (std::map<unsigned int, TeamMessage::Data>::const_iterator i = getTeamMessage().data.begin(); i != getTeamMessage().data.end(); ++i) {
    unsigned int robotNumber = i->first;
    unsigned int receiveTime = getFrameInfo().getTime();
    unsigned int receiveTime_sender = i->second.frameInfo.getTime();
    std::map<unsigned int, RobotMessageStatistics*>::const_iterator r = robotMap.find(robotNumber);
    if (r != robotMap.end()) {
      if ((*r->second).lastMessageReceived_sender < receiveTime_sender) {
        double currentMessageInterval = receiveTime - (*r->second).lastMessageReceived;

        //Update statistics for the corresponding robot
        (*r->second).messageReceived(receiveTime);
        (*r->second).lastMessageReceived_sender = receiveTime_sender;
        getTeamMessageStatisticsModel().amountsOfMessages[robotNumber] = (*r->second).amountOfMessages;
        getTeamMessageStatisticsModel().averages[robotNumber] = (*r->second).avgMsgInterval;
        getTeamMessageStatisticsModel().variances[robotNumber] = (*r->second).varianceMsgInterval;
        PLOT(std::string("MessageStatistics:Robot(")+DataConversion::toStr(robotNumber)+std::string("):MessageInterval"), currentMessageInterval);

        //Update statistics for the whole team
        if (robotNumber == getPlayerInfo().gameData.playerNumber) {
          continue; //Don't consider our own messages
        }
        double old_amountOfMessages = getTeamMessageStatisticsModel().amountOfMessages++;
        if (parameters.interpolation == 0.0 || old_amountOfMessages == 0 || 1.0/getTeamMessageStatisticsModel().amountOfMessages > parameters.interpolation) {
          //No interpolation, compute unweighted average
          getTeamMessageStatisticsModel().avgMsgInterval = (getTeamMessageStatisticsModel().avgMsgInterval * old_amountOfMessages + currentMessageInterval)
            /getTeamMessageStatisticsModel().amountOfMessages;
          getTeamMessageStatisticsModel().expectation_xSquared = (getTeamMessageStatisticsModel().expectation_xSquared * old_amountOfMessages + 
            std::pow(currentMessageInterval, 2))/getTeamMessageStatisticsModel().amountOfMessages;
          getTeamMessageStatisticsModel().varianceMsgInterval = getTeamMessageStatisticsModel().expectation_xSquared - 
            std::pow(getTeamMessageStatisticsModel().avgMsgInterval, 2);
        }
        else {
          //Interpolation of message intervals
          getTeamMessageStatisticsModel().avgMsgInterval = parameters.interpolation * currentMessageInterval + 
            (1.0 - parameters.interpolation) * getTeamMessageStatisticsModel().avgMsgInterval;
          getTeamMessageStatisticsModel().expectation_xSquared = parameters.interpolation * std::pow(currentMessageInterval, 2) + 
            (1.0 - parameters.interpolation) * getTeamMessageStatisticsModel().expectation_xSquared;
          getTeamMessageStatisticsModel().varianceMsgInterval = getTeamMessageStatisticsModel().expectation_xSquared - 
            std::pow(getTeamMessageStatisticsModel().avgMsgInterval, 2);
        }
        PLOT("MessageStatistics:Team:MessageInterval", currentMessageInterval);
      }
    }
    //If we haven't received a message from this robot before; add it to the map
    else {
      RobotMessageStatistics* rms = new RobotMessageStatistics(robotNumber);
      rms->messageReceived(receiveTime);
      rms->lastMessageReceived_sender = receiveTime_sender;
      robotMap.insert(std::make_pair(robotNumber, rms));
    }
  } 
  PLOT("MessageStatistics:Team:stddevMessageInterval", std::sqrt(getTeamMessageStatisticsModel().varianceMsgInterval));
  PLOT("MessageStatistics:Team:averageMessageInterval", getTeamMessageStatisticsModel().avgMsgInterval);

  //Run the update procedure (incremental computation of probability of failure) for each robot
  unsigned int currentTime = getFrameInfo().getTime();
  for (std::map<unsigned int, RobotMessageStatistics*>::const_iterator rms = robotMap.begin(); rms != robotMap.end(); rms++) {
    (*rms->second).update(currentTime);
    getTeamMessageStatisticsModel().failureProbabilities.insert(std::make_pair(rms->first, (*rms->second).probability_messageReceived_upToNow));  
    PLOT(std::string("MessageStatistics:Robot(")+DataConversion::toStr(rms->first)+std::string("):averageMessageInterval"), (*rms->second).avgMsgInterval);
    PLOT(std::string("MessageStatistics:Robot(")+DataConversion::toStr(rms->first)+std::string("):stddevMesssageInterval"), std::sqrt((*rms->second).varianceMsgInterval));
    PLOT(std::string("MessageStatistics:Robot(")+DataConversion::toStr(rms->first)+std::string("):probability"), 1.0-(*rms->second).probability_messageReceived_upToNow);
  } 
}

void TeamMessageStatistics::reset() {
  getTeamMessageStatisticsModel().amountOfMessages = 0;
  getTeamMessageStatisticsModel().avgMsgInterval = 0.0;
  getTeamMessageStatisticsModel().varianceMsgInterval = 0.0;
  getTeamMessageStatisticsModel().expectation_xSquared = 0.0;

  robotMap.clear();
}

double TeamMessageStatistics::probability(double lower, double upper) {
  if (upper <= 0.0) {
    return 0.0;
  }  
  double probability = riemann_integral(&TeamMessageStatistics::normalDensity, lower, upper, 1);

  return probability;
}

double TeamMessageStatistics::exponentialDistribution(double x) {
  return 1.0 - std::exp(-getTeamMessageStatisticsModel().avgMsgInterval * x);
}

/**
** Computes the Riemann Integral of some function for the given interval [a, b] and an amount of rectangles.
**/
double TeamMessageStatistics::riemann_integral(double (TeamMessageStatistics::*func)(double), double a, double b, int amountOfRectangles) {
		
	double *arguments = new double[amountOfRectangles];
	for (int i = 1; i <= amountOfRectangles; i++) {
		arguments[i-1] = a + i * (b - a)/amountOfRectangles;
	}
		
  double sum = 0.0;
	double x = a;
	for (int index = 0; index < amountOfRectangles; index++) {
		double newX = arguments[index];
		double value = (this->*func)((x+newX)/2.0);
		double area = value * (newX-x);
		sum += area;
		x = newX;
	}
	
	return sum;

}