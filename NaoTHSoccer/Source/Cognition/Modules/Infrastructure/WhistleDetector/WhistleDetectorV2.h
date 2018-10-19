#ifndef _WHISTLEDETECTOR_V2_H
#define _WHISTLEDETECTOR_V2_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/AudioData.h>
using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorV2)
REQUIRE(AudioData)
END_DECLARE_MODULE(WhistleDetectorV2)

class WhistleDetectorV2 : public WhistleDetectorV2Base
{
public:
	WhistleDetectorV2();
	virtual ~WhistleDetectorV2();

	virtual void execute();
private:
};
#endif // _WHISTLEDETECTOR_V2_H