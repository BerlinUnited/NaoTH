#ifndef _WHISTLEDETECTOR_V2_H
#define _WHISTLEDETECTOR_V2_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/AudioData.h>

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>


using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorV2)
REQUIRE(AudioData)

PROVIDE(DebugParameterList)
END_DECLARE_MODULE(WhistleDetectorV2)

class WhistleDetectorV2 : public WhistleDetectorV2Base
{
public:
	WhistleDetectorV2();
	virtual ~WhistleDetectorV2();

	virtual void execute();

	class Parameters : public ParameterList
	{
	public:
		Parameters() : ParameterList("WhistleDetectorV2")
		{
			PARAMETER_REGISTER(fWhistleBegin) = 2000;
			PARAMETER_REGISTER(fWhistleEnd) = 3500;
			PARAMETER_REGISTER(fSampleRate) = 8000; // TODO dont use this as a parameter here - samplerate should be defined somewhere else

			PARAMETER_REGISTER(nWindowSize) = 160;
			PARAMETER_REGISTER(nWindowSizePadded) = 200;
			PARAMETER_REGISTER(nWindowSkipping) = 80;

			PARAMETER_REGISTER(vWhistleThreshold) = 3.5;
			PARAMETER_REGISTER(nWhistleOkayFrames) = 30;
			PARAMETER_REGISTER(nWhistleMissFrames) = 7;

			syncWithConfig();
		}
		double fWhistleBegin;
		double fWhistleEnd;
		int fSampleRate;
		int nWindowSize;
		int nWindowSizePadded;
		int nWindowSkipping;
		double vWhistleThreshold;
		unsigned nWhistleOkayFrames;
		unsigned nWhistleMissFrames;
	} params;

private:
	int runFrequencyExtraction();
};
#endif // _WHISTLEDETECTOR_V2_H