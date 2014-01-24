#ifndef OPENCVIMAGEPROVIDER_H
#define OPENCVIMAGEPROVIDER_H


#include <ModuleFramework/Module.h>
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/CVImage.h"

BEGIN_DECLARE_MODULE(OpenCVImageProvider)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  PROVIDE(CVImage)
  PROVIDE(CVImageTop)
END_DECLARE_MODULE(OpenCVImageProvider)

class OpenCVImageProvider : public OpenCVImageProviderBase
{
public:
  OpenCVImageProvider();
  virtual ~OpenCVImageProvider(){}

  virtual void execute();
};

#endif // OPENCVIMAGEPROVIDER_H
