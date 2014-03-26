/**
* @file MonteCarloSelfLocatorSimple.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MonteCarloSelfLocatorSimple
*/

#include "MonteCarloSelfLocatorSimple.h"

// debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugRequest.h"

// tools
#include "Tools/Math/Probabilistics.h"

using namespace std;

MonteCarloSelfLocatorSimple::MonteCarloSelfLocatorSimple() 
  :
    canopyClustering(parameters.thresholdCanopy)
{

  //resetSampleSet(theSampleSet);

  // debug
  DEBUG_REQUEST_REGISTER("MCSLS:reset_samples", "reset the sample set", false);

  // fiald drawings
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Samples", "draw sample set before resampling", false);
}


void MonteCarloSelfLocatorSimple::execute()
{


}//end execute