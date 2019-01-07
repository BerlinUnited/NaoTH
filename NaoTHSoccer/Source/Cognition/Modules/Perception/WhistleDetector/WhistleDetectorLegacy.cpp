/**
* @file WhistleDetectorLegacy.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class WhistleDetectorLegacy
* Implements the austrian kangaroos version of the whistle detector
*/

#include "WhistleDetectorLegacy.h"
//#include "STFT.h"
#include "myspectrumhandler.h"

#include <vector>


WhistleDetectorLegacy::WhistleDetectorLegacy()
  : lastDataTimestamp(0)
{
  DEBUG_REQUEST_REGISTER("Plot:WhistleDetectorLegacy:scan_thresh","", false);

	getDebugParameterList().add(&params);
}

WhistleDetectorLegacy::~WhistleDetectorLegacy()
{
	getDebugParameterList().remove(&params);
}

void WhistleDetectorLegacy::execute()
{
	getWhistlePercept().reset();

  // there is no new data, return
  if(lastDataTimestamp >= getAudioData().timestamp ) {
    return;
  }

  // save the timestamp for later
  lastDataTimestamp = getAudioData().timestamp;


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

	// create short time fourier transform
	static STFT stft(0, params.nWindowSize, params.nWindowSkipping, params.nWindowSizePadded, spectrumHandler);

	//stft.newData(audiobuffer, BUFFER_SIZE_RX, NUM_CHANNELS_RX);
  // NOTE: SAMPLE_OVERLAP_COUNT = 256
  stft.newData(&(getAudioData().samples[0]), static_cast<int>(getAudioData().samples.size()) /*- 256*getAudioData().numChannels*/, (short)getAudioData().numChannels);


  PLOT("WhistleDetectorLegacy:whistleCounter", spectrumHandler->whistleCounterGlobal);
  PLOT("WhistleDetectorLegacy:whistleMissCounter", spectrumHandler->whistleMissCounterGlobal);
  PLOT("WhistleDetectorLegacy:filter_value", spectrumHandler->filter_value);
  spectrumHandler->whistleCounterGlobal = 0;
  spectrumHandler->whistleMissCounterGlobal = 0;
  spectrumHandler->filter_value = 0.0;

  DEBUG_REQUEST("Plot:WhistleDetectorLegacy:scan_thresh",
    for(size_t x = 0; x < spectrumHandler->scan_log.size(); ++x) {
      PLOT_GENERIC("WhistleDetectorLegacy:scan",   static_cast<int>(x), spectrumHandler->scan_log[x]);
      PLOT_GENERIC("WhistleDetectorLegacy:thresh", static_cast<int>(x), spectrumHandler->scan_thc[x]);
    }
  );
  spectrumHandler->scan_log.clear();
  spectrumHandler->scan_thc.clear();

  PLOT("WhistleDetectorLegacy:whistle_filter", (spectrumHandler->whistleDetections > 0) );

  // update the percept
  if(spectrumHandler->whistleDetections > 0) {
    getWhistlePercept().whistleDetected = true;
  }
  spectrumHandler->whistleDetections = 0;
}
