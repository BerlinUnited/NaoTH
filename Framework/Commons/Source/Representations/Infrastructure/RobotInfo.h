/*
 * @file RobotInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @breief basic information about the robot
 *
 */

#ifndef ROBOTINFO_H
#define ROBOTINFO_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"


namespace naoth
{

class RobotInfo: public Printable
{
public:

  // Robot's id information
  std::string platform;     // platform type
  std::string robotName;    // name of the robot (usually hostname), e.g., nao32

  std::string headId;       // usually the serial number of the head, or the MAC of the LAN port
  std::string bodyId;       // serial number of the body

  // TODO: do we need that?
  std::string bodyNickName; // short id of the body, usually NaoXXXX, where XXXX are the last four digits of the bodyId


  // TODO: maybe this should be somewhere else
  // Time step of the sensor data and motion data.
  unsigned int basicTimeStep;
  
  /**
  * The method returns the basic time step in seconds
  */
  inline double getBasicTimeStepInSecond() const { return static_cast<double>(basicTimeStep)/1000.0; }


  virtual void print(std::ostream& stream) const
  {
    stream << "Platform:      " << platform       << std::endl;
    stream << "Name:          " << robotName      << std::endl;
    stream << "HeadId:        " << headId         << std::endl;
    stream << "BodyId:        " << bodyId         << std::endl;
    stream << "BodyNickName:  " << bodyNickName   << std::endl;
    stream << "BasicTimeStep: " << basicTimeStep  << std::endl;
  }
};

template<>
class Serializer<RobotInfo>
{
  public:
    static void serialize(const RobotInfo& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, RobotInfo& representation);
};

} // namespace naoth

#endif // ROBOTINFO_H
