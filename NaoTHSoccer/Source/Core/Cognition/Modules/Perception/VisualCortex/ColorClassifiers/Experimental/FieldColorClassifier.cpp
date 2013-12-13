/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 *
 * Created on 15. march 2011, 15:56
 */

#include "FieldColorClassifier.h"



// Debug


FieldColorClassifier::FieldColorClassifier()
:
  justStarted(true),
  fieldCalibRect("LowerField", ColorClasses::green, 0, 120, 319, 238),
  fieldColorCalibrator("Field", ColorClasses::green),
  sampleCount(0),
  maxSampleCount(20)
{
  fieldColorCalibrator.addCalibrationRect(fieldCalibRect);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:calibrate", " ", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:set_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:reset", " ", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:FieldColorClassifier:enable_plot", " ", false);

  GT_TRACE("before FieldColorClassifier constructor memset");
  memset(&weightedHistV, 0, sizeof(weightedHistV));
  GT_TRACE("after FieldColorClassifier constructor memset");
}

void FieldColorClassifier::execute()
{
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:calibrate",
    calibrate();
  );

  classify();
}

void FieldColorClassifier::classify()
{
  getFieldColorPercept().maxY = fieldParams.MaxBrightnessChannelValue;
  getFieldColorPercept().distV = fieldParams.CromaRedChannelDistance;

  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:reset",
    fieldColorCalibrator.reset();
    justStarted = true;
    sampleCount = 0;
  );

  if(sampleCount >= maxSampleCount)
  {
    justStarted = false;
  }

  if(justStarted)
  {
    sampleCount++;
    getFieldColorPercept().indexV = indexV;
  }


  if(!getColorChannelHistograms().colorChannelIsUptodate)
  {
    return;
  }
 
  // init values
  maxWeightedV = 0.0;
  indexV = 0;

  double sumU = 0.0;
  double meanU = 0.0;

  //double p = 1.0 / getColoredGrid().uniformGrid.numberOfGridPoints;

  STOPWATCH_START("FieldColorClassifier:Cr_filtering");
  for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
  {
    double mCr = max<int>(0,  160 - i);
    double wCr = mCr / 160.0;
    
    weightedHistV[i] = getColorChannelHistograms().histogramV.rawData[i];
    //histogram for blue => needed for later yellow and blue estimation
    histU[i] = getColorChannelHistograms().histogramU.rawData[i] * wCr;
    sumU += histU[i];

    weightedHistV[i] *= wCr;

    // remember the maximal value
    if(weightedHistV[i] > maxWeightedV)
    {
      maxWeightedV = weightedHistV[i];
      indexV = i;
    }
  }//end for
  STOPWATCH_STOP("FieldColorClassifier:Cr_filtering");

  if(sumU == 0.0)
    return;

  for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
  {
    histNormU[i] = histU[i] / sumU;
    meanU += i * histNormU[i];
  }

  double z2U = 0.0;

  for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
  {
    double t = (meanU - histU[i]) * histNormU[i];
    z2U += t * t;
  }

  double stdDevU = sqrt(z2U);

  getFieldColorPercept().minU = (int) Math::clamp(meanU - stdDevU  * 0.5, 0.0, meanU);
  getFieldColorPercept().maxU = (int) Math::clamp(meanU + stdDevU , meanU, (double) fieldParams.MaxCromaBlueChannelValue);

  getFieldColorPercept().set();
  getFieldColorPercept().lastUpdated = getFrameInfo();
  runDebugRequests();
}

void FieldColorClassifier::calibrate()
{
  fieldColorCalibrator.execute(getImage()); 
  
  PixelT<int> ccIdx; 
  ccIdx.y = 0; 
  ccIdx.u = 0; 
  ccIdx.v = 0;
  PixelT<int> ccDist; 
  ccDist.y = 0; 
  ccDist.u = 0; 
  ccDist.v = 0;
  fieldColorCalibrator.get(ccIdx, ccDist);
  fieldParams.MaxBrightnessChannelValue = Math::clamp<int>(ccIdx.y + ccDist.y, 0, 255);
  fieldParams.MaxCromaBlueChannelValue = Math::clamp<int>(ccIdx.u + ccDist.u, 0, 255);
  fieldParams.CromaRedChannelDistance = ccDist.v;
  fieldParams.saveToConfig();
  fieldParams.syncWithConfig();
}

void FieldColorClassifier::runDebugRequests()
{
  DEBUG_REQUEST("Vision:ColorClassifiers:FieldColorClassifier:set_in_image",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(getFieldColorPercept().isFieldColor(pixel))
        {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );
}


