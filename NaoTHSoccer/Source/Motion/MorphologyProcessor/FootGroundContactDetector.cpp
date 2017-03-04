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

  leftFSRBuffer.add(Math::median(getFSRData().dataLeft));
  rightFSRBuffer.add(Math::median(getFSRData().dataRight));

  getGroundContactModel().leftGroundContact  = leftFSRBuffer.getMedian() > footParams.left;
  getGroundContactModel().rightGroundContact = rightFSRBuffer.getMedian() > footParams.right;


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

