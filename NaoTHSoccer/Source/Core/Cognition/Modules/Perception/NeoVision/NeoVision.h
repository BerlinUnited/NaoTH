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
#include "modules/SimpleFieldColorClassifier.h"
#include "modules/ScanLineEdgelDetectorDifferential.h"

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
  ModuleCreator<SimpleFieldColorClassifier>* theSimpleFieldColorClassifier;
  ModuleCreator<ScanLineEdgelDetectorDifferential>* theScanLineEdgelDetectorDifferential;

};//end class NeoVision

#endif // _NeoVision_H_
