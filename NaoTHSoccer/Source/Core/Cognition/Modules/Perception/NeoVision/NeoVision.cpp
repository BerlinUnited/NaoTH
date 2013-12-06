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
  DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, FieldColorClassifierFull, " ", false);
  //DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, SimpleFieldColorClassifier, " ", true);

  DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, SimpleGoalColorClassifier, " ", false);
  DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, SimpleBallColorClassifier, " ", false);

  //DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, ScanLineEdgelDetectorDifferential, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, NeoLineDetector, " ", false);

  //DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, MaximumRedBallDetector, " ", true);
  //DEBUG_REQUEST_REGISTER_SUBMODULE(NeoVision, GradientGoalDetector, " ", true);
}//end constructor


void NeoVision::execute()
{
  DEBUG_EXECUTE_SUBMODULE(NeoVision, FieldColorClassifierFull);
  //DEBUG_EXECUTE_SUBMODULE(NeoVision, SimpleFieldColorClassifier);

  DEBUG_EXECUTE_SUBMODULE(NeoVision, SimpleGoalColorClassifier);
  DEBUG_EXECUTE_SUBMODULE(NeoVision, SimpleBallColorClassifier);

  //DEBUG_EXECUTE_SUBMODULE(NeoVision, ScanLineEdgelDetectorDifferential);
  DEBUG_EXECUTE_SUBMODULE(NeoVision, NeoLineDetector);

  //DEBUG_EXECUTE_SUBMODULE(NeoVision, MaximumRedBallDetector);
  //DEBUG_EXECUTE_SUBMODULE(NeoVision, GradientGoalDetector);
}//end execute

