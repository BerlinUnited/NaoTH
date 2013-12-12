/**
* @file ImageProcessor.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ImageProcessor
*/

#ifndef _ImageProcessor_H_
#define _ImageProcessor_H_

// infrastructure
#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// Representations
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/ArtificialHorizon.h"

#include <sstream>

BEGIN_DECLARE_MODULE(ImageProcessor)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
END_DECLARE_MODULE(ImageProcessor)

class ImageProcessor: private ImageProcessorBase, private ModuleManager
{
public:
  ImageProcessor();
  ~ImageProcessor(){}

  virtual void execute();

private:
};//end class ImageProcessor

#endif // _ImageProcessor_H_
