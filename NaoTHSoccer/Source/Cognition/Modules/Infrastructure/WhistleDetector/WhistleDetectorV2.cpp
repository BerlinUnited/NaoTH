#include "WhistleDetectorV2.h"
#include <vector>

void WhistleDetectorV2::execute(){
	std::vector<short> samples = getAudioData().samples;
	int sum_of_elems = 0;
	for (auto& n : samples){
		sum_of_elems += n;
	}
		
	std::cout << "Samples Sum: " << sum_of_elems << std::endl;
}