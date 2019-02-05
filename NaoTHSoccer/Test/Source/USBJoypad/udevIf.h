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
class UDevInterface
{
private:
  struct udev* pUDev=nullptr;
  struct udev_monitor* pUDevMonitor=nullptr;
  int udevError;
  
  std::thread thrMon;
  bool isMonitoring;
  int deviceChangeStatus;
//  int hidStatus;

/*
  struct udev_enumerate* pDeviceEnum=nullptr;

  struct udev_list_entry* pDeviceListHead=nullptr;
  struct udev_list_entry* pDeviceListEntry=nullptr;
  const char* pDeviceName= nullptr;
  struct udev_device* pHIDeviceRaw=nullptr;
  struct udev_device* pParentDeviceRaw=nullptr;
  const char* pHIDDevNode=nullptr;
  const char* pHIDSysName=nullptr;
*/

  int initUDev();
  int initMonitor();
  int hotplugMonitor();
public:
//  bool isNodeExisting(int node);
//  int GetDeviceDataFromHIDId(JoypadDefaultData& rJoypadData, const char* const pHIDId);
  int startMonitoring();
  int stopMonitoring();
  UDevInterface();
  ~UDevInterface();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _UDEVIF_H
