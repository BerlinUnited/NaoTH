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
//#include <Tools\ThreadUtil.h>
#include "ThreadUtil.h"
#include "USBJoypad.h"
//#include "udevIf.h"
//------------------------------------------------------------------------------
using namespace naoth;
using namespace std;
//------------------------------------------------------------------------------
void USBJoypad::get(USBJoypadData& data)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    data=dataJoypad;
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
      // put data to dataJoypad
      // [data read from joystick] >> dataJoypad.vJoypadInputData
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(250)); // for testing - adjust later
  }
}
//------------------------------------------------------------------------------
USBJoypad::USBJoypad()
  : exiting(false),
    lastGetTime(0)
{
  std::cout << "[JoypadDataReader] thread start" << std::endl;
  readThread = std::thread( [this]{this->readLoop();} );
  ThreadUtil::setPriority(readThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(readThread, "JoypadData (raw)");

}
//------------------------------------------------------------------------------
USBJoypad::~USBJoypad()
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
