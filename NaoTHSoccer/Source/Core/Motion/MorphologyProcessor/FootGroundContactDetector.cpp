/**
* @file FootGroundContactDetector.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#include "FootGroundContactDetector.h"

#include "Tools/Debug/DebugBufferedOutput.h"

FootGroundContactDetector::FootGroundContactDetector(const MotionBlackBoard& theBlackBoard, naoth::FSRData& theFSRData, GroundContactModel& theGroundContactModel)
  : 
    theBlackBoard(theBlackBoard),
    theFSRData(theFSRData),
    theGroundContactModel(theGroundContactModel)
{
}

FootGroundContactDetector::~FootGroundContactDetector()
{
}


void FootGroundContactDetector::update()
{

  // check wether the sensory values look valid
  // usually, broken sensors return large values
  for(int i = 0; i < naoth::FSRData::numOfFSR; i++)
  {
    if(theFSRData.data[i] > 100)
      theFSRData.valid[i] = false;
    else
      theFSRData.valid[i] = true;
  }//end for


  leftFSRBuffer.add(theFSRData.forceLeft());
  rightFSRBuffer.add(theFSRData.forceRight());

  if(theFSRData.forceLeft() < 3)
    theGroundContactModel.leftGroundContact = false;
  else
    theGroundContactModel.leftGroundContact = true;

  if(theFSRData.forceRight() < 3)
    theGroundContactModel.rightGroundContact = false;
  else
    theGroundContactModel.rightGroundContact = true;


  
  if(leftFSRBuffer.getAverage() < 3)
    theGroundContactModel.leftGroundContactAverage = false;
  else
    theGroundContactModel.leftGroundContactAverage = true;

  if(rightFSRBuffer.getAverage() < 3)
    theGroundContactModel.rightGroundContactAverage = false;
  else
    theGroundContactModel.rightGroundContactAverage = true;
  


  PLOT("FootGroundContactDetector:leftGroundContact", theGroundContactModel.leftGroundContact);
  PLOT("FootGroundContactDetector:rightGroundContact", theGroundContactModel.rightGroundContact);

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

