/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformBase_h_
#define _PlatformBase_h_

#include <string>

namespace naoth
{

/* 
  * the PlatformBase holds (and provides access to) 
  * some basic information about the platform
  */
class PlatformBase
{
public:
  PlatformBase(const std::string& platformName, unsigned int basicTimeStep)
    :
    platformName(platformName),
    theBasicTimeStep(basicTimeStep)
  {}

  virtual ~PlatformBase() {
    std::cout << "destruct PlatformBase" << std::endl;
  }

  /////////////////////// get ///////////////////////
  virtual std::string getBodyID() const = 0;
  virtual std::string getBodyNickName() const = 0;
  virtual std::string getHeadNickName() const = 0;

  inline const std::string& getName() const { return platformName; }
  inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

private:
  std::string platformName;
  unsigned int theBasicTimeStep;
};//end class PlatformBase


}//end namespace naoth

#endif  // _PlatformBase_h_

