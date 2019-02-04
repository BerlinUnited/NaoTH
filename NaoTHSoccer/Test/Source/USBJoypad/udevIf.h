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
#include <libudev.h>
#include "configs.h"
//--------------------------------------------------------------------------------------------------
class UDevInterface
{
private:
  int udevError;
  struct udev* pUDev=nullptr;
  struct udev_enumerate* pDeviceEnum=nullptr;
  struct udev_list_entry* pDeviceListHead=nullptr;
  struct udev_list_entry* pDeviceListEntry=nullptr;
  const char* pDeviceName= nullptr;
  struct udev_device* pHIDeviceRaw=nullptr;
  struct udev_device* pParentDeviceRaw=nullptr;
  const char* pHIDDevNode=nullptr;
  const char* pHIDSysName=nullptr;
  struct udev_monitor* pUDevMonitor=nullptr;
  int udevMonitorDescriptor;
  
  int InitUDev();
  int InitMonitor();
public:
  bool isDeviceStatusChanged();
  bool isNodeExisting(int node);
  int GetDeviceDataFromHIDId(JoypadDefaultData& rJoypadData, const char* const pHIDId);
  int StartMonitoring();
  int StopMonitoring();
  UDevInterface();
  ~UDevInterface();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _UDEVIF_H
