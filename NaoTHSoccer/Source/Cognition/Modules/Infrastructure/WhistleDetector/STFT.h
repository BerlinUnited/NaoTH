/*!
 * \brief Short Time Fourier Transform.
 * \author Thomas Hamboeck, Austrian Kangaroos 2014
 */

#ifndef __AK_STFT__
#define __AK_STFT__

#include <fftw3/fftw3.h>
#include <complex>
#include <functional>

#include <stdint.h>

class SpectrumHandler
{
public:
  virtual void handle(const float *spectrum, int length) = 0;
};

class STFT
{
public:
    STFT(const int channelOffset, 
         const int windowTime, 
         const int windowTimeStep, 
		 const int windowFrequency,
		 SpectrumHandler *handler
		 );
    
    virtual ~STFT();

    virtual void newData(const int16_t*, int, short);


protected:
    void intToFloat(const int16_t &in, float &out);

    const int offset;
    const int windowTime, windowTimeStep, windowFrequency, windowFrequencyHalf;

	SpectrumHandler* handler;

    int nOverflow;
    int16_t *overflownData;
    float *input;
    fftwf_complex *output;
    float *outputMag;

    fftwf_plan plan;
};

#endif
