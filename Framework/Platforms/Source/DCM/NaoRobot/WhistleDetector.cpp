#include "WhistleDetector.h"

#include <Tools/ThreadUtil.h>
#include <chrono>
#include <iostream>
#include <cmath>
#include <pulse/error.h>

#include "Tools/Debug/NaoTHAssert.h"

namespace naoth
{

WhistleDetector::WhistleDetector()
:
  command(0),
  whistleListFile("whistles.lst"),
  activeChannels("1010"),
  threshold(0.25),
  checkAllWhistles(true),
  saveRawAudio(true),
  audioReadBuffer(BUFFER_SIZE_RX, 0),
  running(false),
  recording(false),
  resetting(false),
  startStopCount(0),
  deinitCyclesCounter(0),
  samplesRecorded(0)
{
  whistlePercept.counter = 0;
  std::cout << "[INFO] WhistleDetector start thread" << std::endl;
  whistleDetectorThread = std::thread([this] {this->execute();});
  ThreadUtil::setPriority(whistleDetectorThread, ThreadUtil::Priority::lowest);

  signal2correlateBuffer = (double*) fftw_malloc(sizeof(double) * WHISTLE_FFT_LEN);
  fftOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (WHISTLE_BUFF_LEN + 1));
  fftIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (WHISTLE_BUFF_LEN + 1));
  correlatedBuffer = (double*) fftw_malloc(sizeof(double) * WHISTLE_FFT_LEN);

  planFFT = fftw_plan_dft_r2c_1d(WHISTLE_FFT_LEN, signal2correlateBuffer, fftOut, FFTW_ESTIMATE);
  planIFFT = fftw_plan_dft_c2r_1d(WHISTLE_FFT_LEN, fftIn, correlatedBuffer, FFTW_ESTIMATE);

  clearBuffers();

  // loadReferenceTestWhistle();
  loadReferenceWhistles();

  // Load reference whistle
  if(referenceWhistleSpectra.size() <= 0)
  {
    std::cerr << "Couldn't load any reference whistle!" << std::endl;
  }
}

WhistleDetector::~WhistleDetector()
{ 

  if(whistleDetectorThread.joinable())
  {
    whistleDetectorThread.join();
  }

  fftw_destroy_plan(planFFT);
  fftw_destroy_plan(planIFFT);

  fftw_free(signal2correlateBuffer);
  fftw_free(fftIn);
  fftw_free(fftOut);
  fftw_free(correlatedBuffer);

  for(size_t i = 0; i < referenceWhistleSpectra.size(); i++)
  {
    fftw_free(referenceWhistleSpectra[i]);
  }

  if(recording)
  {
    deinitAudio();
  }
}

void WhistleDetector::get(naoth::WhistlePercept& perceptData)
{
  std::unique_lock<std::mutex> lock(getMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    if(whistlePercept.recognizedWhistles.size() > 0)
    {
      perceptData = whistlePercept;
      whistlePercept.reset();
      std::cout << perceptData.recognizedWhistles.size() << " whistles fired since last cognition cycle" << std::endl;
    }
  }
}

void WhistleDetector::set(const naoth::WhistleControl& controlData)
{
  std::unique_lock<std::mutex> lock(setMutex, std::try_to_lock);
  if ( lock.owns_lock() )
  {
    command = controlData.onOffSwitch;
    threshold = controlData.threshold;
    checkAllWhistles = controlData.checkAllWhistles;
    saveRawAudio = controlData.saveRawAudio;

    activeChannels = controlData.activeChannels;
    if(whistleListFile != controlData.whistleListFile)
    {
      resetting =  true;
      if(recording)
      {
        deinitAudio();
      }
      clearBuffers();
      for(size_t i = 0; i < referenceWhistleSpectra.size(); i++)
      {
        fftw_free(referenceWhistleSpectra[i]);
      }
      referenceWhistleSpectra.clear();
      referenceWhistleAutoCorrelationMaxima.clear();
      referenceWhistleNames.clear();    
      whistleListFile = controlData.whistleListFile;
      loadReferenceWhistles();
      resetting = false;
    }
  }
}

void WhistleDetector::execute()
{
  running = true;

  // return;
  while(running)
  {
    int error = 0;

    /*
    if(testFileMode)
    {
      command = 1; //debug
    }
    */
    if(!recording)
    {
      if(!resetting && command == 1)
      {
        std::cout << "start recording" << std::endl;
        initAudio();
      }
      else if(resetting)
      {
        usleep(128);
      }
    }
    else
    {
      if(command == 0)
      {
        // deinit audio device ~8 seconds after switch off command was set
        // (recording has blocking behaviour 1024 samples eqal 128 ms 63*128 equals ~8s
        if(deinitCyclesCounter >= 63)
        {
          std::cout << "stop recording" << std::endl;
          deinitAudio();
          deinitCyclesCounter = 0;
          usleep(128);
          continue;
        }
        deinitCyclesCounter++;
      }
    }

    if(!resetting && recording)
    {
      //calculate amount of samples needed
      int samplesToRead = SAMPLE_NEW_COUNT * NUM_CHANNELS_RX; //BUFFER_SIZE_RX;
      int bytesToRead = samplesToRead * static_cast<int>(sizeof(short));

      // Record some data
      if (!paSimple || paSimple == NULL)
      {
        std::cerr << "[PulseAudio] pa_simple == NULL" << std::endl;
        running = false;
      }

      if (pa_simple_read(paSimple, &audioReadBuffer[0], bytesToRead, &error) < 0)
      {
        std::cerr << "[PulseAudio] pa_simple_read() failed: " << pa_strerror(error) << std::endl;
        running = false;
      }
      /*
      if(testFileMode)
      {
        audioTestFile.read(reinterpret_cast<char*>(&audioReadBuffer[0]), bytesToRead);
        if(audioTestFile.gcount() < bytesToRead)
        {
          std::cout << "Testfile done" << std::endl;
          audioTestFile.close();
          return;
        }
      }
      */

      //
      // Order of audio channels while recording 4 channels is for V4 Naos as follows:
      // TODO: is this the same for the newer Naos?
      // - Channel 0 - Left  Microphone
      // - Channel 1 - Front Microphone
      // - Channel 2 - Right Microphone
      // - Channel 3 - Rear  Microphone
      //
      for(int c = 0; c < NUM_CHANNELS_RX; c++)
      {
        if(NUM_CHANNELS_RX == 1)
        {
          for(int i = SAMPLE_OVERLAP_COUNT; i < SAMPLE_COUNT ; i++)
          {
            signal2correlateBuffer[i - SAMPLE_OVERLAP_COUNT] = signal2correlateBuffer[i];
          }
          for(int i = 0; i < SAMPLE_NEW_COUNT; i++)
          {
            signal2correlateBuffer[SAMPLE_OVERLAP_COUNT + i] = static_cast<double>(audioReadBuffer[NUM_CHANNELS_RX * i + c]) / static_cast<double>(std::numeric_limits<short>::max());
          }
        }
        else
        {
          for(int i = SAMPLE_OVERLAP_COUNT; i < SAMPLE_COUNT ; i++)
          {
            signalBuffer[c][i - SAMPLE_OVERLAP_COUNT] = signalBuffer[c][i];
          }
          for(int i = 0; i < SAMPLE_NEW_COUNT; i++)
          {
            signalBuffer[c][SAMPLE_OVERLAP_COUNT + i] = static_cast<double>(audioReadBuffer[NUM_CHANNELS_RX * i + c]) / static_cast<double>(std::numeric_limits<short>::max());
          }
          for(int i = 0; i < WHISTLE_BUFF_LEN; i++)
          {
            signal2correlateBuffer[i] = signalBuffer[c][i];
          }
        }

        if(deinitCyclesCounter == 0 && activeChannels[c] == '1' && detectWhistles(c))
        {
          whistlePercept.counter++;
        }
      }
      
      if (saveRawAudio)
      {
        outputFileStream.write((char*)(&audioReadBuffer[0]), bytesToRead);
      }    
      samplesRecorded += SAMPLE_NEW_COUNT;
    }
    else
    {
      usleep(128);
      continue;
    }
  } // end while
} // end execute

void WhistleDetector::clearBuffers()
{
  for(int c = 0; c < NUM_CHANNELS_RX; c++)
  {
    if(NUM_CHANNELS_RX > 1)
    {
      for(int i = 0; i < WHISTLE_FFT_LEN ; i++)
      {
        signalBuffer[c][i] = 0.0;
      }
    }
  }
  for(int i = 0; i < WHISTLE_FFT_LEN; i++)
  {
    signal2correlateBuffer[i] = 0.0;
    if(i < WHISTLE_BUFF_LEN + 1)
    {
      fftIn[i][0] = 0.0;
      fftIn[i][1] = 0.0;
      fftOut[i][0] = 0.0;
      fftOut[i][1] = 0.0;
    }
    correlatedBuffer[i] = 0.0;
  }
}

void WhistleDetector::loadReferenceWhistles()
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

          //for(int i = 0; i < WHISTLE_FFT_LEN; i++)
          //{
          //  correlatedBuffer[i] = 0.0;
          //}
          //for(int i = 0; i < WHISTLE_BUFF_LEN + 1; i++)
          //{
          //  fftIn[i][0] = whistleSpectrum[i][0];
          //  fftIn[i][1] = -whistleSpectrum[i][1];
          //}
          //fftw_execute(planIfft);

          //short tmpBuf[WHISTLE_FFT_LEN];
          //for(int i = 0; i < WHISTLE_FFT_LEN; i++)
          //{
          //  double t = round(correlatedBuffer[WHISTLE_FFT_LEN - 1 - i] / 2048.0 * static_cast<double>(std::numeric_limits<short>::max()));
          //  std::cout << t << std::endl;
          //  if(t > std::numeric_limits<short>::max()) t = std::numeric_limits<short>::max();
          //  if(-t < std::numeric_limits<short>::min()) t = std::numeric_limits<short>::min();
          //  tmpBuf[i] = static_cast<short>(t);
          //}

          //whistleFileName = line + "_gen.raw";
          //std::cout << "write " << whistleFileName << std::endl;
          //std::ofstream whistleOut;
          //whistleOut.open(whistleFileName.c_str(), std::ios_base::binary);
          //whistleOut.write(reinterpret_cast<char*>(tmpBuf), sizeof(short) * WHISTLE_FFT_LEN);
          //whistleOut.close();
        }
      }
      else
      {
        std::cout << "Could not load: " << whistleFileName.c_str() << std::endl;
      }
    }
  }
}

bool WhistleDetector::detectWhistles(int channel)
{
  bool whistleDetected = false;
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
    if(response >= threshold)
    {
      std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") maxCorr " << correlation << " ref corr " << referenceWhistleAutoCorrelationMaxima[idxWhistle] << std::endl;
      std::cout << "Whistle " << referenceWhistleNames[idxWhistle] << "(No. " << idxWhistle << ") detected! " << response << std::endl;
      whistlePercept.addWhistle(referenceWhistleNames[idxWhistle], samplesRecorded, response);
      whistleDetected = true;
      if(!checkAllWhistles)
      {
        return true;
      }
    }
  }
  if(whistleDetected)
  {
    std::cout << "@ Audio Channel " << channel;
    /*
    if(testFileMode)
    {
      std::cout << " on Position " << (audioTestFile.tellg() / (sizeof(short) * NUM_CHANNELS_RX));
    }
    */
    std::cout << std::endl;
  }
  return whistleDetected;
}

void WhistleDetector::initAudio()
{
  clearBuffers();

  startStopCount++;

  int error;

  pa_sample_spec paSampleSpec;
  paSampleSpec.format = PA_SAMPLE_S16LE;
  paSampleSpec.rate = SAMPLE_RATE_RX;
  paSampleSpec.channels = NUM_CHANNELS_RX;

  // Create the recording stream
  std::string appName = "WhistleDetector";
  if (!(paSimple = pa_simple_new(NULL, appName.c_str(), PA_STREAM_RECORD, NULL, "WhistleDetectorRecord", &paSampleSpec, NULL, NULL, &error)))
  {
    std::cerr << "[PulseAudio] pa_simple_new() failed: " << pa_strerror(error) << "\n" << std::endl;
    paSimple = NULL;
  }
  else
  {
    std::cout << "[PulseAudio] device opened" << std::endl;
    std::cout << "[PulseAudio] Rate: " << paSampleSpec.rate <<std::endl;
    std::cout << "[PulseAudio] Channels: " << (int) paSampleSpec.channels <<std::endl;
    std::cout << "[PulseAudio] Buffer Size: " << BUFFER_SIZE_RX <<std::endl;
    recording = true;
    samplesRecorded = 0;
  }

  if(saveRawAudio)
  {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    std::stringstream filePath;
    filePath << "/tmp/capture_" << robotName << "_" << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << "-" << 1 + ltm->tm_hour << "-" <<  1 + ltm->tm_min << "-" <<  1 + ltm->tm_sec << "_" << static_cast<double>(paSampleSpec.rate) / 1000 <<  "kHz_" << (int) paSampleSpec.channels << "channels.raw";
    whistlePercept.captureFile = filePath.str();
    outputFileStream.open(whistlePercept.captureFile.c_str(), std::ios_base::out | std::ios_base::binary);
  }
}

void WhistleDetector::deinitAudio()
{
  recording = false;
  samplesRecorded = 0;
  if(saveRawAudio)
  {
    outputFileStream.close();
  }
  if(recording)
  {
    if (paSimple != NULL)
    {
      std::cout << "[PulseAudio] device closed" << std::endl;
      pa_simple_free(paSimple);
      paSimple = NULL;
    }
  }
}

void WhistleDetector::setRobotName(const std::string& name)
{
  robotName = name;
}


}
