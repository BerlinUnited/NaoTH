/**
* @file RobotMessageStatistics.cpp
*/

#include "RobotMessageStatistics.h"


RobotMessageStatistics::RobotMessageStatistics(unsigned int robotNumber):
  robotNumber(robotNumber),
  valid(false),
  amountOfMessages(0),
  lastMessageReceived(0),
  lastUpdate(0),
  probability_messageReceived_upToNow(0.0),
  avgMsgInterval(0.0),
  varianceMsgInterval(0.0),
  expectation_xSquared(0.0)
{}

RobotMessageStatistics::~RobotMessageStatistics()
{}

/**
** Increments the iterative update procedure by updating the expectation of squares, average, and standard deviation of the message intervals
** @param robotNumber: The number of the robot that sent the message
**        message_receiveTime: Time, at which the message was received
**        interpolation: Weight for the new message interval (is set in TeamMessageStatistics)
**/
void RobotMessageStatistics::messageReceived(unsigned int message_receiveTime, double interpolation) {
  if (valid) {
    double currentMessageInterval = message_receiveTime - lastMessageReceived;
    double old_amountOfMessages = amountOfMessages++;
    if (interpolation == 0.0 || old_amountOfMessages == 0 || 1.0/amountOfMessages > interpolation) {
      //No interpolation, compute unweighted average
      avgMsgInterval = (avgMsgInterval * old_amountOfMessages + currentMessageInterval)/amountOfMessages;
      expectation_xSquared = (expectation_xSquared * old_amountOfMessages + std::pow(currentMessageInterval, 2))/amountOfMessages;
      varianceMsgInterval = expectation_xSquared - std::pow(avgMsgInterval, 2);
    }
    else {
      //Interpolation of message intervals
      avgMsgInterval = interpolation * currentMessageInterval + (1.0 - interpolation) * avgMsgInterval;
      expectation_xSquared = interpolation * std::pow(currentMessageInterval, 2) + 
        (1.0 - interpolation) * expectation_xSquared;
      varianceMsgInterval = expectation_xSquared - 
        std::pow(avgMsgInterval, 2);
    }
  }
  else {
    valid = true;
  }

  probability_messageReceived_upToNow = 0.0;
  lastMessageReceived = message_receiveTime;
  lastUpdate = 0;
}

/**
** Updates the probability of failure for this robot at the current time step.
**/
void RobotMessageStatistics::update(unsigned int currentTime) {
  unsigned int currentTime_relative = currentTime - lastMessageReceived;
  std::cout << "RobotNo = " << robotNumber << ", AmountOfMsg = " << amountOfMessages << ", Cur_Time = " << currentTime_relative << ", LastUpdate = " << lastUpdate << std::endl;
  if (varianceMsgInterval > 0) {
    probability_messageReceived_upToNow += probability(lastUpdate, currentTime_relative);
  }
  std::cout << "  Probability of a message arriving before " << currentTime_relative << "ms having elapsed = " << probability_messageReceived_upToNow << std::endl;
  std::cout << "  With mean = " << avgMsgInterval << ", stddev = " << std::sqrt(varianceMsgInterval) << std::endl;
  lastUpdate = currentTime_relative;
}

void RobotMessageStatistics::reset() {
  amountOfMessages = 0;
  avgMsgInterval = 0.0;
  varianceMsgInterval = 0.0;
  expectation_xSquared = 0.0;
  lastMessageReceived = 0;
}

double RobotMessageStatistics::exponentialDistribution(double x) {
  return 1.0 - std::exp(-avgMsgInterval * x);
}

double RobotMessageStatistics::probability(double lower, double upper) {
  if (upper <= lower) {
    return 0.0;
  }  
  double probability = riemann_integral(&RobotMessageStatistics::normalDensity, lower, upper, 1);

  return probability;
}

/**
** Computes the Riemann Integral of some function for the given interval [a, b] and an amount of rectangles.
**/
double RobotMessageStatistics::riemann_integral(double (RobotMessageStatistics::*func)(double), double a, double b, int amountOfRectangles) {
		
	double *arguments = new double[amountOfRectangles];
	for (int i = 1; i <= amountOfRectangles; i++) {
		arguments[i-1] = a + i * (b - a)/amountOfRectangles;
	}
		
  double sum = 0.0;
	double x = a;
	for (int index = 0; index < amountOfRectangles; index++) {
		double newX = arguments[index];
		double value = std::min((this->*func)(x), (this->*func)(newX));
		double area = value * (newX-x);
		sum += area;
		x = newX;
	}
	
	return sum;

}