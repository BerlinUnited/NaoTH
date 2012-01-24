/*
 * @file RobotInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief some basic information about robot
 *
 */

#ifndef ROBOTINFO_H
#define ROBOTINFO_H

#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

namespace naoth
{

class RobotInfo: public PlatformInterchangeable, public Printable
{
public:
  RobotInfo(){}

  std::string platform;
  std::string bodyNickName;
  std::string bodyID;

  unsigned int basicTimeStep; /**< The time of each step which depends on platforms */
  /**
   * The method returns the basic time step in seconds
   */
  inline double getBasicTimeStepInSecond() const { return static_cast<double>(basicTimeStep)/1000.0; }


  virtual void print(ostream& stream) const
  {
    stream<<"Platform: "<<platform<<"\n";
    stream<<"BodyNickName: "<<bodyNickName<<"\n";
    stream<<"BodyID: "<<bodyID<<"\n";
    stream<<"basicTimeStep: "<<basicTimeStep<<"\n";
  }
};

} // namespace naoth

#endif // ROBOTINFO_H
