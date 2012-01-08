#ifndef OPENCVDEBUG_H
#define OPENCVDEBUG_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Perception/OpenCVGrayScale.h>

BEGIN_DECLARE_MODULE(OpenCVDebug)
  REQUIRE(OpenCVGrayScale)
  PROVIDE(Image)
END_DECLARE_MODULE(OpenCVDebug)

class OpenCVDebug : public OpenCVDebugBase
{
public:
    OpenCVDebug();
    virtual void execute();
};

#endif // OPENCVDEBUG_H
