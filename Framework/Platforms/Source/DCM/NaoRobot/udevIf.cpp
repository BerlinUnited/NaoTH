//--------------------------------------------------------------------------------------------------
//
// @file udevIf.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Interface to udev functions
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <sys/epoll.h>
#include <sys/file.h>
#include "Tools/ThreadUtil.h"
#include "udevIf.h"
//--------------------------------------------------------------------------------------------------
#define FD_INVALID        -1
//Valid monitor sources identifiers are "udev" and "kernel"
#define ID_MONITOR_UDEV   "udev"
#define SUBSYSTEM_HIDRAW  "hidraw"
// UDev error codes
#define UDEV_NOERROR      0x0000
#define UDEV_UNKNOWN      0xFFFF
#define UDEV_PLUG_NO_DEV  0x0008
#define UDEV_PLUG_NO_MON  0x0002
#define UDEV_PLUG_NO_FLT  0x0001

#define UDEV_READ_NO_POLL 0x0080
//#define UDEV_READ_NO_MON  0x0040
//#define UDEV_READ_NO_FLT  0x0010


void UDevInterface::get(naoth::USBJoypadData& data)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);

  if (lock.owns_lock())
  {
    data.vJoypadInputData = propsJoypad.inputReportData;
  }
}

//--------------------------------------------------------------------------------------------------
int UDevInterface::initUDev()
{
  pUDev=udev_new();
  if (pUDev == NULL)
  {
    udevError|=UDEV_PLUG_NO_DEV;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev object...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Can't connect to a joypad - exiting!\n"); // error msg
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::deinitUDev()
{
  if (udevError == 0)
  {
    udev_unref(pUDev);
  }
  return 0;
}
  //--------------------------------------------------------------------------------------------------
int UDevInterface::initMonitor()
{
  // Setup a monitor on hidraw devices
  // Create new udev monitor and connect to the specified event source. 
  pUDevMonitor=udev_monitor_new_from_netlink(pUDev, ID_MONITOR_UDEV);
  if (pUDevMonitor == NULL)
  {
    udevError|=UDEV_PLUG_NO_MON;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n"); // error msg
    return -1;
  }
  
  // configure the monitor to listen to hidraw
  int filterResult = udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  if (filterResult < 0)
  {
    udevError|=UDEV_PLUG_NO_FLT;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor filter...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n"); // error msg
  }
  return filterResult;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::deinitMonitor()
{
  if ((udevError & UDEV_PLUG_NO_FLT) == 0)
  {
    udev_monitor_filter_remove(pUDevMonitor);
  }
  if ((udevError & UDEV_PLUG_NO_MON) == 0)
  {
    udev_monitor_unref(pUDevMonitor);
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int UDevInterface::dataReader()
{
  std::lock_guard<std::mutex> lock(dataMutex);
  
  constexpr int maxDataLen=27; // longest known is 27 so far

  ssize_t bytesRead;

  bytesRead=read(fdJoypadDevice, &propsJoypad.inputReportData[0], maxDataLen);
  if (bytesRead == propsJoypad.inputReportLen)
  { // read ok
    // VS debug problem - keep in one line - this is intended and only needed for step debugging
    for (int i=0; i < bytesRead; i++) std::fprintf(stderr, "%02X ", propsJoypad.inputReportData.at(i)); std::fprintf(stderr, "\n"); // debug msg
  }
  else
  {
    std::fprintf(stderr, "[dataReader Info] Read %04i bytes, should have read %04i bytes.\n", bytesRead, propsJoypad.inputReportLen); // error msg
    std::fprintf(stderr, "[dataReader Info] Error code is %03i.\n", errno);
    bytesRead=-1;
  }

  return bytesRead;
}
//--------------------------------------------------------------------------------------------------
void UDevInterface::loopRead()
{
  constexpr int timeReadPoll_ms=1000;

  int fdepRead;
  int resultPollCtl;
  int fdReturnedCount; // number of file descriptors returned from epoll_wait
  epoll_event evEpollRead;
  epoll_event evReturned;
  int resultDataReader;

  std::fprintf(stderr, "[UDevRead Info] Starting the read loop thread.\n"); // debug msg
  fdJoypadDevice=open(propsJoypad.deviceNode.c_str(), O_RDONLY | O_CLOEXEC);
  if (fdJoypadDevice == FD_INVALID)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Open failed with error #%3i\n", errno); // error msg
    return;
  }
  fdepRead=epoll_create1(EPOLL_CLOEXEC);
  if (fdepRead < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Error creating epoll!\n"); // error msg
    udevError|=UDEV_READ_NO_POLL;
    return;
  }
  evEpollRead.events=EPOLLIN;
  evEpollRead.data.fd=fdJoypadDevice;
  resultPollCtl=epoll_ctl(fdepRead, EPOLL_CTL_ADD, fdJoypadDevice, &evEpollRead);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Failed to add fd to epoll_ctl!\n"); // error msg
    udevError=UDEV_READ_NO_POLL;
    close(fdepRead);
    return;
  }
  isDeviceReadyForRead=true;
  while (isDeviceReadyForRead)
  {
    fdReturnedCount=epoll_pwait(fdepRead, &evReturned, 1, timeReadPoll_ms, NULL);
    if (fdReturnedCount > 0)
    {
      resultDataReader=dataReader();
      std::fprintf(stderr, "[UDevRead Info] dataReader returned: %04i.\n", resultDataReader); // debug msg
      if (resultDataReader < 0)
      { // reading failed with error - stop reader loop
        isDeviceReadyForRead=false;
      }
    }
    else
    {
      if (fdReturnedCount == 0)
      { // timed out reading
// debug msg start
        std::fprintf(stderr, "[UDevRead Info] EPoll timeout waiting for data.\n");
        // VS debug problem - keep in one line - this is intended and only needed for step debugging
        for (auto i=0; i < propsJoypad.inputReportLen; i++) std::fprintf(stderr, "%02X ", propsJoypad.inputReportData.at(i)); std::fprintf(stderr, "\n");
// debug msg end
      }
      else
      {
        switch (errno)
        {
        case EINTR:
          // received SIGINT
          break;
        default:
          std::fprintf(stderr, "[UDevRead ERROR] returned error %i!", errno); // error msg
        }
      }
    }
  } // while (isDeviceReadyForRead)
  // stop getting notifications about read
  resultPollCtl=epoll_ctl(fdepRead, EPOLL_CTL_DEL, fdJoypadDevice, &evEpollRead);
  close(fdepRead);
  close(fdJoypadDevice);

  std::fprintf(stderr, "[UDevRead Info] loopRead thread ended.\n"); // debug msg
  return;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::startReading()
{
  if (propsJoypad.deviceNode.empty())
  { // no valid device - nothing to read
    return -1;
  }
  thrRead=std::thread([this]{UDevInterface::loopRead();});
  naoth::ThreadUtil::setPriority(thrRead, naoth::ThreadUtil::Priority::lowest);
  naoth::ThreadUtil::setName(thrRead, "HIDDataReader");

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::stopReading()
{
  isDeviceReadyForRead=false;
  if (thrRead.joinable())
  {
    thrRead.join();
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::updateDeviceList()
{
  udev_enumerate* pDeviceEnum=nullptr;
  udev_list_entry* pDeviceListHead=nullptr;
  udev_list_entry* pDeviceListEntry=nullptr;
  const char* pDeviceName=nullptr;
  udev_device* pUDevDevice=nullptr;
  udev_device* pParentDevice=nullptr;
  std::string hidIdFromList;
  int updateListError=0;
  int foundConfigForDevice=0;
  // Create a list of the devices in the 'hidraw' subsystem
  pDeviceEnum=udev_enumerate_new(pUDev);
  if (pDeviceEnum == NULL)
  {
    std::fprintf(stderr, "[DeviceList ERROR] Couldn't create a pointer for device enumeration!\n"); // error msg
    updateListError=-1;
  }
  else
  {
    updateListError=udev_enumerate_add_match_subsystem(pDeviceEnum, SUBSYSTEM_HIDRAW);
    if (updateListError < 0)
    {
      std::fprintf(stderr, "[DeviceList ERROR] Couldn't enumerate subsystem!\n"); // error msg
    }
    else
    {
      updateListError=udev_enumerate_scan_devices(pDeviceEnum);
      if (updateListError < 0)
      {
        std::fprintf(stderr, "[DeviceList ERROR] Couldn't scan for devices!\n"); // error msg
      }
      else
      {
        pDeviceListHead=udev_enumerate_get_list_entry(pDeviceEnum);
        //#define udev_list_entry_foreach(list_entry, first_entry) ...
        // using the macro raises an "intelli" warning in VS (bug???)
        pDeviceListEntry=pDeviceListHead;
        propsJoypad.deviceNode.clear(); // indicate that propsJoypad is invalid
        while ((foundConfigForDevice == 0) && (pDeviceListEntry != NULL))
        {
          // Get the filename of the /sys entry for the (hid) device and create
          // a udev_device object (pUdevDevice) representing it
          // according to the manual the memory pDeviceName points to 
          // after the assignment doesn't have to explicitely be freed 
          pDeviceName=udev_list_entry_get_name(pDeviceListEntry);
          pUDevDevice=udev_device_new_from_syspath(pUDev, pDeviceName);
          pParentDevice=udev_device_get_parent(pUDevDevice);
          hidIdFromList=udev_device_get_property_value(pParentDevice, "HID_ID");
          foundConfigForDevice=getJoypadConfigFromFile(propsJoypad, hidIdFromList);
          if (foundConfigForDevice > 0)
          {
            propsJoypad.deviceNode=udev_device_get_devnode(pUDevDevice);
          }
          udev_device_unref(pUDevDevice); // unref from udev_device_new_from_syspath
          pDeviceListEntry=udev_list_entry_get_next(pDeviceListEntry);
        }
      }
    }
    udev_enumerate_unref(pDeviceEnum);
  }
  std::fprintf(stderr, "[DeviceListInfo] returning with %04i!\n", updateListError); // debug msg
  std::fprintf(stderr, "[DeviceListInfo] active device has node \"%s\".\n", propsJoypad.deviceNode.c_str()); // debug msg

  return updateListError;
}
//--------------------------------------------------------------------------------------------------
void UDevInterface::loopPlug()
{
  constexpr int timePlugPoll_ms=500;
  constexpr int timeWaitUnplug_ms=1000;

  int fdUDevMon;
  int fdepPlug;
  int resultPollCtl;
  int fdCount; // number of file descriptors returned
  epoll_event evPollPlug;
  epoll_event evReturned;
  udev_device* pReportingDevice=nullptr;
  std::string devNodeReportingDevice;

  udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the monitor. This descriptor will get passed to epoll)
  fdUDevMon=udev_monitor_get_fd(pUDevMonitor);

  // create a new poll event
  fdepPlug=epoll_create1(EPOLL_CLOEXEC);
  if (fdepPlug < 0)
  {
    std::fprintf(stderr, "[UDevMon ERROR] Error creating epoll!\n"); // error msg
    udevError|=UDEV_PLUG_NO_MON;
    return;
  }
  
  // send a poll request
  evPollPlug.events=EPOLLIN;
  evPollPlug.data.fd=fdUDevMon;
  resultPollCtl=epoll_ctl(fdepPlug, EPOLL_CTL_ADD, fdUDevMon, &evPollPlug);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[UDevMon ERROR] Failed to add fd to epoll!\n"); // error msg
    udevError|=UDEV_PLUG_NO_MON;
    close(fdepPlug);
    return;
  }

  updateDeviceList(); // in case devices are already connected upon start
  startReading();
  detectingHIDPlug=true;
  while (detectingHIDPlug)
  {
    fdCount=epoll_pwait(fdepPlug, &evReturned, 1, timePlugPoll_ms, NULL);
    if (fdCount == 0)
    {
      std::fprintf(stderr, "."); // debug msg - monitor is waiting for plug event
      continue;
    }
    if (fdCount < 0)
    {
      switch (errno)
      {
      case EINTR:
      // received SIGINT
        break;
      default:
        std::fprintf(stderr, "[UDevMon ERROR] epoll_wait returned error %i!", errno); // error msg
      }
      continue;
    }
    if (fdCount == 1)
    {
      if ((evReturned.events & EPOLLIN) && (evReturned.data.fd == fdUDevMon))
      {
        pReportingDevice=udev_monitor_receive_device(pUDevMonitor);
        if (pReportingDevice == NULL)
        {
          std::fprintf(stderr, "[UDevMon ERROR] udev_monitor_receive_device returned NULL!\n"); // error msg
        }
        else
        {
          devNodeReportingDevice=udev_device_get_devnode(pReportingDevice);
          std::fprintf(stderr, "\nDC -> %s\n", devNodeReportingDevice.c_str()); // debug msg
          udev_device_unref(pReportingDevice);
          // our node doesn't exist -> goto updateDeviceList
          // else (our node exists)
            // if returned node matches our node
              // reading ok?
                // no -> goto updateDeviceList
                // yes -> nothing to do (unlikely) ???
            // else (returned node doesn't match our node) -> nothing to do
          if (propsJoypad.deviceNode.empty())
          {
            std::fprintf(stderr, "[UDevMon Info] no device present - calling updateDeviceList.\n"); // debug msg
            updateDeviceList();
            startReading();
          }
          else
          { // we have a device node
            if (propsJoypad.deviceNode == devNodeReportingDevice) // it is our device node
            {
              usleep(1000*timeWaitUnplug_ms);
              if (isDeviceReadyForRead)
              {
                std::fprintf(stderr, "[UDevMon ERROR] isDeviceReadyForRead still true!\n"); // debug message
                std::fprintf(stderr, "[UDevMon ERROR] This situation should never occur here.\n"); // debug message
                // some error condition
                // - cause: what action to our device could have happened
                // without setting isDeviceReadyForRead to false ???
              }
              else
              {
                std::fprintf(stderr, "[UDevMon Info] can't read from node - calling updateDeviceList.\n"); // debug message
                stopReading(); // stopping the thread that still may try to read from the old node
                updateDeviceList();
                startReading();
              }
            } // if (HIDNodeName == devNodeReportingDevice) - no else needed (only for debugging)
            else
            {
              std::fprintf(stderr, "[UDevMon Info] Compare failed - not our device.\n"); // debug message
            }
          }
        }
        continue;
      }
    }
    // if (fdCount > 1)
    std::fprintf(stderr, "[UDevMon ERROR] Unexpected fd value or event returned!\n"); // error msg
    std::fprintf(stderr, "[UDevMon ERROR] This error should never occur!\n"); // error msg
  }
  epoll_ctl(fdepPlug, EPOLL_CTL_DEL, fdUDevMon, &evPollPlug);
  close(fdepPlug);
  std::fprintf(stderr, "[UDevMon Info] loopPlug thread ended.\n"); // debug msg

  return;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::startHIDPlugDetection()
{
  if (udevError > 0)
  {
    return -udevError;
  }
  thrPlug=std::thread([this]{loopPlug();});
  naoth::ThreadUtil::setPriority(thrPlug, naoth::ThreadUtil::Priority::lowest);
  naoth::ThreadUtil::setName(thrPlug, "HIDHotplug");

  return 0;
}
//--------------------------------------------------------------------------------------------------
int UDevInterface::stopHIDPlugDetection()
{
  if (udevError > 0)
  {
    return -udevError;
  }
  stopReading();
  detectingHIDPlug=false;
  if (thrPlug.joinable())
  {
    thrPlug.join();
  }

  return 0;
}
//--------------------------------------------------------------------------------------------------
UDevInterface::UDevInterface()
  : udevError(UDEV_NOERROR)
  , detectingHIDPlug(false)
  , isDeviceReadyForRead(false)
{
  propsJoypad.deviceNode.clear(); // invalidate Joypad properties data
  initUDev();
  if (udevError == UDEV_NOERROR)
  {
    initMonitor();
    startHIDPlugDetection();
  }
}
//--------------------------------------------------------------------------------------------------
UDevInterface::~UDevInterface()
{
  stopHIDPlugDetection();
  deinitMonitor();
  deinitUDev();
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
        