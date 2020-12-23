#ifndef WHISTLEDETECTOR_V1_H
#define WHISTLEDETECTOR_V1_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/AudioData.h>
#include <Representations/Perception/WhistlePercept.h>
#include <Representations/Infrastructure/AudioControl.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorV1)
	REQUIRE(AudioData)
	REQUIRE(AudioControl)
	REQUIRE(FrameInfo)

	PROVIDE(WhistlePercept)
	PROVIDE(DebugParameterList)
END_DECLARE_MODULE(WhistleDetectorV1)

class WhistleDetectorV1 : public WhistleDetectorV1Base
{
public:
	WhistleDetectorV1();
	virtual ~WhistleDetectorV1();

	virtual void execute();

	class Parameters : public ParameterList
	{
	public:
		Parameters() : ParameterList("WhistleDetectorV1")
		{
			PARAMETER_REGISTER(threshold) = 60000;
			syncWithConfig();
		}
		double threshold;

	} params;

};
#endif // WHISTLEDETECTOR_V1_H