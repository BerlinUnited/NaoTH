/**
 * @file NaoInfo.h
 *
 * Declaration of class NaoInfo
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 */

#ifndef __NaoInfo_h_
#define __NaoInfo_h_

namespace NaoInfo {

//length of the links (arm, leg, etc.) - from the doc in sdk
const double NeckOffsetZ = 126.5;
const double ShoulderOffsetY = 98;
const double UpperArmLength = 90;
const double LowerArmLength = 50.55;
const double ShoulderOffsetZ = 100;
const double HandOffsetX = 58;
const double HipOffsetZ = 85;
const double HipOffsetY = 50;
const double ThighLength = 100;
const double TibiaLength = 102.74;
const double FootHeight = 45.11;
const double HandOffsetZ = 15.90;

const double FSRMinForce = 0;
const double FSRMaxForce = 250;// It is 25 in NAO DOC;
}//end namespace NaoInfo



#endif //__NaoInfo_h_

