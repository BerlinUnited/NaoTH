#ifndef OPENCVGRAYSCALEIMAGEPROVIDER_H
#define OPENCVGRAYSCALEIMAGEPROVIDER_H

#include <ModuleFramework/Module.h>
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/CVImage.h"

BEGIN_DECLARE_MODULE(OpenCVGrayScaleImageProvider)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  PROVIDE(CVImage)
  PROVIDE(CVImageTop)
END_DECLARE_MODULE(OpenCVGrayScaleImageProvider)

class OpenCVGrayScaleImageProvider : public OpenCVGrayScaleImageProviderBase
{
public:
  OpenCVGrayScaleImageProvider();
  virtual ~OpenCVGrayScaleImageProvider() {}

  virtual void execute();
};

#endif // OPENCVGRAYSCALEIMAGEPROVIDER_H
