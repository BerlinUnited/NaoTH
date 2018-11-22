/**
* @file WhistleDetectorV2.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class WhistleDetectorV2
* Implements the austrian kangaroos version of the whistle detector
*/

#include "WhistleDetectorV2.h"
//#include "STFT.h"
#include "myspectrumhandler.h"

#include <vector>


WhistleDetectorV2::WhistleDetectorV2()
  : 
    lastDataTimestamp(0), 
    whistle_filter(0.3, 0.1)
  //myfile ("example.bin", std::ios::binary)
{
	getDebugParameterList().add(&params);
}

WhistleDetectorV2::~WhistleDetectorV2()
{
	getDebugParameterList().remove(&params);
  //myfile.close();
}

void WhistleDetectorV2::execute()
{
	
  // new data, run setection
  if(lastDataTimestamp < getAudioData().timestamp ) 
  {
    //runDetection();
    runDetectionNew();
  }

  // copy
  lastDataTimestamp = getAudioData().timestamp;
}


void WhistleDetectorV2::runDetection()
{
  /* load window times */
	//TODO dont load it every execution cycle
	int nWhistleBegin = int(params.fWhistleBegin * params.nWindowSizePadded) / params.fSampleRate;
	int nWhistleEnd = int(params.fWhistleEnd  * params.nWindowSizePadded) / params.fSampleRate;

	//check that configs make sense
	//TODO use asserts here
	if (params.fWhistleBegin < 0) {
		std::cerr << "Whistle begin is below zero!" << std::endl;
		return;
	}
	if (params.fWhistleEnd   < 0) {
		std::cerr << "Whistle end is below zero!" << std::endl;
		return;
	}
	if (params.fWhistleBegin >(params.fSampleRate / 2)) {
		std::cerr << "Whistle begin is above Nyquist frequency!" << std::endl;
		return;
	}
	if (params.fWhistleEnd   > (params.fSampleRate / 2)) {
		std::cerr << "Whistle end is above Nyquist frequency!" << std::endl;
		return;
	}
	if (params.fWhistleBegin > params.fWhistleEnd) {
		std::cerr << "Whistle begin is above Whistle end!" << std::endl;
		return;
	}

  //This part corresponds to executeAction function in original implementation
	static MySpectrumHandler* spectrumHandler = new MySpectrumHandler(params.vWhistleThreshold, nWhistleBegin, nWhistleEnd, params.nWhistleMissFrames, params.nWhistleOkayFrames);

  /*
  for (size_t i = 0; i < getAudioData().samples.size() - 256*2; i += getAudioData().numChannels) {
    myfile.write((char*)&getAudioData().samples[i],sizeof(short));
  }
  */

	// create short time fourier transform
	static STFT stft(0, params.nWindowSize, params.nWindowSkipping, params.nWindowSizePadded, spectrumHandler);

	//std::vector<short> samples = getAudioData().samples;
	//HACK the stft class expectes audiobuffer to be a c array of type int16_t
	//int16_t* audiobuffer = &samples[0];

	//stft.newData(audiobuffer, BUFFER_SIZE_RX, NUM_CHANNELS_RX);
  // NOTE: SAMPLE_OVERLAP_COUNT = 256
	stft.newData(&(getAudioData().samples[0]), getAudioData().samples.size() /*- 256*getAudioData().numChannels*/, (short)getAudioData().numChannels);


  PLOT("WhistleDetectorV2:whistleCounter", spectrumHandler->whistleCounterGlobal);
  PLOT("WhistleDetectorV2:whistleMissCounter", spectrumHandler->whistleMissCounterGlobal);
  PLOT("WhistleDetectorV2:filter_value", spectrumHandler->filter_value);
  spectrumHandler->whistleCounterGlobal = 0;
  spectrumHandler->whistleMissCounterGlobal = 0;
  spectrumHandler->filter_value = 0.0;

  for(size_t x = 0; x < spectrumHandler->scan_log.size(); ++x) {
    PLOT_GENERIC("WhistleDetectorV2:scan", x, spectrumHandler->scan_log[x]);
    PLOT_GENERIC("WhistleDetectorV2:thresh", x, spectrumHandler->scan_thc[x]);
  }
  spectrumHandler->scan_log.clear();
  spectrumHandler->scan_thc.clear();

  // update the percept
  getWhistlePercept().counter += spectrumHandler->whistleDetections;
  spectrumHandler->whistleDetections = 0;
}



void WhistleDetectorV2::runDetectionNew()
{
  static FFT fft (params.nWindowSizePadded);
  
  whistle_filter.setParameter(params.whistle_filter.g0, params.whistle_filter.g1);

  static int iInput = 0;
  const int length = getAudioData().samples.size();// - 256*getAudioData().numChannels; // due to a bug in overlay
  const int channels = getAudioData().numChannels;
  const int offset = 0;
  const int windowTime = params.nWindowSize;
  const int windowTimeStep = params.nWindowSkipping;
  const std::vector<short>& data = getAudioData().samples;

  const int nWhistleBegin = int(params.fWhistleBegin * params.nWindowSizePadded) / params.fSampleRate;
	const int nWhistleEnd = int(params.fWhistleEnd  * params.nWindowSizePadded) / params.fSampleRate;

  int plot_idx = 0;

  // iterate over the data
  for (int i = 0; i < length; i += channels) 
  {
    // copy to the input data
    intToNormalizedDouble(data[i + offset], fft.input[iInput++]);

    // enough data is in te input buffer
    if(iInput >= windowTime) 
    {
      fft.execute();
      
      double mean, dev;
      calcMeanDeviationApprox(fft.magnitude(), mean, dev);
      const double whistleThresh = mean + params.vWhistleThreshold * dev;
      
      bool found = false;
      double max_value = -1;
      for(int k = nWhistleBegin; k < nWhistleEnd; ++k) {
        if(fft.magnitude()[k] > max_value) {
          max_value = fft.magnitude()[k];
        }
        if(fft.magnitude()[k] > whistleThresh) {
          found = true;
          break;
        }
      }

      PLOT_GENERIC("WhistleDetectorV2:scan", plot_idx, max_value);
      PLOT_GENERIC("WhistleDetectorV2:thresh", plot_idx, whistleThresh);
      plot_idx++;

      if( whistle_filter.update(found, 0.3, 0.9) ) {
        getWhistlePercept().counter++;
      }

      PLOT("WhistleDetectorV2:whistle_filter", whistle_filter.value());

      // move the overlapping part to the beginning of the input
      iInput = 0;
      for(int k = windowTimeStep; k < windowTime; ++k) {
        fft.input[iInput++] = fft.input[k];
      }
    }
  }

}