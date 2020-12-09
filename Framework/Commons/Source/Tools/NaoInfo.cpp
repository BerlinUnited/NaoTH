#include "NaoInfo.h"

// http://doc.aldebaran.com/2-4/family/robots/links_robot.html
// Correct numbers for Nao V5 & V4 -- checked on 18.07.2017
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

// this isn't used anywhere
//const double NaoInfo::FSRMinForce = 0;
//const double NaoInfo::FSRMaxForce = 250;     // It is 25 in NAO DOC;


// http://doc.aldebaran.com/2-4/family/robots/fsr_robot.html#robot-fsr
const Vector3d NaoInfo::FSRPositionsLeft[naoth::FSRData::numOfFSR] =
{
  Vector3d( 70.25, 29.9,-FootHeight),
  Vector3d( 70.25,-23.1,-FootHeight),
  Vector3d(-30.25, 29.9,-FootHeight),
  Vector3d(-29.65,-19.1,-FootHeight)
};

const Vector3d NaoInfo::FSRPositionsRight[naoth::FSRData::numOfFSR] =
{
  Vector3d( 70.25, 23.1,-FootHeight),
  Vector3d( 70.25,-29.9,-FootHeight),
  Vector3d(-30.25, 19.1,-FootHeight),
  Vector3d(-29.65,-29.9,-FootHeight)
};

const NaoInfo::RobotDimensions NaoInfo::robotDimensions;
