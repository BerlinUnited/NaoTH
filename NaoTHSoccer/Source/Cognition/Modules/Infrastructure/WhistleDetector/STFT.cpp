/*!
 * \brief Short Time Fourier Transform.
 * \author Thomas Hamboeck, Austrian Kangaroos 2014
 */
#include "STFT.h"

#include <limits>
#include <complex>
#include <iostream>

STFT::STFT(const int channelOffset, const int windowTime, const int windowTimeStep, const int windowFrequency, SpectrumHandler *handler)
    : offset(channelOffset),
      windowTime(windowTime), 
      windowTimeStep(windowTimeStep), 
      windowFrequency(windowFrequency), 
      windowFrequencyHalf(windowFrequency / 2 + 1),
	  handler(handler),
      nOverflow(0), overflownData(NULL), input(NULL), output(NULL), outputMag(NULL)
{
    overflownData   = new int16_t[windowTime]; /* actually a max of (windowTime - 1) */
    input           = static_cast<float*>(fftwf_malloc(sizeof(float) * windowFrequency));
    output          = static_cast<fftwf_complex*>(fftwf_malloc(sizeof(fftwf_complex) * windowFrequencyHalf));
    outputMag       = new float[windowFrequencyHalf];

    //WARN(windowFrequency >= windowTime, "Frequency window must be greater than Time Window.");

    for(int i = 0; i < windowFrequency; ++i) {
        input[i] = 0.0f;
    }

    plan = fftwf_plan_dft_r2c_1d(windowFrequency, input, output, FFTW_MEASURE);
}

STFT::~STFT()
{
    if(overflownData) {
        delete[] overflownData;
    }
    if(input) {
        fftwf_free(input);
    }
    if(output) {
        fftwf_free(output);
    }
    if(outputMag) {
        delete[] outputMag;
    }
    if(plan) {
        fftwf_destroy_plan(plan);
    }
}

void STFT::intToFloat(const int16_t &in, float &out)
{
    out = static_cast<float>(in) / (std::numeric_limits<int16_t>::max() + 1);
}

void STFT::newData(const int16_t *data, int length, short channels)
{
    int iBegin, iEnd, iDataChannel, iBuffer;

    iBuffer = 0;

    /* for each overflown data */
    while(iBuffer < nOverflow) {
        intToFloat(overflownData[iBuffer], input[iBuffer]);
        ++iBuffer;
    }

    iBegin = 0;
    while(true) {
        iEnd = iBegin + windowTimeStep;
        if(iEnd > length) {
            break;
        }

        iDataChannel = iBegin * channels + offset;
        while(iBuffer < windowTime) {
            intToFloat(data[iDataChannel], input[iBuffer]);
            ++iBuffer;
            iDataChannel += channels;
        }
        /* and the rest is zero */

        fftwf_execute(plan);

        /* calc magnitude */
        for(int i = 0; i < windowFrequencyHalf; ++i) {
            outputMag[i] = std::abs(*reinterpret_cast<std::complex<float>* >(&output[i]));
        }
        if(handler != NULL)
        {
          handler->handle(outputMag, windowFrequencyHalf);
        }

        /* next cycle */
        iBuffer = 0;
        iBegin  += windowTimeStep;
    }

    nOverflow = 0;
    iDataChannel = iBegin * channels + offset;
    while(iBegin < length) {
        /* copy to overflow buffer */
        overflownData[nOverflow] = data[iDataChannel];
        ++nOverflow;
        ++iBegin;
        iDataChannel += channels;
    }
}
