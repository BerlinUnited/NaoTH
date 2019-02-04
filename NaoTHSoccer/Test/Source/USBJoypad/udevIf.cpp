//--------------------------------------------------------------------------------------------------
//
// @file udevInterface.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Interface to udev functions
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "udevIf.h"
//--------------------------------------------------------------------------------------------------
#define SUBSYSTEM_HIDRAW "hidraw"
//Valid monitor sources identifiers are "udev" and "kernel"
#define ID_MONITOR_UDEV  "udev"
// UDev error codes
#define UDEV_NOERROR    0x0000
#define UDEV_NO_UDEV    0x0001
#define UDEV_NO_MONITOR 0x0002
#define UDEV_NO_FILTER  0x0004
#define UDEV_UNKNOWN    0xFFFF
//--------------------------------------------------------------------------------------------------
int UDevInterface::InitUDev()
{
  pUDev=udev_new();
  if (pUDev == NULL)
  {
    udevError=UDEV_NO_UDEV;
    std::cout << "[UDev] ERROR - unable to create udev object..." << std::endl;
    std::cout << "[UDev] Can't connect to a joypad - exiting!" << std::endl;
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::InitMonitor()
{
  // Setup a monitor on hidraw devices
  // Create new udev monitor and connect to a specified event source. 
  // Valid sources identifiers are "udev" and "kernel"
  pUDevMonitor=udev_monitor_new_from_netlink(pUDev, ID_MONITOR_UDEV);
  if (pUDevMonitor == NULL)
  {
    udevError=UDEV_NO_MONITOR;
    std::cout << "[UDev ERROR] Unable to create udev monitor..." << std::endl;
    std::cout << "[UDev ERROR] Joypad may not work (no hotplug support...)!" << std::endl;
    return -1;
  }
  int filterResult;
  filterResult=udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  if (filterResult < 0)
  {
    udevError=UDEV_NO_FILTER;
    std::cout << "[UDev ERROR] Unable to create udev monitor..." << std::endl;
    std::cout << "[UDev ERROR] Joypad may not work (no hotplug support...)!" << std::endl;
  }
  return filterResult;
}
//--------------------------------------------------------------------------------------------------
bool UDevInterface::isDeviceStatusChanged()
{
  if (udevError != UDEV_NOERROR)
  {
    return false;
  }
  // more code
  return true;
}
//--------------------------------------------------------------------------------------------------
bool UDevInterface::isNodeExisting(int node)
{
  if (udevError != UDEV_NOERROR)
  {
    return false;
  }
  // more code
  return false;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::GetDeviceDataFromHIDId(JoypadDefaultData& rJoypadData, const char* const pHIDId)
{
  int EnumResult;
  int EnumScanResult;
  
  std::string Property1;
  std::string Property2;
  
  if (udevError != UDEV_NOERROR)
  {
    return -udevError;
  }
  // Create a list of the devices in the 'hidraw' subsystem
  pDeviceEnum=udev_enumerate_new(pUDev);
  EnumResult=udev_enumerate_add_match_subsystem(pDeviceEnum, SUBSYSTEM_HIDRAW);
  EnumScanResult=udev_enumerate_scan_devices(pDeviceEnum);
  pDeviceListHead=udev_enumerate_get_list_entry(pDeviceEnum);

//#define udev_list_entry_foreach(list_entry, first_entry) ...
// using the macro raises an "intelli" warning (Visual Studio 2015 bug???)
/*
  udev_list_entry_foreach(pDeviceListEntry, pDeviceListHead)
/*/
  for (pDeviceListEntry = pDeviceListHead; pDeviceListEntry != NULL;
       pDeviceListEntry = udev_list_entry_get_next(pDeviceListEntry))
//*/
  {
// Get the filename of the /sys entry for the (hid) device and create
// a udev_device object (pUdevDevice) representing it
    pDeviceName=udev_list_entry_get_name(pDeviceListEntry);
    pHIDeviceRaw=udev_device_new_from_syspath(pUDev, pDeviceName);
    pHIDDevNode=udev_device_get_devnode(pHIDeviceRaw);
    pParentDeviceRaw=udev_device_get_parent(pHIDeviceRaw);

    Property1=udev_device_get_property_value(pParentDeviceRaw, "HID_ID");
    Property2=udev_device_get_sysattr_value(pParentDeviceRaw, "modalias");
    udev_device_unref(pHIDeviceRaw); pHIDeviceRaw=nullptr;
  }

//  pSysPath=udev_get_sys_path(pUDev);
//  pDevPath=udev_get_dev_path(pUDev);

   // Free the enumerator object
  udev_enumerate_unref(pDeviceEnum); pDeviceEnum=nullptr;
  udev_unref(pUDev); pUDev=nullptr;
  
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::StartMonitoring()
{
  int ResultEnable;
  int ResultSelect;

  if ((udevError & UDEV_NO_MONITOR) > 0)
  {
    return -udevError;
  }
  ResultEnable=udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the monitor. This descriptor will get passed to select()
  udevMonitorDescriptor=udev_monitor_get_fd(pUDevMonitor);
  
  udev_device* pUDevDevice=nullptr;
  fd_set fds;
  timeval tv;
  timespec tspec;
  sigset_t SigMask;
  sigset_t SigOrigMask;
  

  tv.tv_sec=50;
  tv.tv_usec=0;
  tspec.tv_sec=0;
  tspec.tv_nsec=0;
  
  sigemptyset(&SigMask);
  sigaddset(&SigMask, SIGTERM);
  sigaddset(&SigMask, SIGINT);
  sigaddset(&SigMask, SIGUSR1);  
    
  int ii=0;
  while(ii < 200)
  {
    FD_ZERO(&fds);
    FD_SET(udevMonitorDescriptor, &fds);
    ResultSelect=pselect(udevMonitorDescriptor+1, &fds, NULL, NULL, &tspec, &SigMask);
    
    if (ResultSelect < 0)
    {
      printf("Error!!!\n");
    }
    if (ResultSelect > 0 && FD_ISSET(udevMonitorDescriptor, &fds)) 
    {
      printf("\nselect() says there should be data\n");
      // Make the call to receive the device.
      // select() ensured that this will not block.
      // udev_monitor_receive_device allocates new udev_device
      // this has to be udev_device_unref'd at end of use
      pUDevDevice=udev_monitor_receive_device(pUDevMonitor);
      if (pUDevDevice)
      {
//        printf("Got Device\n");
//        printf("   Node: %s\n", udev_device_get_devnode(pUDevDevice));
//        printf("   Subsystem: %s\n", udev_device_get_subsystem(pUDevDevice));
////        printf("   Devtype: %s\n", udev_device_get_devtype(pUDevDevice)); // this is NULL
//        printf("   Action: %s\n", udev_device_get_action(pUDevDevice));
        udev_device_unref(pUDevDevice);
      }
      else
      {
        printf("ERROR(udev_monitor_receive_device) - NULL returned.\n");
      }		
    }
    else
    {
      printf(".");
    }

    //printf("-");
    sleep(1); // compiler complains about usleep ???
    fflush(stdout);
    ii++;
  }
  
  /*

  fd+1, &fds, NULL, NULL, &tv);
*/
    

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::StopMonitoring()
{
  int RemoveResult;

  RemoveResult=udev_monitor_filter_remove(pUDevMonitor);
  return 0;
}
//--------------------------------------------------------------------------------------------------
UDevInterface::UDevInterface()
  : udevError(UDEV_NOERROR)
{
  std::cout << "UDev constructor called!" << std::endl;
  InitUDev();
  if (udevError == UDEV_NOERROR)
    InitMonitor();
}
//--------------------------------------------------------------------------------------------------
UDevInterface::~UDevInterface()
{
  std::cout << "UDev destructor called!" << std::endl;
  udev_monitor_unref(pUDevMonitor);
  udev_unref(pUDev);
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
        