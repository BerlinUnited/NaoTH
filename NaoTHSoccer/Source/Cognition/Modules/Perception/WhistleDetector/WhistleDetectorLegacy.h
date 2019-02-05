/**
* @file WhistleDetectorV2.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class WhistleDetectorLegacy
* Implements the austrian kangaroos version of the whistle detector
*/

#ifndef _WhistleDetectorLegacy_H
#define _WhistleDetectorLegacy_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/AudioData.h>
#include <Representations/Perception/WhistlePercept.h>

// debug
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugRequest.h>

// tools
#include <vector>

using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorLegacy)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(AudioData)

  PROVIDE(WhistlePercept)
END_DECLARE_MODULE(WhistleDetectorLegacy)

class WhistleDetectorLegacy : public WhistleDetectorLegacyBase
{
public:
	WhistleDetectorLegacy();
	virtual ~WhistleDetectorLegacy();

	virtual void execute();

	class Parameters : public ParameterList
	{
	public:
		Parameters() : ParameterList("WhistleDetectorLegacy")
		{
			PARAMETER_REGISTER(fWhistleBegin) = 2000;
			PARAMETER_REGISTER(fWhistleEnd) = 3500;
			PARAMETER_REGISTER(fSampleRate) = 8000; // TODO dont use this as a parameter here - samplerate should be defined somewhere else

			PARAMETER_REGISTER(nWindowSize) = 160; // windowTime
			PARAMETER_REGISTER(nWindowSizePadded) = 200; // windowFrequency
			PARAMETER_REGISTER(nWindowSkipping) = 80; // windowTimeStep

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
  unsigned int lastDataTimestamp;

};
#endif // _WhistleDetectorLegacy_H
