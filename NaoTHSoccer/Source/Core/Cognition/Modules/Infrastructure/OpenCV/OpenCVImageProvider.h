#ifndef OPENCVIMAGEPROVIDER_H
#define OPENCVIMAGEPROVIDER_H


#include <ModuleFramework/Module.h>
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/OpenCVGrayScale.h"

BEGIN_DECLARE_MODULE(OpenCVImageProvider)
  REQUIRE(Image)
  PROVIDE(OpenCVGrayScale)
END_DECLARE_MODULE(OpenCVImageProvider)

class OpenCVImageProvider : public OpenCVImageProviderBase
{
public:
  OpenCVImageProvider();

  virtual void execute();

  virtual ~OpenCVImageProvider();
private:

};

#endif // OPENCVIMAGEPROVIDER_H
