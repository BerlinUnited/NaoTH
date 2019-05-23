/**
* @file WhistleDetectorV2.cpp
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* Implementation of class WhistleDetectorV2
* Implements a whistle detector based on a frequency band filter.
* (inspired by the austrian kangaroos whistle detector)
*/

#include "WhistleDetectorV2.h"


WhistleDetectorV2::WhistleDetectorV2()
  : 
  lastDataTimestamp(0), 
  whistle_filter(0.3, 0.1)
{
  DEBUG_REQUEST_REGISTER("Plot:WhistleDetectorV2:scan_thresh","", false);

  getDebugParameterList().add(&params);
}

WhistleDetectorV2::~WhistleDetectorV2()
{
  getDebugParameterList().remove(&params);
}

void WhistleDetectorV2::execute()
{
  getWhistlePercept().reset();

  // there is no new data, return
  if(lastDataTimestamp >= getAudioData().timestamp ) {
    return;
  }

  // save the timestamp for later
  lastDataTimestamp = getAudioData().timestamp;

  // update parameters if necessary
  if(params.check_changed()) {
    fft.init(params.nWindowSize + params.nWindowZeroPadding);
    whistle_filter.setParameter(params.whistle_filter.g0, params.whistle_filter.g1);
  }

  // NOTE: this is channel offset, i.e., the number of the channel that we are listening to
  const int channel = 0;

  const int length = static_cast<int>(getAudioData().samples.size());
  const int channels = getAudioData().numChannels;
  const std::vector<short>& data = getAudioData().samples;

  const int windowTime = params.nWindowSize;
  const int windowTimeStep = params.nWindowSkipping;
  const int whistleBeginIdx  = int(params.fWhistleBegin * fft.getWindowFrequency()) / params.fSampleRate;
  const int whistleEndIdx    = int(params.fWhistleEnd  * fft.getWindowFrequency()) / params.fSampleRate;
  
  // make sure the bin indices stay within bounds
  // ASSERT(nWhistleBegin <= nWhistleEnd && nWhistleEnd*2 <= params.fSampleRate);
  ASSERT(whistleBeginIdx <= whistleEndIdx && whistleEndIdx <= fft.getWindowFrequency());

  // for debug
  int plot_idx = 0;

  // iterate over the data
  for (int i = 0; i < length; i += channels)
  {
    // copy to the input data
    fft.add(data[i + channel]);

    // enough data is in te input buffer
    if(fft.size() >= windowTime)
    {
      fft.execute();
      
      // find the maximal magnitude within the frequency band of the whistle
      double max_value = -1;
      for(int k = whistleBeginIdx; k < whistleEndIdx; ++k) {
        if(fft.magnitude(k) > max_value) {
          max_value = fft.magnitude(k);
        }
      }

      // estimate the dynamic threshold
      double mean, dev;
      calcMeanDeviationApprox(fft.magnitude(), mean, dev);
      const double whistleThresh = mean + params.vWhistleThreshold * dev;

      bool found = false;
      if(max_value > whistleThresh) {
        found = true;
      }

      DEBUG_REQUEST("Plot:WhistleDetectorV2:scan_thresh",
        PLOT_GENERIC("WhistleDetectorV2:scan", plot_idx, max_value);
        PLOT_GENERIC("WhistleDetectorV2:thresh", plot_idx, whistleThresh);
        plot_idx++;
      );

      if( whistle_filter.update(found, 0.3, 0.9) ) {
        getWhistlePercept().whistleDetected = true;
      }

      PLOT("WhistleDetectorV2:whistle_filter", whistle_filter.value());

      // clear buffer:
      fft.clean(windowTimeStep);
    }
  }
}
