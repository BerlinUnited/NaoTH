/* 
 * File:   ImageCorrector.h
 * Author: claas
 *
 * Created on 22. November 2010, 19:37
 */

#ifndef IMAGECORRECTOR_H
#define	IMAGECORRECTOR_H

#include <string>

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "PlatformInterface/Platform.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Matrix_mxn.h"
#include "Tools/DataStructures/ParameterList.h"

// Debug
#include "Tools/Debug/DebugRequest.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ImageCorrector)
  PROVIDE(Image)
END_DECLARE_MODULE(ImageCorrector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////



class ImageCorrector  : private  ImageCorrectorBase
{
public:
  ImageCorrector();
  ~ImageCorrector();

  /** executes the module */
  void execute();
  void correctBrightnessSimple();

private:
  unsigned int correctionCycle;
  bool correctorIsRunning;
  bool simpleBrighnessCorrectionRequestActive;
};

#endif	/* IMAGECORRECTOR_H */

