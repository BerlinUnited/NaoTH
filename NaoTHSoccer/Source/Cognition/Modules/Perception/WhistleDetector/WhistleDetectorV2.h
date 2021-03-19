/**
* @file WhistleDetectorV2.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* Implementation of class WhistleDetectorV2
* Implements a whistle detector based on a frequency band filter.
* (simmilar to the austrian kangaroos whistle detector)
*/

#ifndef WhistleDetectorV2_H
#define WhistleDetectorV2_H

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
      // frequency band to look for the whistle
      PARAMETER_REGISTER(fWhistleBegin) = 2000;
      PARAMETER_REGISTER(fWhistleEnd)   = 3500;

      // TODO dont use this as a parameter here - samplerate should be defined somewhere else
      // the highest frequency possible: 8000 / 2 = 4000 => wavelength: 1000 / 4000 = 1/4ms => 4 waves/ms
      PARAMETER_REGISTER(fSampleRate) = 8000;

      PARAMETER_REGISTER(nWindowSize) = 160; // windowTime: 160 / (8000 / 1000) = 20ms => we consider a window of 20ms => we can have at maximum 4*20 = 80 waves of the highest frequency
      // NOTE: zero-padding is used to get a smoother spectrum
      // http://www.bitweenie.com/listings/fft-zero-padding/
      // https://dspillustrations.com/pages/posts/misc/spectral-leakage-zero-padding-and-frequency-resolution.html
      PARAMETER_REGISTER(nWindowZeroPadding) = 40; // windowFrequency: nWindowSize + nWindowZeroPadding
      PARAMETER_REGISTER(nWindowSkipping) = 80; // windowTimeStep: 160 / 2 = 80 => we skip half of the window size (10ms)
      
      PARAMETER_REGISTER(vWhistleThreshold) = 3.5;
      PARAMETER_REGISTER(vWhistleThresholdAmplitude) = 5;

      PARAMETER_REGISTER(whistle_filter.g0) = 0.1;
      PARAMETER_REGISTER(whistle_filter.g1) = 0.1;

      syncWithConfig();
    }
    double fWhistleBegin;
    double fWhistleEnd;
    int fSampleRate;
    int nWindowSize;
    int nWindowZeroPadding;
    int nWindowSkipping;

    double vWhistleThreshold;

    // TODO: make this threshold relative to the overall noise of the environment
    double vWhistleThresholdAmplitude;

    struct {
      double g0;
      double g1;
    } whistle_filter;
  } params;

private:

  unsigned int lastDataTimestamp;
  AssymetricalBoolHysteresisFilter whistle_filter;

  void calcMeanDeviationApprox(const std::vector<double>& data, double &mean, double &dev) {
      mean = dev = 0;
      for(size_t i = 0; i < data.size(); ++i) {
          mean    += data[i];
          dev     += data[i] * data[i];
      }

      dev = std::sqrt(static_cast<double>(data.size()) * dev - mean * mean) / static_cast<double>(data.size());
      mean /= static_cast<double>(data.size());
  }

  //
  class FFT
  {
  public:
    FFT() :
      windowFrequency(0),
      windowFrequencyHalf(0),
      input(NULL),
      output(NULL),
      plan(NULL)
    {}

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

    void init(const int windowFrequency) 
    {
      // already initialized
      if(windowFrequency == this->windowFrequency) {
        return;
      }
      
      this->windowFrequency = windowFrequency;
      this->windowFrequencyHalf = windowFrequency / 2 + 1;
      
      magnitude_vector.resize(windowFrequencyHalf);

      input   = static_cast<double*>(fftw_malloc(sizeof(double) * windowFrequency));
      output  = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * windowFrequencyHalf));

      // init the input buffer with zeros
      std::fill_n(input, windowFrequency, 0.0);

      plan = fftw_plan_dft_r2c_1d(windowFrequency, input, output, FFTW_MEASURE);
    }

    // normalize the samples
    void add(const int16_t& v) {
      input[iInput++] = static_cast<double>(v) / (static_cast<double>(std::numeric_limits<int16_t>::max()) + 1.0);
    }

    int size() const {
      return iInput;
    }

    // clean the fist n elements of the buffer
    // move the remaining part to the beginning
    void clean(int n) {
      const int s = size();
      iInput = 0;
      for(int k = n; k < s; ++k) {
        input[iInput++] = input[k];
      }
    }

    inline const std::vector<double>& magnitude() const {
      return magnitude_vector;
    }

    inline const double& magnitude(int i) const {
      return magnitude_vector[i];
    }

    int getWindowFrequency() const {
      return windowFrequency;
    }

  private:
    int windowFrequency;
    int windowFrequencyHalf;

    // index for the FFT input-buffer
    int iInput = 0;
    double *input;

    fftw_complex *output;
    fftw_plan plan;

    std::vector<double> magnitude_vector;
  };

  FFT fft;

};
#endif // WhistleDetectorV2_H
