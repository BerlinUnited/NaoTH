/**
* @file WhistleDetectorTemplateMatching.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* Implements a whistle detector based on template matching.
*/

#ifndef _WhistleDetectorTemplateMatching_H
#define _WhistleDetectorTemplateMatching_H

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
//#include <fstream>
#include <vector>
#include <fftw3/fftw3.h>
#include <Tools/Filters/AssymetricalBoolFilter.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorTemplateMatching)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(AudioData)

  PROVIDE(WhistlePercept)
END_DECLARE_MODULE(WhistleDetectorTemplateMatching)

class WhistleDetectorTemplateMatching : public WhistleDetectorTemplateMatchingBase
{
public:
	WhistleDetectorTemplateMatching();
	virtual ~WhistleDetectorTemplateMatching();

	virtual void execute();

	class Parameters : public ParameterList
	{
	public:
		Parameters() : ParameterList("WhistleDetectorTemplateMatching")
		{
      PARAMETER_REGISTER(threshold) = 0.25;

			syncWithConfig();
		}
		double fWhistleBegin;
		double fWhistleEnd;
		int fSampleRate;
		int nWindowSize;
		int nWindowSizePadded;
		int nWindowSkipping;
		double vWhistleThreshold;

    double threshold;

	} params;

private:

  unsigned int lastDataTimestamp;

  void intToNormalizedDouble(const int16_t &in, double &out) {
      out = static_cast<double>(in) / (static_cast<double>(std::numeric_limits<int16_t>::max()) + 1.0);
  }

  void loadReferenceWhistles();
  void detectWhistles();

  double* signal2correlateBuffer;
  double* correlatedBuffer;

  fftw_complex *fftOut;
  fftw_complex *fftIn;
  
  fftw_plan planFFT;
  fftw_plan planIFFT;

  std::vector<fftw_complex*> referenceWhistleSpectra;
  std::vector<double> referenceWhistleAutoCorrelationMaxima;
  std::vector<std::string> referenceWhistleNames;

};
#endif // _WhistleDetectorTemplateMatching_H
