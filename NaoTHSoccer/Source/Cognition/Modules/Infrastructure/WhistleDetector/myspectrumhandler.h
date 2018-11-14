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
    static unsigned whistleCounter(0), whistleMissCounter(0), whistleDone(false);

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
        whistleMissCounter = 0;
      } else if(whistleCounter > 0) {
        ++whistleMissCounter;
        if(whistleMissCounter > nWhistleMissFrames) {
          whistleCounter = 0;
          whistleMissCounter = 0;
          whistleDone = false;
        }
      }
      if(whistleCounter >= nWhistleOkayFrames) {
		//TODO increment the whistle heard counter!!!
        std::cout << "  !!! Whistle heard !!!" << std::endl;

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
};

#endif // MYSPECTRUMHANDLER

