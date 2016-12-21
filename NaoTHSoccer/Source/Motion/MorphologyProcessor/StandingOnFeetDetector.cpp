/**
 * @file StandingOnFeetDetector.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * detect if the foot touch the ground
 */

#include "StandingOnFeetDetector.h"

StandingOnFeetDetector::StandingOnFeetDetector():
footParams(getDebugParameterList())
{
}

StandingOnFeetDetector::~StandingOnFeetDetector()
{
}


void StandingOnFeetDetector::execute()
{
    
    // check wether the sensory values look valid
    // usually, broken sensors return large values
    for(int i = 0; i < naoth::FSRData::numOfFSR; i++)
    {
        if(getFSRData().data[i] > footParams.invalid)
            getFSRData().valid[i] = false;
        else
            getFSRData().valid[i] = true;
    }//end for
    
    
    leftFSRBuffer.add(getFSRData().forceLeft());
    rightFSRBuffer.add(getFSRData().forceRight());
    
    if(getFSRData().forceLeft() < footParams.left) {
        getGroundContactModel().leftGroundContact = false;
    } else {
        getGroundContactModel().leftGroundContact = true;
    }
    
    if(getFSRData().forceRight() < footParams.right)
    {
        getGroundContactModel().rightGroundContact = false;
    } else {
        getGroundContactModel().rightGroundContact = true;
    }
    
    
    if(leftFSRBuffer.getAverage() < footParams.left)
    {
        getGroundContactModel().leftGroundContactAverage = false;
    } else {
        getGroundContactModel().leftGroundContactAverage = true;
    }
    
    if(rightFSRBuffer.getAverage() < footParams.right) {
        getGroundContactModel().rightGroundContactAverage = false;
    } else {
        getGroundContactModel().rightGroundContactAverage = true;
    }
    
    PLOT("StandingOnFeetDetector:leftGroundContact", getGroundContactModel().leftGroundContact);
    PLOT("StandingOnFeetDetector:rightGroundContact", getGroundContactModel().rightGroundContact);
    
    PLOT("StandingOnFeetDetector:leftFSRBuffer", leftFSRBuffer.getAverage());
    PLOT("StandingOnFeetDetector:rightFSRBuffer", rightFSRBuffer.getAverage());
    
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

