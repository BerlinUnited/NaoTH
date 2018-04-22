#ifndef _AdaptiveAutoExposure_h_
#define _AdaptiveAutoExposure_h_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include "Tools/DoubleCamHelpers.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(AdaptiveAutoExposure)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)

    REQUIRE(Image)
    REQUIRE(ImageTop)

    PROVIDE(CameraSettingsRequest)
    PROVIDE(CameraSettingsRequestTop)
// TODO: add REQUIRE/PROVIDE definitions here
END_DECLARE_MODULE(AdaptiveAutoExposure)

class AdaptiveAutoExposure : public AdaptiveAutoExposureBase {
public:
    AdaptiveAutoExposure();
    virtual ~AdaptiveAutoExposure() {};

    virtual void execute();
private: 

    void execute(CameraInfo::CameraID id);
private:
    CameraInfo::CameraID cameraID;

    DOUBLE_CAM_PROVIDE(AdaptiveAutoExposure, CameraSettingsRequest);
};

#endif // _AdaptiveAutoExposure_h_