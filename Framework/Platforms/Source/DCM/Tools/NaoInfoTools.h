/**
* @file NaoInfoTools.h
*
* Class NaoTime provides information about the robot, such as ids and name.
* The names and the ids are read from system files on the robot NAO6.
*
* @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
*/

#ifndef NAO_INFO_TOOLS_H
#define NAO_INFO_TOOLS_H

#include <string>
#include <fstream>
#include <iostream>

namespace naoth
{
class NaoInfoTools
{
public:

  static inline std::string readRobotName() {
    // RobotName is the same as the hostname
    char hostname[128];
    // returns the null-terminated hostname
    gethostname(hostname, 127);
    return std::string(hostname);
  }
  
  static inline std::string readRobotType() {
    std::ifstream is("/sys/qi/robot_type");
    std::string robotType;
    is >> robotType;
    return robotType;
  }
  
  static inline std::string readHeadId() {
    std::ifstream is("/sys/qi/head_id");
    std::string headId;
    is >> headId;
    return headId;
  }
  
  /*
  // this function is a place holder. There is no simple way to ind out the current body number.
  static inline std::string readBodyId() {
    std::ifstream is("/sys/qi/???");
    std::string bodyId;
    is >> bodyId;
    return bodyId;
  }
  */
  
  // generate a short id for the body, e.g., NaoXXXX, 
  // where XXXX are the last 4 digits of the body ID
  static inline std::string makeBodyNickName(std::string bodyId) {
    std::string theBodyNickName = "error";
    if(bodyId.length() >= 4) {
      return "Nao" + bodyId.substr( bodyId.length() - 4 );
    }
    
    return "error";
  }
  
};
}// end namespace naoth
#endif  /* NAO_INFO_TOOLS_H */

