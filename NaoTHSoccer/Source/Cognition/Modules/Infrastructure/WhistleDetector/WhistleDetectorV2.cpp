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
//#include <

#include <vector>


WhistleDetectorV2::WhistleDetectorV2()
: lastDataTimestamp(0)
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
	runFrequencyExtraction();

	//Dummy print out
	//std::cout << "Whistledetector Result: " << result << std::endl;
}

int WhistleDetectorV2::runFrequencyExtraction()
{
	/* load window times */
	//TODO dont load it every execution cycle
	int nWhistleBegin = int(params.fWhistleBegin * params.nWindowSizePadded) / params.fSampleRate;
	int nWhistleEnd = int(params.fWhistleEnd  * params.nWindowSizePadded) / params.fSampleRate;

	//check that configs make sense
	//TODO use asserts here
	if (params.fWhistleBegin < 0) {
		std::cerr << "Whistle begin is below zero!" << std::endl;
		return -1;
	}
	if (params.fWhistleEnd   < 0) {
		std::cerr << "Whistle end is below zero!" << std::endl;
		return -1;
	}
	if (params.fWhistleBegin >(params.fSampleRate / 2)) {
		std::cerr << "Whistle begin is above Nyquist frequency!" << std::endl;
		return -1;
	}
	if (params.fWhistleEnd   > (params.fSampleRate / 2)) {
		std::cerr << "Whistle end is above Nyquist frequency!" << std::endl;
		return -1;
	}
	if (params.fWhistleBegin > params.fWhistleEnd) {
		std::cerr << "Whistle begin is above Whistle end!" << std::endl;
		return -1;
	}

  // no new data, return
  if(lastDataTimestamp >= getAudioData().timestamp ) {
    lastDataTimestamp = getAudioData().timestamp;
    return 0;
  }

	//This part corresponds to executeAction function in original implementation
	static MySpectrumHandler* spectrumHandler = new MySpectrumHandler(params.vWhistleThreshold, nWhistleBegin, nWhistleEnd, params.nWhistleMissFrames, params.nWhistleOkayFrames);

  PLOT("WhistleDetectorV2:whistleCounter", spectrumHandler->whistleCounterGlobal);
  PLOT("WhistleDetectorV2:whistleMissCounter", spectrumHandler->whistleMissCounterGlobal);
  spectrumHandler->whistleCounterGlobal = 0;
  spectrumHandler->whistleMissCounterGlobal = 0;

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
	stft.newData(&(getAudioData().samples[0]), getAudioData().samples.size() - 256*getAudioData().numChannels, (short)getAudioData().numChannels);

  // copy
  lastDataTimestamp = getAudioData().timestamp;
	return 0;
}
