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

  void calcMeanDeviation(const double *data, int length, double &mean, double &dev) {
      mean = dev = 0;
      for(int i = 0; i < length; ++i) {
          mean    += data[i];
          dev     += data[i] * data[i];
      }

      dev = std::sqrt(length * dev - mean * mean) / length;
      mean /= length;
  }

  virtual void handle(const double *spectrum, int length)
  {
    double mean, dev;
    calcMeanDeviation(spectrum, length, mean, dev);

    bool found;
    const double whistleThresh = mean + vWhistleThreshold * dev;
    found = false;

    int i;
    for(i = nWhistleBegin; i < nWhistleEnd; ++i) {
      if(spectrum[i] > whistleThresh) {
        found = true;
        break;
      }
    }

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
      } else {
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
};

#endif // MYSPECTRUMHANDLER

