
#include "CPUTemperatureReader.h"

#include <Tools/ThreadUtil.h>
#include <chrono>
#include <iostream>

using namespace naoth;
using namespace std;

CPUTemperatureReader::CPUTemperatureReader()
  : exiting(false)
{
  std::cout << "[INFO] CPUTemperatureReader start thread" << std::endl;
  readThread = std::thread([this] {this->readLoop();});
  ThreadUtil::setPriority(readThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(readThread, "CPUTemperature");
  
  temperatureFile.open("/sys/class/thermal/thermal_zone0/temp");
}

void CPUTemperatureReader::get(CpuData& cpuData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if ( lock.owns_lock() ) {
    cpuData = data;
  }
}

CPUTemperatureReader::~CPUTemperatureReader()
{
  std::cout << "[CPUTemperatureReader] stop wait" << std::endl;
  exiting = true;

  if(readThread.joinable()) {
    readThread.join();
  }
  
  temperatureFile.close();
  std::cout << "[CPUTemperatureReader] stop done" << std::endl;
}

void CPUTemperatureReader::readLoop()
{
  while(!exiting)
  {
    { // 
      std::lock_guard<std::mutex> lock(dataMutex);
      if (temperatureFile.is_open() && temperatureFile.good()) {
        // read temperature
        temperatureFile >> data.temperature;
        data.temperature /= 1000.0;
        // reset stream
        temperatureFile.clear();                 // clear fail and eof bits
        temperatureFile.seekg(0, std::ios::beg); // back to the start!
      } else {
        // Failed to open temperatureFile!
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

