//--------------------------------------------------------------------------------------------------
//
// @file USBJoypad.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Interface to udev functions - header
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _USBJOYPAD_H
#define _USBJOYPAD_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <thread>
#include <mutex>
#include <libudev.h>
#include "Representations/Infrastructure/USBJoypadData.h"
#include "configs.h"
//--------------------------------------------------------------------------------------------------
class USBJoypad
{
private:
  struct udev* pUDev=nullptr;
  struct udev_monitor* pUDevMonitor=nullptr;
  int udevError;
  bool detectingHIDPlug;
  bool isDeviceReadyForRead;
  JoypadProperties propsJoypad;
  int fdJoypadDevice;

  std::thread thrPlug;
  std::thread thrRead;
  std::mutex dataMutex;

  int initUDev();
  int deinitUDev();
  int initMonitor();
  int deinitMonitor();

  int dataReader();
  void loopRead();
  int startReading();
  int stopReading();
  int updateDeviceList();
  void loopPlug();
  int startHIDPlugDetection();
  int stopHIDPlugDetection();
public:
  void get(naoth::USBJoypadData& dataJoypad);
  USBJoypad();
  ~USBJoypad();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _USBJOYPAD_H
