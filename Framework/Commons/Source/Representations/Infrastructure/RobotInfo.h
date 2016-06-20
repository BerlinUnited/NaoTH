/*
 * @file RobotInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief some basic information about robot
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
  RobotInfo(){}

  std::string platform;
  std::string bodyNickName;
  std::string headNickName;
  std::string bodyID;

  /** The time of each step which depends on platforms */
  unsigned int basicTimeStep; 
  
  /**
  * The method returns the basic time step in seconds
  */
  inline double getBasicTimeStepInSecond() const { return static_cast<double>(basicTimeStep)/1000.0; }


  virtual void print(std::ostream& stream) const
  {
    stream << "Platform: " << platform << std::endl;
    stream << "HeadNickName: " << headNickName << std::endl;
    stream << "BodyNickName: " << bodyNickName << std::endl;
    stream << "BodyID: " << bodyID << std::endl;
    stream << "basicTimeStep: " << basicTimeStep << std::endl;
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
