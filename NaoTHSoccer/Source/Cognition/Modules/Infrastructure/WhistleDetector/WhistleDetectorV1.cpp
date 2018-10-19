/**
* @file WhistleDetectorV1.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class WhistleDetectorV1
* Implements a basic Peak Detection as whistle detector. This is only to show how a whistle detection fits in the overall infrastructure
*/

#include "WhistleDetectorV1.h"
#include <vector>
#include <stdlib.h>
#include <numeric>

WhistleDetectorV1::WhistleDetectorV1(){
	getWhistlePercept().counter = 0;

	getDebugParameterList().add(&params);
}

WhistleDetectorV1::~WhistleDetectorV1()
{
	getDebugParameterList().remove(&params);
}

void WhistleDetectorV1::execute(){
	std::vector<short> samples = getAudioData().samples;

	int sum = std::accumulate(samples.begin(), samples.end(), 0);		
	if (sum > params.threshold){
		std::cout << "Whistle Detected with sample sum: " << sum << std::endl;
		getWhistlePercept().counter++;
	}
}