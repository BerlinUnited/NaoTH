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
//#include "../../../../../Commons/Source/Tools/ThreadUtil.h"
#include "Tools/ThreadUtil.h"
#include "udevIf.h"
//--------------------------------------------------------------------------------------------------
//Valid monitor sources identifiers are "udev" and "kernel"
#define ID_MONITOR_UDEV  "udev"
#define SUBSYSTEM_HIDRAW "hidraw"
// UDev error codes
#define UDEV_NOERROR      0x0000
#define UDEV_UNKNOWN      0xFFFF
#define UDEV_PLUG_NO_DEV  0x0008
#define UDEV_PLUG_NO_MON  0x0002
#define UDEV_PLUG_NO_FLT  0x0001

#define UDEV_READ_NO_POLL 0x0080
//#define UDEV_READ_NO_MON  0x0040
//#define UDEV_READ_NO_FLT  0x0010
//--------------------------------------------------------------------------------------------------
int UDevInterface::initUDev()
{
  pUDev=udev_new();
  if (pUDev == NULL)
  {
    udevError|=UDEV_PLUG_NO_DEV;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev object...\n");
    std::fprintf(stderr, "[UDev ERROR] Can't connect to a joypad - exiting!\n");
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::initMonitor()
{
  // Setup a monitor on hidraw devices
  // Create new udev monitor and connect to a specified event source. 
  pUDevMonitor=udev_monitor_new_from_netlink(pUDev, ID_MONITOR_UDEV);
  if (pUDevMonitor == NULL)
  {
    udevError|=UDEV_PLUG_NO_MON;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor...\n");
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n");
    return -1;
  }
  int filterResult;
  filterResult=udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  if (filterResult < 0)
  {
    udevError|=UDEV_PLUG_NO_FLT;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor filter...\n");
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n");
  }
  return filterResult;
}
//--------------------------------------------------------------------------------------------------
void debugPrint() // errno
{
  // process special events
  switch (errno)
  {
  case SIGHUP:
    std::fprintf(stderr, "SIGHUP\n");
    break;
  case SIGINT:
    std::fprintf(stderr, "SIGINT\n");
    break;
  case SIGQUIT:
    std::fprintf(stderr, "SIGQUIT\n");
    break;
  case SIGILL:
    std::fprintf(stderr, "SIGILL\n");
    break;
  case SIGTRAP:
    std::fprintf(stderr, "SIGTRAP\n");
    break;
  case SIGABRT:
    std::fprintf(stderr, "SIGABRT\n");
    break;
  case SIGBUS:
    std::fprintf(stderr, "SIGBUS\n");
    break;
  case SIGFPE:
    std::fprintf(stderr, "SIGFPE\n");
    break;
  case SIGKILL:
    std::fprintf(stderr, "SIGKILL\n");
    break;
  case SIGUSR1:
    std::fprintf(stderr, "SIGUSR1\n");
    break;
  case SIGSEGV:
    std::fprintf(stderr, "SIGSEGV\n");
    break;
  case SIGUSR2:
    std::fprintf(stderr, "SIGUSR2\n");
    break;
  case SIGPIPE:
    std::fprintf(stderr, "SIGPIPE\n");
    break;
  case SIGALRM:
    std::fprintf(stderr, "SIGALRM\n");
    break;
  case SIGTERM:
    std::fprintf(stderr, "SIGTERM\n");
    break;
  case SIGSTKFLT:
    std::fprintf(stderr, "SIGSTKFLT\n");
    break;
  case SIGCHLD:
    std::fprintf(stderr, "SIGCHLD\n");
    break;
  case SIGCONT:
    std::fprintf(stderr, "SIGCONT\n");
    break;
  case SIGSTOP:
    std::fprintf(stderr, "SIGSTOP\n");
    break;
  case SIGTSTP:
    std::fprintf(stderr, "SIGTSTP\n");
    break;
  case SIGTTIN:
    std::fprintf(stderr, "SIGTTIN\n");
    break;
  case SIGTTOU:
    std::fprintf(stderr, "SIGTTOU\n");
    break;
  case SIGURG:
    std::fprintf(stderr, "SIGURG\n");
    break;
  case SIGXCPU:
    std::fprintf(stderr, "SIGXCPU\n");
    break;
  case SIGXFSZ:
    std::fprintf(stderr, "SIGXFSZ\n");
    break;
  case SIGVTALRM:
    std::fprintf(stderr, "SIGVTALRM\n");
    break;
  case SIGPROF:
    std::fprintf(stderr, "SIGPROF\n");
    break;
  case SIGWINCH:
    std::fprintf(stderr, "SIGWINCH\n");
    break;
  case SIGPOLL:
    std::fprintf(stderr, "SIGPOLL\n");
    break;
  case SIGPWR:
    std::fprintf(stderr, "SIGPWR\n");
    break;
  case SIGSYS:
    std::fprintf(stderr, "SIGSYS\n");
    break;
  default:
    std::fprintf(stderr, "[errno] Unknown error\n");
  }
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::dataReader(const int fdHIDOpen, const int fdPollResult)
{
  const int maxLen=50;
  std::vector<unsigned char> data(maxLen);
  ssize_t bytesRead; // int on x86 long int on x64

  if (fdPollResult > 0)
  {
    bytesRead=read(fdHIDOpen, &data[0], data.size());
// VS debug problem - keep in one line - this is only needed for debugging anyway
    for (int iter=0; iter < bytesRead; iter++) std::fprintf(stderr, "%02X ", data.at(iter)); std::cerr << std::endl;
    // cmp bytesRead with default data len for device
    // if (bytesRead == ...)
    // {
    // put data to device field
  }
  else
  { // fdPollResult < 0; 0 is covered by readLoop
    std::fprintf(stderr, "fdPollResult error: %4i\n", fdPollResult);
    debugPrint(); // errno
    bytesRead=0;
  }
  return bytesRead;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::readLoop(const int fdHIDOpen)
{
  const int pollTimeRead_ms=1000;
  int fdEpollRead;
  int resultPollCtl;
  int fdReturnedCount; // number of file descriptors returned
  epoll_event evEpollRead;
  memset(&evEpollRead, 0, sizeof(evEpollRead));
  epoll_event evReturned;

  fdEpollRead=epoll_create1(EPOLL_CLOEXEC);
  if (fdEpollRead < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Error creating epoll!\n");
    udevError|=UDEV_READ_NO_POLL;
    return errno;
  }
  evEpollRead.events=EPOLLIN;
  evEpollRead.data.fd=fdHIDOpen;
  resultPollCtl=epoll_ctl(fdEpollRead, EPOLL_CTL_ADD, fdHIDOpen, &evEpollRead);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Failed to add fd to epoll!\n");
    udevError=UDEV_READ_NO_POLL;
    resultPollCtl=errno;
    close(fdEpollRead);
    return resultPollCtl;
  }
  int loop=100;
  while (loop > 0)
  {
    std::fprintf(stderr, "%4i\n", loop); loop--; // debug msg
    fdReturnedCount=epoll_pwait(fdEpollRead, &evReturned, 1, pollTimeRead_ms, NULL);
    if (fdReturnedCount > 0)
    {
      //int xxx=(evReturned.events & 0xFFFFFFFE);
        switch (evReturned.events)
      {
      case EPOLLIN:
        std::fprintf(stderr, "%08X: EPOLLIN.\n", evReturned.events); // debug msg 
        dataReader(fdHIDOpen, fdReturnedCount);
        break;
      case EPOLLERR:
        std::fprintf(stderr, "%08X: EPOLLERR.\n", evReturned.events); // debug msg
        sleep(1);
        break;
      case EPOLLHUP:
        std::fprintf(stderr, "%08X: EPOLLHUP.\n", evReturned.events); // debug msg 
        sleep(1);
        break;
      case EPOLLHUP | EPOLLERR:
        std::fprintf(stderr, "%08X: EPOLLHUP | EPOLLERR.\n", evReturned.events); // debug msg 
        sleep(1);
        break;
      default:
        std::fprintf(stderr, "%08X: unknown EPOLL result.\n", evReturned.events); // debug msg 
        sleep(1);
      }
    }
    else
    {
      if (fdReturnedCount == 0)
      { // timed out reading or error - 
        std::fprintf(stderr, "Read Timeout.\n"); // debug msg
      }
      else
      {
        std::fprintf(stderr, "fdReturnedCount < 0!\n"); // debug msg
      }
    }
  } // while
  resultPollCtl=epoll_ctl(fdEpollRead, EPOLL_CTL_DEL, fdHIDOpen, &evEpollRead);
  close(fdEpollRead);

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::startReading()
{
  // open file for reading
  // fd=open(node, O_RDONLY | O_CLOEXEC[, permissions]) file for reading
  // file descriptor returned to read ...
  // call the read loop thread

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::stopReading()
{
  // close(fd) close the file descriptor

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::updateDeviceList()
{
  // test for plug event - done - called only when plug event 

  std::fprintf(stderr, "Our device is affected!\n");
  // if our pHIDDevNode == nullptr



  // if plugevent test if our device is already present and reading
  // return if so



  // if not: find if our device is connected

  // Create a list of the devices in the 'hidraw' subsystem
//  pDeviceEnum=udev_enumerate_new(pUDev);



  /*
  int UDevInterface::GetDeviceDataFromHIDId(JoypadDefaultData& rJoypadData, const char* const pHIDId)
  {
  int EnumResult;
  int EnumScanResult;

  std::string Property1;
  std::string Property2;


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


  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::hotplugMonitor()
{
  const int pollTimeMon_ms=500;
  int fdMonitor;
  int fdEpollPlug;
  int resultPollCtl;
  int fdCount; // number of file descriptors returned
  epoll_event evEpollPlug;
  memset(&evEpollPlug, 0, sizeof(evEpollPlug));
  epoll_event evReturned;
  udev_device* pReportingDevice=nullptr;
  std::string devNodeReportingDevice;

  udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the monitor. This descriptor will get passed to epoll)
  fdMonitor=udev_monitor_get_fd(pUDevMonitor);

  fdEpollPlug=epoll_create1(EPOLL_CLOEXEC);
  if (fdEpollPlug < 0)
  {
    std::fprintf(stderr, "[UDevMon ERROR] Error creating epoll!\n");
    udevError|=UDEV_PLUG_NO_MON;
    return errno;
  }
  evEpollPlug.events=EPOLLIN;
  evEpollPlug.data.fd=fdMonitor;

  resultPollCtl=epoll_ctl(fdEpollPlug, EPOLL_CTL_ADD, fdMonitor, &evEpollPlug);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[UDevMon ERROR] Failed to add fd to epoll!\n");
    udevError|=UDEV_PLUG_NO_MON;
    resultPollCtl=errno;
    close(fdEpollPlug);
    return resultPollCtl;
  }

  isMonitoring=true;
  while (isMonitoring)
  {
    fdCount=epoll_pwait(fdEpollPlug, &evReturned, 1, pollTimeMon_ms, NULL);
    if (fdCount == 0)
    {
      std::fprintf(stderr, ".");
      continue;
    }
    if (fdCount > 0)
    {
      if ((evReturned.data.fd == fdMonitor) && (evReturned.events & EPOLLIN))
      {
        pReportingDevice=udev_monitor_receive_device(pUDevMonitor);
        if (pReportingDevice == NULL)
        {
          std::fprintf(stderr, "[UDevMon ERROR] udev_monitor_receive_device returned NULL!\n");
        }
        else
        {
          devNodeReportingDevice.assign(udev_device_get_devnode(pReportingDevice));
          std::fprintf(stderr, "\nDC -> ");
          std::fprintf(stderr, devNodeReportingDevice.data());
          std::fprintf(stderr, "\n");
          udev_device_unref(pReportingDevice);
          if ((pHIDDevNode == nullptr) || (devNodeReportingDevice == pHIDDevNode))
          {
            updateDeviceList();
          }
        }
      }
      else
      {
        std::fprintf(stderr, "[UDevMon ERROR] Unexpected file descriptor or event returned!\n");
      }
      continue;
    }
    if (fdCount < 0)
    {
      std::fprintf(stderr, "[UDevMon ERROR] epoll_pwait returned error %i!", errno);
//      std::fprintf(stderr, "Press [ENTER]"); getchar();
      //    if (errno != EINTR)
      //    {
      //    std::fprintf(stderr, "error receiving uevent message: %m\n");
      //    }
    }
  }
  epoll_ctl(fdEpollPlug, EPOLL_CTL_DEL, fdMonitor, &evEpollPlug);
  close(fdEpollPlug);
  std::fprintf(stderr, "[UDevMon Info] Thread ends.\n");
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
  naoth::ThreadUtil::setName(thrMon, "hidHotplugMon");

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

  return 0;
}
//--------------------------------------------------------------------------------------------------
UDevInterface::UDevInterface()
  : udevError(UDEV_NOERROR)
//  , deviceChanged(ATOMIC_FLAG_INIT) // set to false (atomic)
{
  initUDev();
  if (udevError == UDEV_NOERROR)
  {
    initMonitor();
  }
}
//--------------------------------------------------------------------------------------------------
UDevInterface::~UDevInterface()
{
  if ((udevError & UDEV_PLUG_NO_MON) == 0)
  {
    udev_monitor_filter_remove(pUDevMonitor);
  }
  if ((udevError & UDEV_PLUG_NO_MON) == 0)
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
        