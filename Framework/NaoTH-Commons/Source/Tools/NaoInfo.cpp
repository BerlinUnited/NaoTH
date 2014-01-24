#include "NaoInfo.h"

// todo: those numbers are not the same for each robot
const double NaoInfo::NeckOffsetZ = 126.5;
const double NaoInfo::ShoulderOffsetY = 98;
const double NaoInfo::UpperArmLength = 105.00; // v3.3
const double NaoInfo::LowerArmLength = 50.55; //55.95 // v3.3
const double NaoInfo::ShoulderOffsetZ = 100;
const double NaoInfo::HandOffsetX = 58; // 57.75 // v3.3
const double NaoInfo::HipOffsetZ = 85;
const double NaoInfo::HipOffsetY = 50;
const double NaoInfo::ThighLength = 100;
const double NaoInfo::TibiaLength = 102.74;
const double NaoInfo::FootHeight = 45.11; // 45.19 // v3.3
const double NaoInfo::HandOffsetZ = 15.90; // 12.31 // v3.3
const double NaoInfo::ElbowOffsetY = 0; // 15.00 // v3.3
const double NaoInfo::LegJointToHeadPan = 211.5;

const double NaoInfo::FSRMinForce = 0;
const double NaoInfo::FSRMaxForce = 250;// It is 25 in NAO DOC;


const Vector3d NaoInfo::FSRPositions[naoth::FSRData::numOfFSR] =
{
  Vector3d( 70.1, 30,-46),
  Vector3d( 70.1,-23,-46),
  Vector3d(-30.4, 30,-46),
  Vector3d(-29.8,-19,-46),
  Vector3d( 70.1, 23,-46),
  Vector3d( 70.1,-30,-46),
  Vector3d(-29.8, 19,-46),
  Vector3d(-30.4,-30,-46)
};

const NaoInfo::RobotDimensions NaoInfo::robotDimensions;