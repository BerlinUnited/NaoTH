/**
 * @file NaoInfo.h
 *
 * Declaration of class NaoInfo
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Yuan Xu</a>
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 */

#ifndef __NaoInfo_h_
#define __NaoInfo_h_

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector3.h"
#include "Math/Matrix.h"

class NaoInfo
{
public:
  //length of the links (arm, leg, etc.) - from the doc in sdk
  static double NeckOffsetZ;
  static double ShoulderOffsetY;
  static double UpperArmLength;
  static double LowerArmLength;
  static double ShoulderOffsetZ;
  static double HandOffsetX;
  static double HipOffsetZ;
  static double HipOffsetY;
  static double ThighLength;
  static double TibiaLength;
  static double FootHeight;
  static double HandOffsetZ;
  static double ElbowOffsetY;
  static double LegJointToHeadPan;

  static double FSRMinForce;
  static double FSRMaxForce;
}; //end namespace NaoInfo

#endif //__NaoInfo_h_

