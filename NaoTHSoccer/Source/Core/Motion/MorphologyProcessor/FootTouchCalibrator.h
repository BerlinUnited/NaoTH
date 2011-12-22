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

  FootTouchDetector theLeftFootTouchDetector;
  FootTouchDetector theRightFootTouchDetector;

  RingBufferWithSum<Vector2<double>,100> predictionBuffer;

public:
  FootTouchCalibrator(const naoth::FSRData& theFSRData, const MotionStatus& theMotionStatus, const SupportPolygon& theSupportPolygon)
    :
    theFSRData(theFSRData),
    theMotionStatus(theMotionStatus),
    theSupportPolygon(theSupportPolygon)
  {
    string leftFootTouchDetectorCfg("1 1 1 1 -5");
    string rightFootTouchDetectorCfg("1 1 1 1 -5");
    const naoth::Configuration& cfg = naoth::Platform::getInstance().theConfiguration;
    string cfgname = "TouchDetector";
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
    // predict
    Vector2<double> p;

    bool leftFootSupportable = theSupportPolygon.isLeftFootSupportable();
    bool rightFootSupportable = theSupportPolygon.isRightFootSupportable();

    switch(theMotionStatus.stepControl.moveableFoot)
    {
      // left foot lifted
      case MotionStatus::MotionStatus::StepControlStatus::RIGHT: 
        //theLeftFootTouchDetector.calibrate(false);
        theRightFootTouchDetector.calibrate(true);
        p[1] = theSupportPolygon.isRightFootSupportable();
        break;

      // right foot lifted
      case MotionStatus::MotionStatus::StepControlStatus::LEFT: 
        theLeftFootTouchDetector.calibrate(true);
        //theRightFootTouchDetector.calibrate(false);
        p[0] = theSupportPolygon.isLeftFootSupportable();
        break;

      // both are on the ground
      default: 
        theLeftFootTouchDetector.calibrate(true);
        theRightFootTouchDetector.calibrate(true);
        p[0] = theSupportPolygon.isLeftFootSupportable();
        p[1] = theSupportPolygon.isRightFootSupportable();
        break;
    }//end switch

    predictionBuffer.add(p);
    
    Vector2<double> mean = predictionBuffer.getAverage();

    PLOT("FootTouchCalibrator:mean:X", mean.x);
    PLOT("FootTouchCalibrator:mean:Y", mean.y);
  }//end execute

};

#endif  // _FootTouchCalibrator_h_

