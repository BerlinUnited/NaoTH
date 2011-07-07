/* 
 * File:   LEDSetter.h
 * Author: thomas
 *
 * Created on 22. April 2009, 16:55
 */


#ifndef _LEDSETTER_H
#define	_LEDSETTER_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/LEDRequest.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/FrameInfo.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(LEDSetter)

REQUIRE(GameControllerLEDRequest)
REQUIRE(BehaviorLEDRequest)
REQUIRE(FrameInfo)

PROVIDE(LEDData)

END_DECLARE_MODULE(LEDSetter)

class LEDSetter : public LEDSetterBase
{
public:
  LEDSetter();
  virtual ~LEDSetter();

  virtual void execute();
private:

  void copyMonoLEDData(const LEDRequest& data, int from, int to);
  void copyMultiLEDData(const LEDRequest& data, int from, int to);
};

#endif	/* _LEDSETTER_H */

