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
#include <sys/epoll.h>
#include <sys/file.h>
//#include "Tools/ThreadUtil.h"
#include "ThreadUtil.h"
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
#define HANDLE_INVALID  (-1)
//--------------------------------------------------------------------------------------------------
int UDevInterface::initUDev()
{
  pUDev=udev_new();
  if (pUDev == NULL)
  {
    udevError=UDEV_NO_UDEV;
    fprintf(stderr, "[UDev ERROR] Unable to create udev object...\n");
    fprintf(stderr, "[UDev ERROR] Can't connect to a joypad - exiting!\n");
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::initMonitor()
{
  // Setup a monitor on hidraw devices
  // Create new udev monitor and connect to a specified event source. 
  // Valid sources identifiers are "udev" and "kernel"
  pUDevMonitor=udev_monitor_new_from_netlink(pUDev, ID_MONITOR_UDEV);
  if (pUDevMonitor == NULL)
  {
    udevError=UDEV_NO_MONITOR;
    fprintf(stderr, "[UDev ERROR] Unable to create udev monitor...\n");
    fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n");
    return -1;
  }
  int filterResult;
  filterResult=udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  if (filterResult < 0)
  {
    udevError=UDEV_NO_FILTER;
    fprintf(stderr, "[UDev ERROR] Unable to create udev monitor filter...\n");
    fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n");
  }
  return filterResult;
}
//--------------------------------------------------------------------------------------------------
/*
bool UDevInterface::isNodeExisting(int node)
{
  if (udevError != UDEV_NOERROR)
  {
    return false;
  }
  // more code
  return false;
}
*/
//--------------------------------------------------------------------------------------------------
/*
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
//
  udev_list_entry_foreach(pDeviceListEntry, pDeviceListHead)
/*//*
  for (pDeviceListEntry = pDeviceListHead; pDeviceListEntry != NULL;
       pDeviceListEntry = udev_list_entry_get_next(pDeviceListEntry))
*//*
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
*/
//--------------------------------------------------------------------------------------------------
int UDevInterface::hotplugMonitor()
{
  const int pollTime_ms=500;
  int fd_udevMon;
  int fd_epoll;
  int resultPollCtl;
  int fdcount;
  epoll_event ev_epoll_udev ={0};
  epoll_event evReturned;
  udev_device* pReportingDevice=nullptr;

  udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the monitor. This descriptor will get passed to epoll)
  fd_udevMon=udev_monitor_get_fd(pUDevMonitor);

  fd_epoll=epoll_create1(EPOLL_CLOEXEC);
  if (fd_epoll < 0)
  {
    fprintf(stderr, "[UDevMon ERROR] Error creating epoll.\n");
    udevError=UDEV_NO_MONITOR;
    return errno;
  }
  ev_epoll_udev.events=EPOLLIN;
  ev_epoll_udev.data.fd=fd_udevMon;

  resultPollCtl=epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd_udevMon, &ev_epoll_udev);
  if (resultPollCtl < 0)
  {
    fprintf(stderr, "[UDevMon ERROR] Failed to add fd to epoll.\n");
    udevError=UDEV_NO_MONITOR;
    resultPollCtl=errno;
    close(fd_epoll);
    return resultPollCtl;
  }

  isMonitoring=true;
  while (isMonitoring)
  {
    fdcount = epoll_wait(fd_epoll, &evReturned, 1, pollTime_ms);
    if (fdcount == 0)
    {
//      fprintf(stderr, ".");
      continue;
    }
    if (fdcount > 0)
    {
      if ((evReturned.data.fd == fd_udevMon) && (evReturned.events & EPOLLIN))
      {
        pReportingDevice=udev_monitor_receive_device(pUDevMonitor);
        if (pReportingDevice == NULL)
        {
          fprintf(stderr, "[UDevMon ERROR] udev_monitor_receive_device returned NULL!\n");
        }
        else
        {
          deviceChangeStatus=1;
          udev_device_unref(pReportingDevice);
        }
      }
      else
      {
        fprintf(stderr, "[UDevMon ERROR] Something unknown returned!\n");
      }
      continue;
    }
    if (fdcount < 0)
    {
      fprintf(stderr, "[UDevMon ERROR] epoll_wait returned error %i!", errno);
      fprintf(stderr, "Press [ENTER]"); getchar();
      //    if (errno != EINTR)
      //    {
      //    fprintf(stderr, "error receiving uevent message: %m\n");
      //    }
    }
//    fprintf(stderr, ".");
//    std::this_thread::sleep_for(std::chrono::nanoseconds(200000000));
  }
  close(fd_epoll);
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::startMonitoring()
{
  if (udevError > 0)
  {
    return -udevError;
  }
  thrMon=std::thread(&UDevInterface::hotplugMonitor, this);
  naoth::ThreadUtil::setPriority(thrMon, naoth::ThreadUtil::Priority::lowest);
  naoth::ThreadUtil::setName(thrMon, "hidMonitor");

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::stopMonitoring()
{
  if (udevError > 0)
  {
    return -udevError;
  }
  isMonitoring=false;
  if (thrMon.joinable())
  {
    thrMon.join();
  }
  std::cout << "after joinable" << std::endl;

  return 0;
}
//--------------------------------------------------------------------------------------------------
UDevInterface::UDevInterface()
  : udevError(UDEV_NOERROR), deviceChangeStatus(0)
{
  std::cout << "UDev constructor called!" << std::endl;
  initUDev();
  if (udevError == UDEV_NOERROR)
    initMonitor();
}
//--------------------------------------------------------------------------------------------------
UDevInterface::~UDevInterface()
{
  std::cout << "UDev destructor called!" << std::endl;

  if ((udevError & UDEV_NO_FILTER) == 0)
  {
    udev_monitor_filter_remove(pUDevMonitor);
  }
  if ((udevError & UDEV_NO_MONITOR) == 0)
  {
    udev_monitor_unref(pUDevMonitor);
  }
  if (udevError == 0)
  {
    udev_unref(pUDev);
  }
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
        