/**
* @file WhistleDetectorTemplateMatching.cpp
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* Implements a whistle detector based on template matching.
*/

#include "WhistleDetectorTemplateMatching.h"

#include <vector>
#include <fstream>

#define SAMPLE_COUNT            (1024)
#define WHISTLE_BUFF_LEN        (SAMPLE_COUNT)
#define WHISTLE_FFT_LEN         (WHISTLE_BUFF_LEN * 2)

WhistleDetectorTemplateMatching::WhistleDetectorTemplateMatching()
  : 
  lastDataTimestamp(0)
{
  DEBUG_REQUEST_REGISTER("Plot:WhistleDetectorTemplateMatching:whistle_response", "", false);

  signal2correlateBuffer = static_cast<double*>(fftw_malloc(sizeof(double) * WHISTLE_FFT_LEN));
  fftOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (WHISTLE_BUFF_LEN + 1));
  fftIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (WHISTLE_BUFF_LEN + 1));
  correlatedBuffer = static_cast<double*>(fftw_malloc(sizeof(double) * WHISTLE_FFT_LEN));

  planFFT = fftw_plan_dft_r2c_1d(WHISTLE_FFT_LEN, signal2correlateBuffer, fftOut, FFTW_ESTIMATE);
  planIFFT = fftw_plan_dft_c2r_1d(WHISTLE_FFT_LEN, fftIn, correlatedBuffer, FFTW_ESTIMATE);

  loadReferenceWhistles();

  getDebugParameterList().add(&params);
}

WhistleDetectorTemplateMatching::~WhistleDetectorTemplateMatching()
{
	getDebugParameterList().remove(&params);
}

void WhistleDetectorTemplateMatching::loadReferenceWhistles()
{
  std::ifstream whistleListFile;
  std::ifstream whistleFile;

  //read text file with list of reference whistle spectrum files to use
  std::string referenceWhistleList = "Config/referenceWhistles/whistles.lst";
  whistleListFile.open(referenceWhistleList.c_str());
  ASSERT_MSG(whistleListFile.is_open(), "Couldn't find/open file with list of reference whistles ("+referenceWhistleList+")!");

  std::string line;
  std::cout << "loading whistle reference data" << std::endl;
  while(std::getline(whistleListFile, line))
  {
    if(line.size() > 0)
    {
      size_t ext = line.find_last_of(".dat");
      // ignore "commented" lines or lines without ".dat" substring
      if(line.at(0) == '#' || ext==std::string::npos) {
        std::cout << "ignore reference whistle: " << line << std::endl;
        continue;
      }
      // "trim" line to file extension
      line.erase(ext+1);

      std::string whistleFileName = "Config/referenceWhistles/" + line;
      whistleFile.open(whistleFileName.c_str(), std::ios_base::binary);
      if(whistleFile.is_open())
      {
        std::cout << "trying to load '" << whistleFileName << "'." << std::endl;
        fftw_complex* whistleSpectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (WHISTLE_BUFF_LEN + 1));
        double autoCorrelationMax = 0.0;

        //read file content byte wise
        for(int j = 0; j < WHISTLE_BUFF_LEN + 1; j++)
        {
          whistleFile.read(reinterpret_cast<char*>(&(whistleSpectrum[j][0])), sizeof(double));
          whistleFile.read(reinterpret_cast<char*>(&(whistleSpectrum[j][1])), sizeof(double));
        }
        whistleFile.read(reinterpret_cast<char*>(&autoCorrelationMax), sizeof(double));
        whistleFile.close();

        for(int i = 0; i < WHISTLE_FFT_LEN; i++)
        {
          correlatedBuffer[i] = 0.0;
        }
        for(int j = 0; j < WHISTLE_BUFF_LEN + 1; j++)
        {
          const double realFFTIn = whistleSpectrum[j][0];
          const double imagFFTIn = whistleSpectrum[j][1];

          const double realFFTCmp = whistleSpectrum[j][0];
          const double imagFFTCmp = -whistleSpectrum[j][1];

          // real x real - imag x imag
          fftIn[j][0] = realFFTIn * realFFTCmp - imagFFTIn * imagFFTCmp;
          // real x imag + imag x real
          fftIn[j][1] = realFFTIn * imagFFTCmp + imagFFTIn * realFFTCmp;
        }
        fftw_execute(planIFFT); // calculate the IFFT which is now the same as the correlation

        //calculate the maximum of the correlation
        double correlation = 0.0;
        for(int j = 0; j < WHISTLE_FFT_LEN; j++)
        {
          if(correlation < fabs(correlatedBuffer[j]))
          {
            correlation = fabs(correlatedBuffer[j]);
          }
        }
        if(fabs(autoCorrelationMax - correlation) > 0.1f)
        {
          std::cerr << "error loading " << line << " calculated and read max correlation values do not match!"<< std::endl;
          std::cerr << "(ACmax_read=" << autoCorrelationMax << ") (ACmax_calc=" << correlation << ")" << " diff=" << fabs(autoCorrelationMax - correlation) << std::endl;
        }
        else
        {
          //push to list of reference whistle spectra
          
          referenceWhistleSpectra.push_back(whistleSpectrum);
          referenceWhistleAutoCorrelationMaxima.push_back(autoCorrelationMax);
          referenceWhistleNames.push_back(line);

          std::cout << "successfully loaded " << line << " (ACmax_read=" << autoCorrelationMax << " (ACmax_calc=" << correlation << ")." << std::endl;
        }
      }
      else
      {
        std::cout << "Could not load: " << whistleFileName.c_str() << std::endl;
      }
    }
  }
}


void WhistleDetectorTemplateMatching::execute()
{
	getWhistlePercept().reset();

  // there is no new data, return
  if(lastDataTimestamp >= getAudioData().timestamp ) {
    return;
  }

  // save the timestamp for later
  lastDataTimestamp = getAudioData().timestamp;
  const std::vector<short>& data = getAudioData().samples;
  const int channels = getAudioData().numChannels;

  // copy to the input data
  for (size_t i = 0; i < data.size(); i += channels) {
    intToNormalizedDouble(data[i], signal2correlateBuffer[i]);
  }
  

  fftw_execute(planFFT); // compute the FFT - repeat as needed

  for(size_t idxWhistle = 0; idxWhistle < referenceWhistleSpectra.size(); idxWhistle++)
  {
    
    // Now multiply the FFT of dataIn with the conjugate FFT of cmpData
    for(int j = 0; j < WHISTLE_BUFF_LEN + 1; j++)
    {
      const double realFFTIn = fftOut[j][0];
      const double imagFFTIn = fftOut[j][1];

      const double realFFTCmp = referenceWhistleSpectra[idxWhistle][j][0];
      const double imagFFTCmp = referenceWhistleSpectra[idxWhistle][j][1];

      // real x real - imag x imag
      fftIn[j][0] = realFFTIn * realFFTCmp - imagFFTIn * imagFFTCmp;
      // real x imag + imag x real
      fftIn[j][1] = realFFTIn * imagFFTCmp + imagFFTIn * realFFTCmp;
    }
    fftw_execute(planIFFT); // calculate the IFFT which is now the same as the correlation

    // Post Processing:
    // Find the maximum of the correlation and weight by the
    // highest value possible, which is given by the auto correlation

    double correlation = 0.0;
    for(int j = 0; j < WHISTLE_FFT_LEN; j++)
    {
      if(correlation < fabs(correlatedBuffer[j]))
      {
        correlation = fabs(correlatedBuffer[j]);
      }
    }

    double response = correlation / referenceWhistleAutoCorrelationMaxima[idxWhistle];
    
    DEBUG_REQUEST("Plot:WhistleDetectorTemplateMatching:whistle_response",
      PLOT_GENERIC("WhistleDetectorTemplateMatching:whistle_response:"+referenceWhistleNames[idxWhistle], getFrameInfo().getTime(), response);
    );

    if(response >= params.threshold)
    {
      std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") maxCorr " << correlation << " ref corr " << referenceWhistleAutoCorrelationMaxima[idxWhistle] << std::endl;
      std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") detected! " << response << std::endl;
      getWhistlePercept().whistleDetected = true;
    }
  }

  PLOT("WhistleDetectorTemplateMatching:whistle_filter", getWhistlePercept().whistleDetected);
}
