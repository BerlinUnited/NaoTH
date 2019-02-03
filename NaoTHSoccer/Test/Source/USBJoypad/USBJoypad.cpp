//--------------------------------------------------------------------------------------------------
//
// @file USBJoypad.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Joypad data supply for Nao
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include "Tools\ThreadUtil.h"
#include "USBJoypad.h"
//#include "udevIf.h"
//------------------------------------------------------------------------------
using namespace naoth;
using namespace std;
//------------------------------------------------------------------------------
void USBJoypad::get(USBJoypadData& rJoypadData)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);

  if (lock.owns_lock())
  {
    rJoypadData=JoypadInputData;
  }
}
//------------------------------------------------------------------------------
void USBJoypad::readLoop()
{
  while (!exiting)
  {
    { // 
      std::lock_guard<std::mutex> lock(dataMutex);

      // test for hotplug (added/removed)
      // put data to rJoypadData

      
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
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // for testing - much less later
  }
}
//------------------------------------------------------------------------------
USBJoypad::USBJoypad()
  : exiting(false),
    lastGetTime(0)
{
  std::cout << "[INFO] JoypadDataReader - thread start" << std::endl;
  readThread = std::thread( [this]{this->readLoop();} );
  ThreadUtil::setPriority(readThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(readThread, "JoypadData (raw)");

//  temperatureFile.open("/sys/class/thermal/thermal_zone0/temp");
}
//------------------------------------------------------------------------------
USBJoypad::~USBJoypad()
{
  std::cout << "[INFO] JoypadDataReader - stop wait" << std::endl;
  exiting = true;

  if (readThread.joinable()) 
  {
    readThread.join();
  }
//  temperatureFile.close();
  std::cout << "[INFO] JoypadDataReader - stop done" << std::endl;
}
//------------------------------------------------------------------------------
//==============================================================================
