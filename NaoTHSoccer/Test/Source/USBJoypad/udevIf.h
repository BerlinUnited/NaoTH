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
#define SUBSYSTEM_HIDRAW "hidraw"
//Valid monitor sources identifiers are "udev" and "kernel"
#define ID_UDEVMONITOR   "udev"
//#define DEVICETYPE_USB "usb"
//--------------------------------------------------------------------------------------------------
class clUDevInterface
{
private:
  int isUDevInitialized;
  int isUDevMonInitialized;
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
  int UDevMonitorDescriptor;
  
  int InitUDev();
  int InitMonitor();
public:
  int GetDeviceDataFromHIDId(sctJoypadData& JoypadData, const char* const pHIDId);
  int StartMonitoring();
  int StopMonitoring();
  clUDevInterface();
  ~clUDevInterface();
};
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _UDEVIF_H
