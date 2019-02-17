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
  bool isDeviceReadyForRead;
  JoypadProperties propsJoypad;
  int fdJoypadDevice;
  int fdPollPipe[2];

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
  enum controlType {CTRL_READ_STOP=2, CTRL_READ_START, CTRL_PLUG_STOP=8, CTRL_PLUG_START=12};
  int set(controlType);
  void get(naoth::USBJoypadData& dataJoypad);
  USBJoypad();
  ~USBJoypad();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _USBJOYPAD_H
