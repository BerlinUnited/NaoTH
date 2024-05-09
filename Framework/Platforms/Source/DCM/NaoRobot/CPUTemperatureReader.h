#ifndef CPU_TEMPERATURE_READER_H
#define CPU_TEMPERATURE_READER_H

#include "Representations/Infrastructure/CpuData.h"

#include <mutex>
#include <thread>
#include <fstream>

class CPUTemperatureReader
{
public:
  CPUTemperatureReader();
  ~CPUTemperatureReader();

  void get(naoth::CpuData& cpuData);
  
private:
  void readLoop();
  
  bool exiting;
  std::thread readThread;

  naoth::CpuData data;
  std::mutex dataMutex;
  
  std::ifstream temperatureFile;
};

#endif // CPU_TEMPERATURE_READER_H
