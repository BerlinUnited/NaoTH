#include "NaoInfo.h"

// todo: those numbers are not the same for each robot
const double NaoInfo::NeckOffsetZ = 126.5;
const double NaoInfo::ShoulderOffsetY = 98;
const double NaoInfo::UpperArmLength = 105.00;
const double NaoInfo::LowerArmLength = 55.95;
const double NaoInfo::ShoulderOffsetZ = 100;
const double NaoInfo::HandOffsetX = 57.75;
const double NaoInfo::HandOffsetZ = 12.31;
const double NaoInfo::HipOffsetZ = 85;
const double NaoInfo::HipOffsetY = 50;
const double NaoInfo::ThighLength = 100;
const double NaoInfo::TibiaLength = 102.90;
const double NaoInfo::FootHeight = 45.19;
const double NaoInfo::ElbowOffsetY = 15.00;

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