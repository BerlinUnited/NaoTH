#ifndef MYSPECTRUMHANDLER
#define MYSPECTRUMHANDLER

#include "STFT.h"

//#include "SharedMemoryIO.h"

#include <iostream>


class MySpectrumHandler : public SpectrumHandler
{
public:

	MySpectrumHandler(
		double vWhistleThreshold,
		int nWhistleBegin,
		int nWhistleEnd,
		unsigned nWhistleMissFrames,
		unsigned nWhistleOkayFrames)
    : overallWhistleEventCounter(0),
	vWhistleThreshold(vWhistleThreshold),
	nWhistleBegin(nWhistleBegin),
	nWhistleEnd(nWhistleEnd),
	nWhistleMissFrames(nWhistleMissFrames),
	nWhistleOkayFrames(nWhistleOkayFrames)
  {  }

  void calcMeanDeviationApprox(const double *data, int length, double &mean, double &dev) {
      mean = dev = 0;
      for(int i = 0; i < length; ++i) {
          mean    += data[i];
          dev     += data[i] * data[i];
      }

      dev = std::sqrt(length * dev - mean * mean) / length;
      mean /= length;
  }

  void calcMeanDeviation(const double *data, int length, double &mean, double &dev) {
      mean = dev = 0;

      double n = 0.0;
      double M2 = 0.0;

      for(int i = 0; i < length; ++i) 
      {
        ++n;
        double delta = data[i] - mean;
        mean += delta/n;
        M2 += delta*(data[i] - mean);
      }

      if(n < 2) {
        return;
      }

      dev = std::sqrt(M2 / (n-1));
  }

  virtual void handle(const double *spectrum, int length)
  {
    double mean, dev;
    calcMeanDeviationApprox(spectrum, length, mean, dev);

    bool found;
    const double whistleThresh = mean + vWhistleThreshold * dev;
    found = false;

    int i;
    double max_value = -1;
    for(i = nWhistleBegin; i < nWhistleEnd; ++i) {
      if(spectrum[i] > max_value) {
        max_value = spectrum[i];
      }
      if(spectrum[i] > whistleThresh) {
        found = true;
        break;
      }
    }

    scan_thc.push_back(whistleThresh);
    scan_log.push_back(max_value);

    if(whistleDone) {
      if(!found) {
        ++whistleMissCounter;
        if(whistleMissCounter > nWhistleMissFrames) {
          whistleCounter = 0;
          whistleMissCounter = 0;
          whistleDone = false;
        }
      }
    }
    else
    {
      if(found) {
        ++whistleCounter;
        ++whistleCounterGlobal;
        whistleMissCounter = 0;
      } 
      else 
      {
        if(whistleCounter > 0) {
          ++whistleMissCounter;
          if(whistleMissCounter > nWhistleMissFrames) {
            std::cout << "> " << whistleCounter << std::endl;
            whistleCounter = 0;
            whistleMissCounter = 0;
            whistleDone = false;
          }
        }
        ++whistleMissCounterGlobal;
      }
      if(whistleCounter >= nWhistleOkayFrames) {
		//TODO increment the whistle heard counter!!!
        std::cout << "  !!! Cool Whistle heard !!!" << std::endl;
        whistleDetections++;

        whistleCounter = 0;
        whistleMissCounter = 0;
        whistleDone = true;
      }
    }
  }
private:
  int overallWhistleEventCounter;
  double vWhistleThreshold;
  int nWhistleBegin;
  int nWhistleEnd;
  unsigned nWhistleMissFrames;
  unsigned nWhistleOkayFrames;

public:
  // internal state
  unsigned whistleCounterGlobal = 0;
  unsigned whistleMissCounterGlobal = 0;
  unsigned whistleCounter = 0; 
  unsigned whistleMissCounter = 0;
  unsigned whistleDone = false;

  std::vector<double> scan_log;
  std::vector<double> scan_thc;
  double filter_value = 0;

  unsigned whistleDetections = 0;
};

#endif // MYSPECTRUMHANDLER

