#ifndef CPUTemperatureReader_H
#define CPUTemperatureReader_H

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

  void readLoop();
  
private:
  bool exiting;
  std::thread readThread;

  naoth::CpuData data;
  std::mutex  dataMutex;
  
  std::ifstream temperatureFile;
};

#endif // CPUTemperatureReader_H
