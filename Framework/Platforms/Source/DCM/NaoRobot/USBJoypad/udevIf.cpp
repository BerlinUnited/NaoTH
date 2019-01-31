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
#include "udevIf.h"
#include <unistd.h>
#include <signal.h>
//--------------------------------------------------------------------------------------------------
int clUDevInterface::InitUDev()
{
  pUDev=udev_new();
  if (pUDev == 0)
  {
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int clUDevInterface::InitMonitor()
{
  int Result;
  // Setup a monitor on hidraw devices
  //Create new udev monitor and connect to a specified event source. 
  //Valid sources identifiers are "udev" and "kernel"
  pUDevMonitor=udev_monitor_new_from_netlink(pUDev, ID_UDEVMONITOR);
  
  if (pUDevMonitor == NULL)
  {
    printf("Error! Couldn't create udev monitor!\n");
    return -1;
  }
  Result=udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  return 0;
}
//--------------------------------------------------------------------------------------------------
int clUDevInterface::GetDeviceDataFromHIDId(sctJoypadData& rJoypadData, 
                                            const char* const pHIDId)
{
  int EnumResult;
  int EnumScanResult;
  
  std::string Property1;
  std::string Property2;
  
//  int handle;
//  int BytesRead;
//  unsigned char Buffer[100];
  // Create a list of the devices in the 'hidraw' subsystem
  pDeviceEnum=udev_enumerate_new(pUDev);
  EnumResult=udev_enumerate_add_match_subsystem(pDeviceEnum, SUBSYSTEM_HIDRAW);
  EnumScanResult=udev_enumerate_scan_devices(pDeviceEnum);
  pDeviceListHead=udev_enumerate_get_list_entry(pDeviceEnum);
  udev_list_entry_foreach(pDeviceListEntry, pDeviceListHead)
  {
// Get the filename of the /sys entry for the device and create
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
int clUDevInterface::StartMonitoring()
{
  int ResultEnable;
  int ResultSelect;

  ResultEnable=udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the monitor. This descriptor will get passed to select()
  UDevMonitorDescriptor=udev_monitor_get_fd(pUDevMonitor);
  
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
    FD_SET(UDevMonitorDescriptor, &fds);
    ResultSelect=pselect(UDevMonitorDescriptor+1, &fds, NULL, NULL, &tspec, &SigMask);
    
    if (ResultSelect < 0)
    {
      printf("Error!!!\n");
    }
    if (ResultSelect > 0 && FD_ISSET(UDevMonitorDescriptor, &fds)) 
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
    usleep(1000*1000);
    fflush(stdout);
    ii++;
  }
  
  /*

  fd+1, &fds, NULL, NULL, &tv);
*/
    

  return 0;
}
//--------------------------------------------------------------------------------------------------
int clUDevInterface::StopMonitoring()
{
  int RemoveResult;
  RemoveResult=udev_monitor_filter_remove(pUDevMonitor);
  return 0;
}
//--------------------------------------------------------------------------------------------------
clUDevInterface::clUDevInterface()
//: isUDevInitialized(0), isUDevMonInitialized(0)
{
  InitUDev();
  InitMonitor();
  /*
  if (InitUDev() < 0)
  {
    std::cout << "Couldn't create udev reference - exiting" << std::endl;
    exit(-1);
  }
  if (InitMonitor() < 0)
  {
    std::cout << "Couldn't create udev monitor" << std::endl;
    exit(-1);
  }
  */
}
//--------------------------------------------------------------------------------------------------
clUDevInterface::~clUDevInterface()
{
  udev_monitor_unref(pUDevMonitor);
  udev_unref(pUDev);
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
        