/*!
 * \brief Short Time Fourier Transform.
 * \author Thomas Hamboeck, Austrian Kangaroos 2014
 */
#include "STFT.h"

#include <limits>
#include <complex>
#include <iostream>

//                     0, params.nWindowSize, params.nWindowSkipping, params.nWindowSizePadded, spectrumHandler
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
    input           = static_cast<double*>(fftw_malloc(sizeof(double) * windowFrequency));
    output          = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * windowFrequencyHalf));
    outputMag       = new double[windowFrequencyHalf];

    //WARN(windowFrequency >= windowTime, "Frequency window must be greater than Time Window.");

    for(int i = 0; i < windowFrequency; ++i) {
      input[i] = 0.0;
    }

    plan = fftw_plan_dft_r2c_1d(windowFrequency, input, output, FFTW_MEASURE);
}

STFT::~STFT()
{
    if(overflownData) {
        delete[] overflownData;
    }
    if(input) {
        fftw_free(input);
    }
    if(output) {
        fftw_free(output);
    }
    if(outputMag) {
        delete[] outputMag;
    }
    if(plan) {
        fftw_destroy_plan(plan);
    }
}

void STFT::intToFloat(const int16_t &in, double &out)
{
    out = static_cast<double>(in) / (static_cast<double>(std::numeric_limits<int16_t>::max()) + 1.0);
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
    while(true) 
    {
        iEnd = iBegin * channels + windowTime*channels;
        if(iEnd > length) {
            break;
        }

        // copy the data fro the window
        
        iDataChannel = iBegin * channels + offset;
        while(iBuffer < windowTime) {
            intToFloat(data[iDataChannel], input[iBuffer]);
            ++iBuffer;
            iDataChannel += channels;
        }
        /* and the rest is zero */

        fftw_execute(plan);

        /* calc magnitude */
        for(int i = 0; i < windowFrequencyHalf; ++i) {
            outputMag[i] = std::sqrt(output[i][0]*output[i][0] + output[i][1]*output[i][1]);
        }

        if(handler != NULL) {
          handler->handle(outputMag, windowFrequencyHalf);
        }

        /* next cycle */
        iBegin  += windowTimeStep;
        iBuffer = 0;
    }

    nOverflow = 0;
    iDataChannel = iBegin * channels + offset;
    while(iDataChannel < length) {
        /* copy to overflow buffer */
        overflownData[nOverflow] = data[iDataChannel];
        ++nOverflow;
        iDataChannel += channels;
    }
}
