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


WhistleDetectorV2::WhistleDetectorV2(){
	getDebugParameterList().add(&params);
}

WhistleDetectorV2::~WhistleDetectorV2()
{
	getDebugParameterList().remove(&params);
}

void WhistleDetectorV2::execute(){
	int result;
	result = runFrequencyExtraction();

	//Dummy print out
	std::cout << "Whistledetector Result: " << result << std::endl;
}

int WhistleDetectorV2::runFrequencyExtraction(){
	/* load window times */
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

	//This part corresponds to executeAction function in original implementation
	MySpectrumHandler* spectrumHandler = new MySpectrumHandler(params.vWhistleThreshold, nWhistleBegin, nWhistleEnd, params.nWhistleMissFrames, params.nWhistleOkayFrames);

	// create short time fourier transform
	STFT stft(0, params.nWindowSize, params.nWindowSkipping, params.nWindowSizePadded, spectrumHandler);

	std::vector<short> samples = getAudioData().samples;
	//HACK the stft class expectes audiobuffer to be a c array of type int16_t
	int16_t* audiobuffer = &samples[0];

	//stft.newData(audiobuffer, BUFFER_SIZE_RX, NUM_CHANNELS_RX);
	stft.newData(audiobuffer, 1024, 1);
	return 0;
}
