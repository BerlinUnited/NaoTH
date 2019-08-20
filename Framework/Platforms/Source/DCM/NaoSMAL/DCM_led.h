 /* 
 * File:   DCMHandler.h
 * Author: Oliver Welter (welter@informatik.hu-berlin.de)
 *
 * Created on 14. Juli 2008, 13:16
 */

#ifndef _DCM_led_h_
#define	_DCM_led_h_

#include <string>

#include <boost/shared_ptr.hpp>

#include <alvalue/alvalue.h>
#include <alcommon/albroker.h>
#include <alproxies/dcmproxy.h>

#include "Representations/Infrastructure/LEDData.h"

namespace naoth
{

class DCM_led
{
private:
  boost::shared_ptr<AL::DCMProxy> dcm;

  // dcm paths
  std::string path_MonoLED[LEDData::numOfMonoLED];
  std::string path_MultiLED[LEDData::numOfMultiLED][LEDData::numOfLEDColor];

  //DCMCommand-Structures
  AL::ALValue allLedCommand;
  AL::ALValue singleLedCommand;

  // remember last commands (needed by "smart" methods) 
  LEDData lastLEDData;

public:
  DCM_led();
  ~DCM_led();
  void init(boost::shared_ptr<AL::ALBroker> pB);

  void setAllLED(const LEDData& data, int dcmTime);
  bool setSingleLED(const LEDData& data, int dcmTime);
  bool setLEDSmart(const LEDData& data, int dcmTime);

};//end class DCM_led

}//end namespace naoth
#endif	/* _DCM_led_h_ */

