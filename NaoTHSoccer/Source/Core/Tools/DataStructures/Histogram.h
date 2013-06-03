/*
 * File:   histogram.h
 * Author: claas
 *
 */

#ifndef _Histogram_H
#define  _Histogram_H

#include <vector>
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/NaoTHAssert.h"

namespace Statistics
{

  template <int SIZE> class Histogram
  {
    public:
      int rawData[SIZE];
      double normalizedData[SIZE];
      double cumulativeData[SIZE];
     
      int median;
      double mean;
      double variance;
      double sigma;
      double squareMean;
      double skewness;
      double kurtosis;

      Histogram()
      {
        clear();
      };
      
      ~Histogram()
      {};

      void clear()
      {
         calculated = false;
         memset(&rawData, 0, sizeof(rawData));
         memset(&normalizedData, 0, sizeof(normalizedData));
         memset(&cumulativeData, 0, sizeof(cumulativeData));
      };

      void add(int value)
      {
        rawData[value]++;
      };

      void set(int idx, int value)
      {
        rawData[idx] = value;
      };

      void calculate()
      {
        double sum = 0.0;
        mean = 0.0;
        for(int i = 0; i < SIZE; i++)
        {
          sum += rawData[i];
          mean += i * rawData[i];
        }
        mean /= sum;
        median = 0;
        squareMean = 0.0;
        variance = 0.0;
        skewness = 0.0;
        kurtosis = 0.0;
        for(int i = 0; i < SIZE; i++)
        {
          normalizedData[i] = rawData[i] / sum;
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
        }
        squareMean = sqrt(squareMean);
        sigma = sqrt(variance);
        double s2 = sigma * sigma;
        skewness /= (sigma * s2);
        kurtosis /= (s2 * s2);
        calculated = true;
      };

      void plot(std::string id) const
      {
        for(int i = 0; i < SIZE; i++)
        {
          PLOT_GENERIC(id + ":rawHistogram", i, rawData[i]);
          if(calculated)
          {
            PLOT_GENERIC(id + ":normalizedHistogram", i, normalizedData[i]);
            PLOT_GENERIC(id + ":cumulativeHistogram", i, cumulativeData[i]);
          }
        }
      };

      void plotRaw(std::string id) const
      {
        for(int i = 0; i < SIZE; i++)
        {
          PLOT_GENERIC(id + ":rawHistogram", i, rawData[i]);
        }
      };

      void plotNormalized(std::string id) const
      {
        for(int i = 0; i < SIZE; i++)
        {
          PLOT_GENERIC(id + ":normalizedHistogram", i, normalizedData[i]);
        }
      };

      void plotCumulated(std::string id) const
      {
        for(int i = 0; i < SIZE; i++)
        {
          PLOT_GENERIC(id + ":cumulativeHistogram", i, cumulativeData[i]);
        }
      };

  private:
    bool calculated;

  };
}
#endif  /* _Histogram_H */

