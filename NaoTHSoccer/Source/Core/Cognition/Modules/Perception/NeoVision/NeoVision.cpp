/**
* @file NeoVision.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class NeoVision
*/

#include "NeoVision.h"

// Debug

NeoVision::NeoVision()
{
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:execute", " ", true);
  theFieldColorClassifierFull = registerModule<FieldColorClassifierFull>("FieldColorClassifierFull");
  theFieldColorClassifierFull->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:execute", " ", false);
  theSimpleFieldColorClassifier = registerModule<SimpleFieldColorClassifier>("SimpleFieldColorClassifier");
  theSimpleFieldColorClassifier->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:execute", " ", false);
  theSimpleGoalColorClassifier = registerModule<SimpleGoalColorClassifier>("SimpleGoalColorClassifier");
  theSimpleGoalColorClassifier->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:execute", " ", false);
  theSimpleBallColorClassifier = registerModule<SimpleBallColorClassifier>("SimpleBallColorClassifier");
  theSimpleBallColorClassifier->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:ScanLineEdgelDetectorDifferential:execute", " ", false);
  theScanLineEdgelDetectorDifferential = registerModule<ScanLineEdgelDetectorDifferential>("ScanLineEdgelDetectorDifferential");
  theScanLineEdgelDetectorDifferential->setEnabled(true);
}//end constructor


void NeoVision::execute()
{
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:execute",
    GT_TRACE("executing FieldColorClassifierFull");
    STOPWATCH_START("FieldColorClassifierFull");
    theFieldColorClassifierFull->execute();
    STOPWATCH_STOP("FieldColorClassifierFull");
  );

  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:execute",
    GT_TRACE("executing SimpleFieldColorClassifier");
    STOPWATCH_START("SimpleFieldColorClassifier");
    theSimpleFieldColorClassifier->execute();
    STOPWATCH_STOP("SimpleFieldColorClassifier");
  );

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:execute",
    GT_TRACE("executing SimpleGoalColorClassifier");
    STOPWATCH_START("SimpleGoalColorClassifier");
    theSimpleGoalColorClassifier->execute();
    STOPWATCH_STOP("SimpleGoalColorClassifier");
  );

  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:execute",
    GT_TRACE("executing SimpleBallColorClassifier");
    STOPWATCH_START("SimpleBallColorClassifier");
    theSimpleBallColorClassifier->execute();
    STOPWATCH_STOP("SimpleBallColorClassifier");
  );

  DEBUG_REQUEST("NeoVision:ScanLineEdgelDetectorDifferential:execute",
    GT_TRACE("executing ScanLineEdgelDetectorDifferential");
    STOPWATCH_START("ScanLineEdgelDetectorDifferential");
    theScanLineEdgelDetectorDifferential->execute();
    STOPWATCH_STOP("ScanLineEdgelDetectorDifferential");
  );
}//end execute

