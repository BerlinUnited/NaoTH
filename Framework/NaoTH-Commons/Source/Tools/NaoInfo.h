/**
 * @file NaoInfo.h
 *
 * Declaration of class NaoInfo
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Yuan Xu</a>
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 */

#ifndef _NaoInfo_h_
#define _NaoInfo_h_

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

#endif //_NaoInfo_h_

