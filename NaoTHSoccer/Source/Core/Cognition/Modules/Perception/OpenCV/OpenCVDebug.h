#ifndef OPENCVDEBUG_H
#define OPENCVDEBUG_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Perception/CVImage.h>

BEGIN_DECLARE_MODULE(OpenCVDebug)
  REQUIRE(CVImage)
  REQUIRE(CVImageTop)
  PROVIDE(Image)
  PROVIDE(ImageTop)
END_DECLARE_MODULE(OpenCVDebug)

class OpenCVDebug : public OpenCVDebugBase
{
public:
    OpenCVDebug();
    virtual void execute();

    virtual ~OpenCVDebug() {}
private:
  void writeToImage(cv::Mat result, Image &theImage);
};

#endif // OPENCVDEBUG_H
