//--------------------------------------------------------------------------------------------------
//
// @file udevIf.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Interface to udev functions - header
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _UDEVIF_H
#define _UDEVIF_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <thread>
#include <libudev.h>
#include "configs.h"
//--------------------------------------------------------------------------------------------------
class UDevInterface
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
public:
  int startHIDPlugDetection();
  int stopHIDPlugDetection();
  UDevInterface();
  ~UDevInterface();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _UDEVIF_H
