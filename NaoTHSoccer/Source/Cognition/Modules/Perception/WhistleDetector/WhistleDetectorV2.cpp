/**
* @file WhistleDetectorV2.cpp
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class WhistleDetectorV2
* Implements the austrian kangaroos version of the whistle detector
*/

#include "WhistleDetectorV2.h"

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
	
  // there is no new data, return
  if(lastDataTimestamp >= getAudioData().timestamp ) {
    return;
  }

  // save the timestamp for later
  lastDataTimestamp = getAudioData().timestamp;

  fft.init(params.nWindowSizePadded);
  
  whistle_filter.setParameter(params.whistle_filter.g0, params.whistle_filter.g1);

  static int iInput = 0;
  const int length = static_cast<int>(getAudioData().samples.size());// - 256*getAudioData().numChannels; // due to a bug in overlay
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
