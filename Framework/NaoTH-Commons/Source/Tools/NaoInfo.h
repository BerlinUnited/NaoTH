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

// TODO: this should not be here
#include <Representations/Infrastructure/FSRData.h>

class NaoInfo
{
public:
  //length of the links (arm, leg, etc.) - from the doc in sdk
  static const double NeckOffsetZ;
  static const double ShoulderOffsetY;
  static const double UpperArmLength;
  static const double LowerArmLength;
  static const double ShoulderOffsetZ;
  static const double HandOffsetX;
  static const double HipOffsetZ;
  static const double HipOffsetY;
  static const double ThighLength;
  static const double TibiaLength;
  static const double FootHeight;
  static const double HandOffsetZ;
  static const double ElbowOffsetY;
  static const double LegJointToHeadPan;

  static const double FSRMinForce;
  static const double FSRMaxForce;

  static const Vector3d FSRPositions[naoth::FSRData::numOfFSR];
  
}; //end namespace NaoInfo

#endif //_NaoInfo_h_

