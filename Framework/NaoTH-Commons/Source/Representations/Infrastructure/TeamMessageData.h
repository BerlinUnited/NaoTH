/*
 * @file RobotMessage.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef ROBOT_MESSAGE_DATA_H
#define ROBOT_MESSAGE_DATA_H

#include <string>
#include <vector>
#include "PlatformInterface/PlatformInterchangeable.h"

namespace naoth
{

// message to other robots
class RobotMessageData: public PlatformInterchangeable
{
public:
  std::string data;
};

// message from other robots
class TeamMessageData: public PlatformInterchangeable
{
public:
  std::vector<std::string> data;
};

} // namespace naoth

#endif // ROBOT_MESSAGE_DATA_H
