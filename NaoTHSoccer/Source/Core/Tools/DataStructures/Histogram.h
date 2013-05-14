/*
 * File:   histogram.h
 * Author: claas
 *
 */

#ifndef _Histogram_H
#define  _Histogram_H

#include <vector>
#include "Tools/Debug/DebugBufferedOutput.h"

namespace Statistics
{

  template <class T, int SIZE> class Histogram
  {
   public:
      
      Histogram()
      {
          rawData.reserve(SIZE);
          normalizedData.reserve(SIZE);
          cumulativeData.reserve(SIZE);
      };
      
      ~Histogram()
      {};

      void clear()
      {
          rawData.clear();
          normalizedData.clear();
          cumulativeData.clear();
      };

      void add(T value)
      {
        if(rawData.size() < SIZE)
        {
          rawData.push_back(value);
        }
      };

      void calculate()
      {
        double sum = 0.0;
        mean = 0.0;
        for(unsigned int i = 0; i < SIZE; i++)
        {
          sum += rawData[i];
          mean += i * rawData[i];
        }
        mean /= sum;
        median = 0.0;
        squareMean = 0.0;
        variance = 0.0;
        skewness = 0.0;
        kurtosis = 0.0;
        for(unsigned int i = 0; i < SIZE; i++)
        {
          normalizedData.push_back(rawData[i] / sum);
          squareMean += i * i * normalizedData[i];
          double v = (i - mean);
          double v2 = v * v;
          variance += v2 * normalizedData[i];
          skewness += v2 * v * normalizedData[i];
          kurtosis += v2 * v2 * normalizedData[i];

          if(i == 0)
          {
            cumulativeData.push_back(normalizedData[i]);
          }
          else
          {
            cumulativeData.push_back(cumulativeData[i - 1] + normalizedData[i]);
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
      };

      void plot(std::string id)
      {
        for(int i = 0; i < SIZE; i++)
        {
          PLOT_GENERIC(id + ":rawHistogram", i, rawData[i]);
          PLOT_GENERIC(id + ":normalizedHistogram", i, normalizedData[i]);
          PLOT_GENERIC(id + ":cumulativeHistogram", i, cumulativeData[i]);
        }
      };

  private:

    vector<T> rawData;
    vector<double> normalizedData;
    vector<double> cumulativeData;

    T median;
    double mean;
    double variance;
    double sigma;
    double squareMean;
    double skewness;
    double kurtosis;

  };
}
#endif  /* _Histogram_H */

