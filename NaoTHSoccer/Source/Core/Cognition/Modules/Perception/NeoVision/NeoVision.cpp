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
  theSimpleFieldColorClassifier = registerModule<SimpleFieldColorClassifier>("SimpleFieldColorClassifier");
  theSimpleFieldColorClassifier->setEnabled(true);

  theScanLineEdgelDetectorDifferential = registerModule<ScanLineEdgelDetectorDifferential>("ScanLineEdgelDetectorDifferential");
  theScanLineEdgelDetectorDifferential->setEnabled(true);
}//end constructor


void NeoVision::execute()
{
  GT_TRACE("executing ScanLineEdgelDetectorDifferential");
  STOPWATCH_START("ScanLineEdgelDetectorDifferential");
  theScanLineEdgelDetectorDifferential->execute();
  STOPWATCH_STOP("ScanLineEdgelDetectorDifferential");
}//end execute

