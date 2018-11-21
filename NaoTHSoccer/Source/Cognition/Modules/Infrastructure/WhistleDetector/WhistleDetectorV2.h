#ifndef _WHISTLEDETECTOR_V2_H
#define _WHISTLEDETECTOR_V2_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/AudioData.h>
#include <Representations/Infrastructure/WhistlePercept.h>

// debug
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugRequest.h>

// tools
#include <fstream>
#include <vector>
#include <fftw3/fftw3.h>
#include <Tools/Filters/AssymetricalBoolFilter.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(WhistleDetectorV2)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(AudioData)

  PROVIDE(WhistlePercept)
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

			PARAMETER_REGISTER(nWindowSize) = 160; // windowTime
			PARAMETER_REGISTER(nWindowSizePadded) = 200; // windowFrequency
			PARAMETER_REGISTER(nWindowSkipping) = 80; // windowTimeStep

			PARAMETER_REGISTER(vWhistleThreshold) = 3.5;
			PARAMETER_REGISTER(nWhistleOkayFrames) = 30;
			PARAMETER_REGISTER(nWhistleMissFrames) = 7;

      PARAMETER_REGISTER(whistle_filter.g0) = 0.1;
      PARAMETER_REGISTER(whistle_filter.g1) = 0.01;

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

    struct {
      double g0;
      double g1;
    } whistle_filter;
	} params;

private:
	void runDetection();
  void runDetectionNew();
  unsigned int lastDataTimestamp;
  AssymetricalBoolHysteresisFilter whistle_filter;
  std::ofstream myfile;

  void intToNormalizedDouble(const int16_t &in, double &out) {
      out = static_cast<double>(in) / (static_cast<double>(std::numeric_limits<int16_t>::max()) + 1.0);
  }

  void calcMeanDeviationApprox(const std::vector<double>& data, double &mean, double &dev) {
      mean = dev = 0;
      for(size_t i = 0; i < data.size(); ++i) {
          mean    += data[i];
          dev     += data[i] * data[i];
      }

      dev = std::sqrt(data.size() * dev - mean * mean) / static_cast<double>(data.size());
      mean /= static_cast<double>(data.size());
  }

  //
  class FFT
  {
  public:
    FFT() :
      windowFrequency(0),
      windowFrequencyHalf(0),
      output(NULL),
      plan(NULL),
      input(NULL)
    {}

    FFT(const int windowFrequency) 
      : 
      windowFrequency(windowFrequency),
      windowFrequencyHalf(windowFrequency / 2 + 1),
      magnitude_vector(windowFrequencyHalf)
    {
      input   = static_cast<double*>(fftw_malloc(sizeof(double) * windowFrequency));
      output  = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * windowFrequencyHalf));

      // init the input buffer with zeros
      std::fill_n(input, windowFrequency, 0.0);

      plan = fftw_plan_dft_r2c_1d(windowFrequency, input, output, FFTW_MEASURE);
    }

    ~FFT()
    {
      if(input) {
        fftw_free(input);
      }
      if(output) {
        fftw_free(output);
      }
      if(plan) {
        fftw_destroy_plan(plan);
      }
    }

    void execute() {
      fftw_execute(plan);

      for(int i = 0; i < windowFrequencyHalf; ++i) {
          magnitude_vector[i] = std::sqrt(output[i][0]*output[i][0] + output[i][1]*output[i][1]);
      }
    }

    int input_length() const {
      return windowFrequency;
    }

  private:
    int windowFrequency;
    int windowFrequencyHalf;

    fftw_complex *output;
    fftw_plan plan;

    std::vector<double> magnitude_vector;

  public:
    double *input;
    const std::vector<double>& magnitude() const {
      return magnitude_vector;
    }
  };

  FFT fft;

};
#endif // _WHISTLEDETECTOR_V2_H