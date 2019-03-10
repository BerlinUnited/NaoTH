//--------------------------------------------------------------------------------------------------
//
// @file Joypad.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// controlling Joypads using udev functions - header
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _JOYPAD_H
#define _JOYPAD_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <thread>
#include <mutex>
#include <libudev.h>
#include "USBJoypadConfig.h"
//--------------------------------------------------------------------------------------------------
class Joypad
{
private:
  struct udev* pUDev{};
  struct udev_monitor* pUDevMonitor{};
  int udevError{};
  bool isDeviceReadyForRead{false};
  //  JoypadProperties propsJoypad;
  naoth::joypads::SupportedJoypad supportedJoypad;
  std::string nodeDevice;
  int fdJoypadDevice{};
  int fdPollPipe[2]{}; // 0, 0

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
  enum controlType{CTRL_READ_STOP=2, CTRL_READ_START, CTRL_PLUG_STOP=8, CTRL_PLUG_START=12};
  int set(controlType);
//  void get(naoth::JoypadData& dataJoypad);
  void get(naoth::JoypadData& dataJoypad);
  Joypad();
  ~Joypad();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _JOYPAD_H
