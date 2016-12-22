/**
 * @file StandingOnFeetDetector.cpp
 *
 * @author Yigit Can Akcay
 * Detect if feet have contact to the ground
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
    
}//end update
