//--------------------------------------------------------------------------------------------------
//
// @file udevInterface.h
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
#define HANDLE_INVALID  (-1)
//--------------------------------------------------------------------------------------------------
class UDevInterface
{
private:
  struct udev* pUDev=nullptr;
  struct udev_monitor* pUDevMonitor=nullptr;
  int udevError;
  
  std::thread thrMon;
  bool isMonitoring;
//  std::atomic_flag deviceChanged;
  struct udev_device* pHIDeviceRaw=nullptr;
//  struct udev_device* pParentDeviceRaw=nullptr;
  const char* pHIDDevNode=nullptr;

/*
  struct udev_enumerate* pDeviceEnum=nullptr;

  struct udev_list_entry* pDeviceListHead=nullptr;
  struct udev_list_entry* pDeviceListEntry=nullptr;
  const char* pDeviceName= nullptr;
  const char* pHIDDevNode=nullptr;
  const char* pHIDSysName=nullptr;
*/

  int initUDev();
  int initMonitor();

  int updateDeviceList();
  int hotplugMonitor();
  int dataReader(const int fdHIDOpen, const int fdPollResult);
//  int readLoop();
public:
  int startReading();
  int stopReading();
  int startMonitoring();
  int stopMonitoring();
  //  bool isNodeExisting(int node);
  //  int GetDeviceDataFromHIDId(JoypadDefaultData& rJoypadData, const char* const pHIDId);
  UDevInterface();
  ~UDevInterface();

// for debugging only!!!
public:
  int readLoop(const int);
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _UDEVIF_H
