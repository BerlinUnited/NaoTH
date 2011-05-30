/**
* @file ImageProcessor.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ImageProcessor
*/

#ifndef __ImageProcessor_H_
#define __ImageProcessor_H_

// infrastructure
#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

//local Tools
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"

// submodules
#include "ObjectDetectors/BallDetector.h"

// Representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/ColorTable64.h"
//#include "Representations/Perception/BlobPercept.h"
//#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/CameraMatrix.h"


#include <sstream>

BEGIN_DECLARE_MODULE(ImageProcessor)
  REQUIRE(ColorTable64)
  REQUIRE(CameraMatrix)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
END_DECLARE_MODULE(ImageProcessor)

class ImageProcessor: private ImageProcessorBase
{
public:
  ImageProcessor();
  ~ImageProcessor(){};

  virtual void execute();

private:
  
};//end class ImageProcessor

#endif // __ImageProcessor_H_
