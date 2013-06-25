/**
* @file NeoVision.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class NeoVision
*/

#ifndef _NeoVision_H_
#define _NeoVision_H_

// infrastructure
#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// submodules
#include "modules/FieldColorClassifierFull.h"
#include "modules/SimpleFieldColorClassifier.h"
#include "modules/SimpleGoalColorClassifier.h"
#include "modules/SimpleBallColorClassifier.h"
#include "modules/ScanLineEdgelDetectorDifferential.h"
#include "modules/MaximumRedBallDetector.h"
#include "modules/GradientGoalDetector.h"
#include "modules/NeoLineDetector.h"

#include <sstream>

BEGIN_DECLARE_MODULE(NeoVision)

END_DECLARE_MODULE(NeoVision)

class NeoVision: private NeoVisionBase, private ModuleManager
{
public:
  NeoVision();
  ~NeoVision(){}

  virtual void execute();

private:
  ModuleCreator<FieldColorClassifierFull>* theFieldColorClassifierFull;
  ModuleCreator<SimpleFieldColorClassifier>* theSimpleFieldColorClassifier;
  ModuleCreator<SimpleGoalColorClassifier>* theSimpleGoalColorClassifier;
  ModuleCreator<SimpleBallColorClassifier>* theSimpleBallColorClassifier;
  ModuleCreator<ScanLineEdgelDetectorDifferential>* theScanLineEdgelDetectorDifferential;
  ModuleCreator<NeoLineDetector>* theNeoLineDetector;
  ModuleCreator<MaximumRedBallDetector>* theMaximumRedBallDetector;
  ModuleCreator<GradientGoalDetector>* theGradientGoalDetector;

};//end class NeoVision

#endif // _NeoVision_H_
