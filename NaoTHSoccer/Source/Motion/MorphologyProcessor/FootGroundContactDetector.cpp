/**
* @file FootGroundContactDetector.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#include "FootGroundContactDetector.h"

FootGroundContactDetector::FootGroundContactDetector():
    footParams(getDebugParameterList())
{
}

FootGroundContactDetector::~FootGroundContactDetector()
{
}

void FootGroundContactDetector::execute()
{

  double forceLeft  = 0;
  double forceRight = 0;

  for (int i = 0; i < FSRData::numOfFSR; i++) {
    forceLeft  += getFSRData().dataLeft[i];
    forceRight += getFSRData().dataRight[i];
  }

  leftFSRBuffer.add(forceLeft);
  rightFSRBuffer.add(forceRight);

  getGroundContactModel().leftGroundContact  = leftFSRBuffer.getMedian() > footParams.left;
  getGroundContactModel().rightGroundContact = rightFSRBuffer.getMedian() > footParams.right;


  if (!getGroundContactModel().leftGroundContact && !getGroundContactModel().rightGroundContact) {
    getGroundContactModel().supportFoot = GroundContactModel::NONE;
  } else if(rightFSRBuffer.getMedian() > leftFSRBuffer.getMedian()) {
    getGroundContactModel().supportFoot = GroundContactModel::RIGHT;
  } else {
    getGroundContactModel().supportFoot = GroundContactModel::LEFT;
  }

  PLOT("FootGroundContactDetector:leftGroundContact", getGroundContactModel().leftGroundContact);
  PLOT("FootGroundContactDetector:rightGroundContact", getGroundContactModel().rightGroundContact);

  PLOT("FootGroundContactDetector:leftFSRBuffer", leftFSRBuffer.getAverage());
  PLOT("FootGroundContactDetector:rightFSRBuffer", rightFSRBuffer.getAverage());

  /*
  // use the current to estimate the ground contact
  {
  static RingBufferWithSum<double, 10> currentBufferLeft;
  static RingBufferWithSum<double, 10> currentBufferRight;

  double currentLeftLeg = theBlackBoard.theSensorJointData.electricCurrent[JointData::LHipPitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::LHipRoll]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::LKneePitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::LAnklePitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::LAnkleRoll];

  currentBufferLeft.add(currentLeftLeg);

  double currentRightLeg = theBlackBoard.theSensorJointData.electricCurrent[JointData::RHipPitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::RHipRoll]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::RKneePitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::RAnklePitch]
    + theBlackBoard.theSensorJointData.electricCurrent[JointData::RAnkleRoll];

  currentBufferRight.add(currentRightLeg);

  PLOT("Motion:currentLeg:left", currentBufferLeft.getAverage() );
  PLOT("Motion:currentLeg:right", currentBufferRight.getAverage() );
  }
  */

}//end update

