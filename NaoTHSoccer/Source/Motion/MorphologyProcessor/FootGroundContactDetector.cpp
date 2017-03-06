/**
* @file FootGroundContactDetector.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:yigit.akcay@icloud.com">Yigit, Akcay</a>
* detect if the foot touch the ground
*/

#include "FootGroundContactDetector.h"
#include "Tools/Math/Common.h"

FootGroundContactDetector::FootGroundContactDetector()
{
  getDebugParameterList().add(&footParams);
}

FootGroundContactDetector::~FootGroundContactDetector()
{
  getDebugParameterList().remove(&footParams);
}

void FootGroundContactDetector::execute()
{
  if (footParams.useMaxMedian){
    leftFSRBuffer.add(Math::medianMax(getFSRData().dataLeft));
    rightFSRBuffer.add(Math::medianMax(getFSRData().dataRight));
  }
  else if (footParams.useMax){
    //Maximum of FSR Sensors
    leftFSRBuffer.add(Math::get_nth_element(getFSRData().dataLeft, getFSRData().dataLeft.size()));
    rightFSRBuffer.add(Math::get_nth_element(getFSRData().dataRight, getFSRData().dataRight.size()));
  }
  else{
    //Median
    leftFSRBuffer.add(Math::get_nth_element(getFSRData().dataLeft, getFSRData().dataLeft.size() / 2));
    rightFSRBuffer.add(Math::get_nth_element(getFSRData().dataRight, getFSRData().dataRight.size() / 2));
  }


  getGroundContactModel().leftGroundContact  = leftFSRBuffer.getAverage() > footParams.left;
  getGroundContactModel().rightGroundContact = rightFSRBuffer.getAverage() > footParams.right;


  if (!getGroundContactModel().leftGroundContact && !getGroundContactModel().rightGroundContact) {
    getGroundContactModel().supportFoot = GroundContactModel::NONE;
  } else if(rightFSRBuffer.getAverage() > leftFSRBuffer.getAverage()) {
    getGroundContactModel().supportFoot = GroundContactModel::RIGHT;
  } else {
    getGroundContactModel().supportFoot = GroundContactModel::LEFT;
  }

  PLOT("FootGroundContactDetector:leftGroundContact", getGroundContactModel().leftGroundContact);
  PLOT("FootGroundContactDetector:rightGroundContact", getGroundContactModel().rightGroundContact);

  PLOT("FootGroundContactDetector:leftFSRBuffer", leftFSRBuffer.getAverage());
  PLOT("FootGroundContactDetector:rightFSRBuffer", rightFSRBuffer.getAverage());

}//end update

