/*
 * File:   FieldColorClassifierPreProcessor.cpp
 * Author: claas
 *
 */

#include "FieldColorClassifierPreProcessor.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

FieldColorClassifierPreProcessor::FieldColorClassifierPreProcessor()
:
  enablePlots(false)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPreProcessor:enable_plots", " ", false);
}


void FieldColorClassifierPreProcessor::execute()
{
  if(!getHistogram().colorChannelIsUptodate)
  {
    return;
  }
  enablePlots = false;
  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPreProcessor:enable_plots",
    enablePlots = true;
  );

  getFieldColorPreProcessingPercept().distCr = getFieldColorPercept().distCr;
  getFieldColorPreProcessingPercept().maxWeightedCr = 0.0;//0.85 * getFieldColorPercept().maxWeightedCr;
  getFieldColorPreProcessingPercept().maxWeightedIndexCr = 0;//(unsigned int) (getFieldColorPercept().maxWeightedIndexCr);

  STOPWATCH_START("FieldColorClassifier:PreProcessor_Cr_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCr = (double) max<int>(0,  128 - i);
    double wCr = mCr / 128.0;
    double wCrG = exp(-Math::sqr(0.0 - i)/(60.0*60.0));
    getFieldColorPreProcessingPercept().weightedHistCr[i] = (double) getHistogram().colorChannelHistogramField[2][i];
    if(enablePlots)
    {
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight",i, wCr);
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight_square",i, wCr * wCr);
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight_gauss",i, wCrG);
      PLOT_GENERIC("FCCPre_weightedHistCr_unfiltered",i, getFieldColorPreProcessingPercept().weightedHistCr[i]);
      PLOT_GENERIC("FCCPre_weightedHistCr_linear_filter",i, getFieldColorPreProcessingPercept().weightedHistCr[i] * wCr);
      PLOT_GENERIC("FCCPre_weightedHistCr_square_filter",i, getFieldColorPreProcessingPercept().weightedHistCr[i] * wCr * wCr);
      PLOT_GENERIC("FCCPre_weightedHistCr_gauss_filter",i, getFieldColorPreProcessingPercept().weightedHistCr[i] * wCrG);
    }
    getFieldColorPreProcessingPercept().weightedHistCr[i] *= wCr;
    if(enablePlots)
    {
      PLOT_GENERIC("FCCPre_weightedHistCr_filtered",i, getFieldColorPreProcessingPercept().weightedHistCr[i]);
    }
    if(getFieldColorPreProcessingPercept().weightedHistCr[i] > getFieldColorPreProcessingPercept().maxWeightedCr)
    {
      getFieldColorPreProcessingPercept().maxWeightedCr = getFieldColorPreProcessingPercept().weightedHistCr[i];
      getFieldColorPreProcessingPercept().maxWeightedIndexCr = i;
    }
  }
  getFieldColorPreProcessingPercept().setRegionBorder();
  STOPWATCH_STOP("FieldColorClassifier:PreProcessor_Cr_filtering");

  getFieldColorPreProcessingPercept().lastUpdated = getFrameInfo();
}

