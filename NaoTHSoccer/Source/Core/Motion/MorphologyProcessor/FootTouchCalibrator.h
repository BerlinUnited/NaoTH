/**
* @file FootTouchCalibrator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Declaration of the class FootTouchCalibrator
*/

#ifndef _FootTouchCalibrator_h_
#define  _FootTouchCalibrator_h_

#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/SupportPolygon.h"
#include "Representations/Modeling/KinematicChain.h"
#include <Representations/Infrastructure/FSRData.h>
#include "FootTouchDetector.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Vector2.h"
#include "PlatformInterface/Platform.h"

#include "Tools/Debug/DebugBufferedOutput.h"

class FootTouchCalibrator
{
private:
  const naoth::FSRData& theFSRData;
  const MotionStatus& theMotionStatus;
  const SupportPolygon& theSupportPolygon;
  const KinematicChain& theKinematicChain;

  FootTouchDetector theLeftFootTouchDetector;
  FootTouchDetector theRightFootTouchDetector;

  RingBufferWithSum<Vector2<double>,100> predictionBuffer;

public:
  FootTouchCalibrator(const naoth::FSRData& theFSRData, const MotionStatus& theMotionStatus, const SupportPolygon& theSupportPolygon, const KinematicChain& theKinematicChain)
    :
    theFSRData(theFSRData),
    theMotionStatus(theMotionStatus),
    theSupportPolygon(theSupportPolygon),
    theKinematicChain(theKinematicChain)
  {
    std::string leftFootTouchDetectorCfg("1 1 1 1 -5");
    std::string rightFootTouchDetectorCfg("1 1 1 1 -5");
    const naoth::Configuration& cfg = naoth::Platform::getInstance().theConfiguration;
    std::string cfgname = "TouchDetector";
    if ( cfg.hasGroup(cfgname) )
    {
      leftFootTouchDetectorCfg = cfg.getString(cfgname, "leftFoot");
      rightFootTouchDetectorCfg = cfg.getString(cfgname, "rightFoot");
    }
    
    theLeftFootTouchDetector.init(theFSRData.data + naoth::FSRData::LFsrFL, leftFootTouchDetectorCfg);
    theRightFootTouchDetector.init(theFSRData.data + naoth::FSRData::RFsrFL, rightFootTouchDetectorCfg);
  }//


  void execute()
  {
    double leftFootHeight = theKinematicChain.theLinks[KinematicChain::LFoot].p.z;
    double rightFootHeight = theKinematicChain.theLinks[KinematicChain::RFoot].p.z;

    bool leftFootTouch = false;
    bool rightFootTouch = false;

    rightFootTouch = !(rightFootHeight > leftFootHeight + 5);
    leftFootTouch = !(leftFootHeight > rightFootHeight + 5);

    // predict
    Vector2<double> p;
    p[0] = theRightFootTouchDetector.isTouch();
    p[1] = theLeftFootTouchDetector.isTouch();


    theRightFootTouchDetector.calibrate(rightFootTouch);
    theLeftFootTouchDetector.calibrate(leftFootTouch);

    predictionBuffer.add(p);
    
    Vector2<double> mean = predictionBuffer.getAverage();

    PLOT("FootTouchCalibrator:target:left", leftFootTouch);
    PLOT("FootTouchCalibrator:prediction:left", p[1]);

    //PLOT("FootTouchCalibrator:mean:right", mean.x);
    PLOT("FootTouchCalibrator:mean:left", mean.y);
  }//end execute

};

#endif  // _FootTouchCalibrator_h_

