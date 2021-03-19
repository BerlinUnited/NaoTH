/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformBase_h_
#define _PlatformBase_h_

#include <string>
#include <iostream>

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

namespace naoth
{

/* 
  * the PlatformBase provides access to
  * some basic information about the platform
  */
class PlatformBase
{
public:

  // Platform Info
  virtual std::string getPlatformName() const = 0;

  virtual std::string getRobotName() const = 0;
  virtual std::string getBodyID() const = 0;
  virtual std::string getBodyNickName() const = 0;
  virtual std::string getHeadNickName() const = 0;
  
  // Robot Info
  virtual unsigned int getBasicTimeStep() const = 0;

};//end class PlatformBase


}//end namespace naoth

#endif  // _PlatformBase_h_

