#ifndef RANSACLINEDETECTOR_H
#define RANSACLINEDETECTOR_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

BEGIN_DECLARE_MODULE(RansacLineDetector)

END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
 RansacLineDetector();
 ~RansacLineDetector();

 virtual void execute();
};

#endif // RANSACLINEDETECTOR_H
