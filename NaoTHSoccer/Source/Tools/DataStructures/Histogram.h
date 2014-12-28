/*
 * File:   histogram.h
 * Author: claas
 *
 */

#ifndef _Histogram_H
#define  _Histogram_H

#include <vector>
//#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/NaoTHAssert.h"

namespace Statistics
{
  class HistogramX
  {
    public:
      static const int maxInt = (int)((unsigned) -1 / 2);
      int size;

      std::vector<int> rawData;
      std::vector<double> normalizedData;
      std::vector<double> cumulativeData;
     
      int maxTotalSum;
      int median;
      int min;
      int max;
      int common;
      int spanWidth;
      double sum;
      double mean;
      double variance;
      double sigma;
      double squareMean;
      double skewness;
      double kurtosis;

      HistogramX()
      {
        maxTotalSum = maxInt;
        size = 0;
        clear();
      }
      
      HistogramX(int newSize)
      {
        maxTotalSum = maxInt;
        size = newSize;
        resize(newSize);
      }
      
      ~HistogramX()
      {}

      void clear()
      {
        median = 0;
        min = 0;
        max = 0;
        common = 0;
        spanWidth = 0;
        sum = 0.0;
        mean = 0.0;
        variance = 0.0;
        sigma = 0.0;
        squareMean = 0.0;
        skewness = 0.0;
        kurtosis = 0.0;
        calculated = false;
        for(int i = 0; i < size; i++)
        {
          rawData[i] = 0;
          normalizedData[i] = 0.0;
          cumulativeData[i] = 0.0;
        }
      }

      //resize histogram to newSize bins
      void resize(int newSize)
      {
        size = newSize;
        rawData.resize(size, 0);
        normalizedData.resize(size, 0.0);
        cumulativeData.resize(size, 0.0);
        clear();
      }

      //add 1 to the bin for value
      void add(int value)
      {
        rawData[value]++;
      }

      //add value to the bin at index idx
      void add(int idx, int value)
      {
        rawData[idx] += value;
      }

      //set value of bin at index idx
      void set(int idx, int value)
      {
        rawData[idx] = value;
      }

      //set maximal total sum for recalulation of bins
      void setMaxTotalSum(int maxSum)
      {
        maxTotalSum = maxSum < maxInt / 2 ? maxSum * 2 : maxInt;// > 10 * size ? maxSum : 10 * size;
      }

      //reset maximal total sum for recalulation of bins to max integer value
      void resetMaxTotalSum()
      {
        maxTotalSum = maxInt;
      }

      //calculate normalized and cumulated histograms and some moments
      void calculate()
      {
        sum = 0.0;
        mean = 0.0;
        for(int i = 0; i < size; i++)
        {
          sum += rawData[i];
          mean += i * rawData[i];
        }
        //if histogram is empty skip here
        if(sum == 0) 
          return;
        //save total sum of bins if calculate() is running the first time, needed for recalculation if accumulation behaviour is used
        mean /= sum;
        median = 0;
        min = 255;
        max = 0;
        common = 0;
        squareMean = 0.0;
        variance = 0.0;
        skewness = 0.0;
        kurtosis = 0.0;
        //can only be true if accumulation behaviour is used and total sum is about to reach max value of int
        bool reachingMaxSum = sum >= maxTotalSum;
        double newSum = 0.0;
        int maxRate = 0;
        for(int i = 0; i < size; i++)
        {
          normalizedData[i] = rawData[i] / sum;
          //if accumulation behaviour is used and total sum is about to reach max value of int,
          //recalculate bin values by use of normalized bin value and the total sum of first run
          //we do loose some accuracy here and total sum of bins is not equal to the one from first run because of rounding doubles to ints
          if(reachingMaxSum) 
            rawData[i] = (int) Math::round(normalizedData[i] *maxTotalSum * 0.5); 
          newSum += rawData[i];
          
          if(maxRate < rawData[i])
          {
            maxRate = rawData[i];
            common = i;
          }
          squareMean += i * i * normalizedData[i];
          double v = (i - mean);
          double v2 = v * v;
          variance += v2 * normalizedData[i];
          skewness += v2 * v * normalizedData[i];
          kurtosis += v2 * v2 * normalizedData[i];

          if(i == 0)
          {
            cumulativeData[i] = normalizedData[i];
          }
          else
          {
            cumulativeData[i] = cumulativeData[i - 1] + normalizedData[i];
          }
          if(median == 0.0 && cumulativeData[i] >= 0.5)
          {
            median = i;
          }
          if(i < min && rawData[i] > 0)
          {
            min = i;
          }
          if(i > max && rawData[i] > 0)
          {
            max = i;
          }
        }
        sum = newSum;
        spanWidth = max - min;
        squareMean = sqrt(squareMean);
        sigma = sqrt(variance);
        double s2 = sigma * sigma;
        skewness /= (sigma * s2);
        kurtosis /= (s2 * s2);
        calculated = true;
      }

      //generate plot for raw, normalized and cumulated histograms
      void plot(std::string id) const
      {
        for(int i = 0; i < size; i++)
        {
          //PLOT_GENERIC(id + ":rawHistogram", i, rawData[i]);
          //if(calculated)
          //{
          //  PLOT_GENERIC(id + ":normalizedHistogram", i, normalizedData[i]);
          //  PLOT_GENERIC(id + ":cumulativeHistogram", i, cumulativeData[i]);
          //}
        }
      }

      //generate plot only for raw histogram
      void plotRaw(std::string id) const
      {
        for(int i = 0; i < size; i++)
        {
          //PLOT_GENERIC(id + ":rawHistogram", i, rawData[i]);
        }
      }

      //generate plot only for normalized histogram
      void plotNormalized(std::string id) const
      {
        for(int i = 0; i < size; i++)
        {
          //PLOT_GENERIC(id + ":normalizedHistogram", i, normalizedData[i]);
        }
      }

      //generate plot only for cumulated histogram
      void plotCumulated(std::string id) const
      {
        for(int i = 0; i < size; i++)
        {
          //PLOT_GENERIC(id + ":cumulativeHistogram", i, cumulativeData[i]);
        }
      }

  protected:
    bool calculated;

  };

  template <int SIZE> class Histogram : public HistogramX
  {
  public:
    Histogram() : HistogramX(SIZE) {}
  };

}
#endif  /* _Histogram_H */

