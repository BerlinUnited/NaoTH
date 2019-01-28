//==============================================================================
//------------------------------------------------------------------------------
#include <chrono>
#include <iostream>
//#include <Tools/ThreadUtil.h>
#include <Tools/ThreadUtil.h>
#include "USBJoypad.h"
//------------------------------------------------------------------------------
using namespace naoth;
using namespace std;
//------------------------------------------------------------------------------
void clJoypad::get(clJoypadData& rJoypadData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);

  if (lock.owns_lock())
  {
    rJoypadData=JoypadInputData;
  }
}
//------------------------------------------------------------------------------
void clJoypad::readLoop()
{
  while (!exiting)
  {
    { // 
      std::lock_guard<std::mutex> lock(dataMutex);
/*
      if (temperatureFile.is_open() && temperatureFile.good())
      { // read temperature
        temperatureFile >> data.temperature;
        data.temperature /= 1000.0;
        // reset stream
        temperatureFile.clear();                 // clear fail and eof bits
        temperatureFile.seekg(0, std::ios::beg); // back to the start!
      }
      else 
      {
        // Failed to open temperatureFile!
      }
*/
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // much less
  }
}
//------------------------------------------------------------------------------
clJoypad::clJoypad()
  : exiting(false),
    lastGetTime(0)
{
  std::cout << "[INFO] JoypadDataReader thread start" << std::endl;
  readThread = std::thread( [this]{this->readLoop();} );
  ThreadUtil::setPriority(readThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(readThread, "JoypadData (raw)");

//  temperatureFile.open("/sys/class/thermal/thermal_zone0/temp");
}
//------------------------------------------------------------------------------
clJoypad::~clJoypad()
{
  std::cout << "[JoypadDataReader] stop wait" << std::endl;
  exiting = true;

  if (readThread.joinable()) 
  {
    readThread.join();
  }
//  temperatureFile.close();
  std::cout << "[JoypadDataReader] stop done" << std::endl;
}
//------------------------------------------------------------------------------
//==============================================================================
