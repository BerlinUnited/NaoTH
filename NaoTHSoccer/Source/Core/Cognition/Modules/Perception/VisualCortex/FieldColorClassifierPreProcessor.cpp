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
  const Histogram& histogram = getHistogram();
  if(!histogram.colorChannelIsUptodate)
  {
    return;
  }
  enablePlots = false;
  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPreProcessor:enable_plots",
    enablePlots = true;
  );

  FieldColorPreProcessingPercept& fPre = getFieldColorPreProcessingPercept();
  fPre.distCr = getFieldColorPercept().distCr;
  fPre.maxWeightedCr = 0.0;//0.85 * getFieldColorPercept().maxWeightedCr;
  fPre.maxWeightedIndexCr = 0;//(unsigned int) (getFieldColorPercept().maxWeightedIndexCr);

  STOPWATCH_START("FieldColorClassifier:PreProcessor_Cr_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCr = (double) max<int>(0,  128 - i);
    double wCr = mCr / 128.0;
    double wCrG = exp(-Math::sqr(0.0 - i)/(60.0*60.0));
    fPre.weightedHistCr[i] = (double) histogram.colorChannelHistogramField[2][i];
    if(enablePlots)
    {
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight",i, wCr);
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight_square",i, wCr * wCr);
      //PLOT_GENERIC("FCCPre_weightedHistCr_weight_gauss",i, wCrG);
      PLOT_GENERIC("FCCPre_weightedHistCr_unfiltered",i, fPre.weightedHistCr[i]);
      PLOT_GENERIC("FCCPre_weightedHistCr_linear_filter",i, fPre.weightedHistCr[i] * wCr);
      PLOT_GENERIC("FCCPre_weightedHistCr_square_filter",i, fPre.weightedHistCr[i] * wCr * wCr);
      PLOT_GENERIC("FCCPre_weightedHistCr_gauss_filter",i, fPre.weightedHistCr[i] * wCrG);
    }
    fPre.weightedHistCr[i] *= wCr;
    if(enablePlots)
    {
      PLOT_GENERIC("FCCPre_weightedHistCr_filtered",i, fPre.weightedHistCr[i]);
    }
    if(fPre.weightedHistCr[i] > fPre.maxWeightedCr)
    {
      fPre.maxWeightedCr = fPre.weightedHistCr[i];
      fPre.maxWeightedIndexCr = i;
    }    
  }
  STOPWATCH_STOP("FieldColorClassifier:PreProcessor_Cr_filtering");

  fPre.lastUpdated = getFrameInfo();
}

