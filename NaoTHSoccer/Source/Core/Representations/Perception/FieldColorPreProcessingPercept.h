/*
 * File:   FieldColorPercept.h
 * Author: claas
 *
 */

#ifndef FIELDCOLORPREPROCESSINGPERCEPT_H
#define  FIELDCOLORPREPROCESSINGPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Representations/Infrastructure/FrameInfo.h>

//TODO: move COLOR_CHANNEL_VALUE_COUNT to somewhere else
#include "Tools/ImageProcessing/Histogram.h"

using namespace naoth;

class FieldColorPreProcessingPercept : public naoth::Printable
{
public:
  double distCr;
  double maxWeightedCr;
  int maxWeightedIndexCr;
  int lowerBorderCr;
  int upperBorderCr;


  double weightedHistCr[COLOR_CHANNEL_VALUE_COUNT];

  FrameInfo lastUpdated;

  FieldColorPreProcessingPercept()
  {
    distCr = 0.0;
    maxWeightedCr = 0.0;
    maxWeightedIndexCr = 115;
    memset(&weightedHistCr, 0, sizeof(weightedHistCr));
  }

  ~FieldColorPreProcessingPercept()
  {}

  inline void setRegionBorder()
  {
    lowerBorderCr = maxWeightedIndexCr - (int) ceil(distCr);
    upperBorderCr = maxWeightedIndexCr + (int) ceil(distCr);
  }

  inline bool isFieldCromaRed(int cr) const
  {
//    return abs(cr - maxWeightedIndexCr) < (int) distCr;
    return lowerBorderCr <= cr && cr <= upperBorderCr;
  }

  inline void print(ostream& stream) const
  {
    stream << "distance in Cr channel = " << distCr << endl;
    stream << "max rate value in Cr channel = " << maxWeightedCr << endl;
    stream << "max value in Cr channel = " << maxWeightedIndexCr << endl;
  }//end print

};


#endif  /* FIELDCOLORPREPROCESSINGPERCEPT_H */

