//--------------------------------------------------------------------------------------------------
//
// @file Joypad.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Joypad data supply for Nao
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
//#include <iostream>
//#include <iomanip>
#include <sys/epoll.h>
#include <sys/file.h>
#include "Tools/ThreadUtil.h"
#include "USBJoypad.h"
//--------------------------------------------------------------------------------------------------
#ifndef DEBUG
#define DEBUG
#endif // !DEBUG

#define FD_INVALID        -1
//Valid monitor sources identifiers are "udev" and "kernel"
#define ID_MONITOR_UDEV   "udev"
#define SUBSYSTEM_HIDRAW  "hidraw"
// UDev error codes
#define UDEV_NOERROR      0x0000
#define UDEV_UNKNOWN      0xFFFF
#define UDEV_PLUG_NO_DEV  0x0008
#define UDEV_PLUG_NO_MON  0x0004
#define UDEV_PLUG_NO_FLT  0x0002
#define UDEV_PLUG_NO_POLL 0x0001

#define UDEV_READ_NO_POLL 0x0080
//#define UDEV_READ_NO_MON  0x0040
//#define UDEV_READ_NO_FLT  0x0010
//--------------------------------------------------------------------------------------------------
int Joypad::set(controlType ctrl)
{
  switch (ctrl)
  {
  case CTRL_PLUG_START:
    startHIDPlugDetection();
    break;
  case CTRL_PLUG_STOP:
    stopHIDPlugDetection();
    break;
  case CTRL_READ_START:
    startReading();
    break;
  case CTRL_READ_STOP:
    stopReading();
    break;
  default:
    // error msg
    std::fprintf(stderr, "[Set ERROR] Unknown control type - nothing to do!\n"); // error msg
    return -1;
  }
  return ctrl;
}
//--------------------------------------------------------------------------------------------------
void Joypad::get(naoth::JoypadData& dataJoypad)
{
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    supportedJoypad.getReportAsControls(dataJoypad.controls);
  }
}
//--------------------------------------------------------------------------------------------------
int Joypad::initUDev()
{
  pUDev = udev_new();
  if (pUDev == NULL)
  {
    udevError |= UDEV_PLUG_NO_DEV;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev object...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Can't connect to a joypad - exiting!\n"); // error msg
    return -1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int Joypad::deinitUDev()
{
  if (udevError == UDEV_NOERROR)
  {
    udev_unref(pUDev);
  }
  return 0;
}
  //--------------------------------------------------------------------------------------------------
int Joypad::initMonitor()
{
  // Setup a monitor on hidraw devices
  // Create new udev monitor and connect to the specified event source. 
  pUDevMonitor = udev_monitor_new_from_netlink(pUDev, ID_MONITOR_UDEV);
  if (pUDevMonitor == NULL)
  {
    udevError |= UDEV_PLUG_NO_MON;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n"); // error msg
    return -1;
  }
  // configure the monitor to listen to hidraw
  int filterResult = udev_monitor_filter_add_match_subsystem_devtype(pUDevMonitor, SUBSYSTEM_HIDRAW, NULL);
  if (filterResult < 0)
  {
    udevError |= UDEV_PLUG_NO_FLT;
    std::fprintf(stderr, "[UDev ERROR] Unable to create udev monitor filter...\n"); // error msg
    std::fprintf(stderr, "[UDev ERROR] Joypad may not work (no hotplug support...)!\n"); // error msg
  }
  return filterResult;
}
//--------------------------------------------------------------------------------------------------
int Joypad::deinitMonitor()
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
int Joypad::dataReader()
{
  constexpr int maxDataLen = 27; // longest known is 27 so far

  std::vector<unsigned char> data(maxDataLen);
  data.reserve(maxDataLen);
  ssize_t bytesRead = read(fdJoypadDevice, &data[0], maxDataLen);

  std::lock_guard<std::mutex> lock(dataMutex);
  if ((std::size_t)bytesRead == supportedJoypad.getDefaultReportLen())
  {
    supportedJoypad.setReport({data.begin(), data.begin() + bytesRead});
  }
  else
  {
#ifdef DEBUG
    std::fprintf(stderr, "[dataReader ERROR] Read %03i bytes, should have read %03i bytes.\n",
                 bytesRead, supportedJoypad.getDefaultReportLen()); // error msg
    std::fprintf(stderr, "[dataReader ERROR] Error code is %03i.\n", errno);
#endif // DEBUG
    supportedJoypad.setReport({0xFF});
    bytesRead = -1;
  }
#ifdef DEBUG
  // VS debug problem workaround - keep in one line - this is intended and only needed for step debugging
  for (auto& i : supportedJoypad.getReportRaw()) { std::fprintf(stderr, "%02X ", i); } std::fprintf(stderr, "\n"); // debug msg
//  for (auto& i : propsJoypad.inputReportData){std::cerr << std::setfill('0') << std::setprecision(2) << std::uppercase << std::hex << (int)i << ' ';}std::cerr << std::endl;
#endif // DEBUG
  return bytesRead;
}
//--------------------------------------------------------------------------------------------------
void Joypad::loopRead()
{
  constexpr int timeReadPoll_ms = 250;

#ifdef DEBUG
  std::fprintf(stderr, "[UDevRead Info] Starting the read loop thread.\n"); // debug msg
#endif // DEBUG
  fdJoypadDevice = open(nodeDevice.c_str(), O_RDONLY | O_CLOEXEC);
  if (fdJoypadDevice == FD_INVALID)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Open failed with error #%3i\n", errno); // error msg
    return;
  }
  int fdepRead = epoll_create1(EPOLL_CLOEXEC);
  if (fdepRead < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Error creating epoll!\n"); // error msg
    udevError |= UDEV_READ_NO_POLL;
    return;
  }
  epoll_event evEpollRead;
  evEpollRead.events = EPOLLIN;
  evEpollRead.data.fd = fdJoypadDevice;
  int resultPollCtl = epoll_ctl(fdepRead, EPOLL_CTL_ADD, fdJoypadDevice, &evEpollRead);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[UDevRead ERROR] Failed to add fd to epoll_ctl!\n"); // error msg
    udevError = UDEV_READ_NO_POLL;
    close(fdepRead);
    return;
  }

  epoll_event evReturned;
  int fdReturnedCount; // number of file descriptors returned from epoll_wait
  isDeviceReadyForRead = true;
  while (isDeviceReadyForRead)
  {
    fdReturnedCount = epoll_pwait(fdepRead, &evReturned, 1, timeReadPoll_ms, NULL);
    if (fdReturnedCount > 0)
    {
      if (dataReader() < 0)
      { // reading failed with error - stop reader loop
        isDeviceReadyForRead = false;
      }
    }
#ifdef DEBUG
    else
    {
      if (fdReturnedCount == 0)
      { // timed out reading
        std::fprintf(stderr, "[UDevRead Info] EPoll timeout waiting for data.\n");
        // VS debug problem - keep in one line - this is intended and only needed for step debugging
        for (auto& i : supportedJoypad.getReportRaw()) { std::fprintf(stderr, "%02X ", i); } std::fprintf(stderr, "\n");
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
#endif // DEBUG
  } // while (isDeviceReadyForRead)
  // stop getting notifications about read
  epoll_ctl(fdepRead, EPOLL_CTL_DEL, fdJoypadDevice, &evEpollRead);
  close(fdepRead);
  close(fdJoypadDevice);
#ifdef DEBUG
  std::fprintf(stderr, "[UDevRead Info] loopRead thread ended.\n"); // debug msg
#endif // DEBUG
  return;
}
//--------------------------------------------------------------------------------------------------
int Joypad::startReading()
{
  if (nodeDevice.empty())
  { // no valid device - nothing to read
    return -1;
  }
  thrRead = std::thread([this] {loopRead(); });
  naoth::ThreadUtil::setPriority(thrRead, naoth::ThreadUtil::Priority::lowest);
  naoth::ThreadUtil::setName(thrRead, "HIDDataReader");
  return 0;
}
//--------------------------------------------------------------------------------------------------
int Joypad::stopReading()
{
  isDeviceReadyForRead = false;
  if (thrRead.joinable())
  {
    thrRead.join();
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
int Joypad::updateDeviceList()
{
  // Create a list of the devices in the 'hidraw' subsystem
  udev_enumerate* pDeviceEnum = udev_enumerate_new(pUDev);
  if (pDeviceEnum == NULL)
  {
    std::fprintf(stderr, "[DeviceList ERROR] Couldn't create a pointer for device enumeration!\n"); // error msg
    return -1;
  }
  int updateListError = udev_enumerate_add_match_subsystem(pDeviceEnum, SUBSYSTEM_HIDRAW);
  if (updateListError < 0)
  {
    std::fprintf(stderr, "[DeviceList ERROR] Couldn't enumerate subsystem!\n"); // error msg
    return updateListError;
  }
  updateListError = udev_enumerate_scan_devices(pDeviceEnum);
  if (updateListError < 0)
  {
    std::fprintf(stderr, "[DeviceList ERROR] Couldn't scan for devices!\n"); // error msg
    return updateListError;
  }

  nodeDevice.clear(); // invalidate old Joypad data
  udev_list_entry* pDeviceListEntry = udev_enumerate_get_list_entry(pDeviceEnum);

  const char* pDeviceName{nullptr};
  udev_device* pUDevDevice{nullptr};
  udev_device* pParentDevice{nullptr};
  std::string hidIdFromList;
  int foundDevices = 0;
  while ((foundDevices == 0) && (pDeviceListEntry != NULL))
  {
    // Get the filename of the /sys entry for the (hid) device and create
    // a udev_device object (pUdevDevice) representing it
    // according to the manual the memory pDeviceName points to 
    // after the assignment doesn't have to explicitely be freed 
    pDeviceName = udev_list_entry_get_name(pDeviceListEntry);
    pUDevDevice = udev_device_new_from_syspath(pUDev, pDeviceName);
    pParentDevice = udev_device_get_parent(pUDevDevice);
    hidIdFromList = udev_device_get_property_value(pParentDevice, "HID_ID");

    foundDevices = supportedJoypad.findDevice(hidIdFromList); 
    if (foundDevices > 0)
    {
      nodeDevice=udev_device_get_devnode(pUDevDevice);
    }
    udev_device_unref(pUDevDevice); // unref from udev_device_new_from_syspath
    pDeviceListEntry = udev_list_entry_get_next(pDeviceListEntry);
  }
  udev_enumerate_unref(pDeviceEnum);
#ifdef DEBUG
  std::fprintf(stderr, "[DeviceListInfo] returning with %03i.\n", updateListError); // debug msg
  std::fprintf(stderr, "[DeviceListInfo] active device has node \"%s\".\n", nodeDevice.c_str()); // debug msg
#endif // DEBUG
  return updateListError;
}
//--------------------------------------------------------------------------------------------------
void Joypad::loopPlug()
{
  constexpr int timeWaitUnplug_ms = 1000;

  std::string devNodeReportingDevice;
  // create a new epoll instance
  int fdepPlug = epoll_create1(EPOLL_CLOEXEC);
  if (fdepPlug < 0)
  {
    std::fprintf(stderr, "[hidPlug ERROR] Error creating epoll instance!\n"); // error msg
    udevError |= UDEV_PLUG_NO_POLL;
    return;
  }
  // add an epoll request for the pipe signalling stopPlugDetection
  // now the blocked epoll_pwait can be unblocked by simply closing the write end of the pipe
  epoll_event evPollPipe;
  evPollPipe.events = EPOLLONESHOT;
  evPollPipe.data.fd = fdPollPipe[0];
  int resultPollCtl = epoll_ctl(fdepPlug, EPOLL_CTL_ADD, fdPollPipe[0], &evPollPipe);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[hidPlug ERROR] Failed to add pipe fd to epoll!\n"); // error msg
    udevError |= UDEV_PLUG_NO_POLL;
    close(fdepPlug);
    return;
  }
  udev_monitor_enable_receiving(pUDevMonitor);
  // Get the file descriptor for the HID monitor...
  int fdUDevMon = udev_monitor_get_fd(pUDevMonitor);
  // add an epoll request for plug events
  epoll_event evPollPlug;
  evPollPlug.events = EPOLLIN;
  evPollPlug.data.fd = fdUDevMon;
  resultPollCtl = epoll_ctl(fdepPlug, EPOLL_CTL_ADD, fdUDevMon, &evPollPlug);
  if (resultPollCtl < 0)
  {
    std::fprintf(stderr, "[hidPlug ERROR] Failed to add fd to epoll - hotplug detection won't work!\n"); // error msg
    udevError |= UDEV_PLUG_NO_POLL;
  }
  updateDeviceList(); // in case devices are already connected upon start
  set(CTRL_READ_START);
  int timePlugPoll_ms = -1; // -1 means block (infinite wait time)
  bool detectingHIDPlug = true;
  int fdCount{}; // number of file descriptors returned
  epoll_event evReturned;
  udev_device* pReportingDevice{nullptr};
  while (detectingHIDPlug)
  {
    fdCount = epoll_pwait(fdepPlug, &evReturned, 1, timePlugPoll_ms, NULL); // wait is blocking
    if (fdCount == -1)
    { // returned with error value - but this does not necessarily stop the polling loop
      std::fprintf(stderr, "[hidPlug ERROR] epoll_pwait returned with error %i!\n", errno); // error msg
      continue;
    }
    if (fdCount == 0)
    { // epoll_pwait should come here on timeout after stopHIDPlugDetection was triggered
#ifdef DEBUG
      std::fprintf(stderr, "No more events.\n"); // debug msg - loop was stopped and no more events were received
#endif // DEBUG
      detectingHIDPlug = false;
      continue;
    }
    if (fdCount == 1)
    { // hidplug or pipe event received here, others should be ignored
      if ((evReturned.events & EPOLLHUP) && (evReturned.data.fd == fdPollPipe[0]))
      { // write end of the pipe was closed - this signals stopHIDPlugDetection
#ifdef DEBUG
        std::fprintf(stderr, "[hidPlug Info] EPOLLHUP on pipe.\n"); // debug msg
#endif // DEBUG
        timePlugPoll_ms = 100;
        continue;
      }
      if ((evReturned.events & EPOLLIN) && (evReturned.data.fd == fdUDevMon))
      { // this is an expected event - now get a pointer to the device that triggered it
        pReportingDevice = udev_monitor_receive_device(pUDevMonitor);
        if (pReportingDevice == NULL)
        {
          std::fprintf(stderr, "[UDevMon ERROR] udev_monitor_receive_device returned NULL!\n"); // error msg
        }
        else
        {
          devNodeReportingDevice = udev_device_get_devnode(pReportingDevice);
#ifdef DEBUG
          std::fprintf(stderr, "\nDC -> %s\n", devNodeReportingDevice.c_str()); // debug msg
#endif // DEBUG
          udev_device_unref(pReportingDevice);
          if (nodeDevice.empty())
          { // no Joypad was connected yet
            updateDeviceList(); // find one 
            set(CTRL_READ_START); // start reading from Joypad if there is one connected
          }
          else
          { // there was a Joypad connected already
            if (nodeDevice == devNodeReportingDevice)
            { // plug event came from the connected device
              usleep(1000 * timeWaitUnplug_ms);
              if (isDeviceReadyForRead)
              {
                std::fprintf(stderr, "[UDevMon ERROR] isDeviceReadyForRead still true!\n"); // error message
                std::fprintf(stderr, "[UDevMon ERROR] This situation should never occur.\n"); // error message
                // some kind of error condition
                // - cause: what action to the device could have happened
                // without setting isDeviceReadyForRead to false ???
              }
              else
              {
#ifdef DEBUG
                std::fprintf(stderr, "[UDevMon Info] can't read from node - calling updateDeviceList.\n"); // debug message
#endif // DEBUG
                set(CTRL_READ_STOP); // stopping the thread that still may try to read from the old node
                updateDeviceList();
                set(CTRL_READ_START);
              }
            }
          }
        }
        continue;
      }
    }
    // if (fdCount > 1)
    std::fprintf(stderr, "[UDevMon ERROR] Unexpected fd value returned (%i)!\n", fdCount); // error msg
    std::fprintf(stderr, "[UDevMon ERROR] This error should never occur!\n"); // error msg
  }
  epoll_ctl(fdepPlug, EPOLL_CTL_DEL, fdUDevMon, &evPollPlug);
  close(fdepPlug);
  epoll_ctl(fdepPlug, EPOLL_CTL_DEL, fdPollPipe[0], &evPollPlug);
#ifdef DEBUG
  std::fprintf(stderr, "[UDevMon Info] loopPlug thread ended.\n"); // debug msg
#endif // DEBUG
  return;
}
//--------------------------------------------------------------------------------------------------
int Joypad::startHIDPlugDetection()
{
  if (udevError > 0)
  {
    std::fprintf(stderr, "[startPlugDetection ERROR] udevError > 0!\n"); // error msg
    return -udevError;
  }
  if (pipe(fdPollPipe) < 0)
  {
    std::fprintf(stderr, "[startPlugDet ERROR] error (%3i) creating pipe!\n", errno); // error msg
    return -errno;
  }
  thrPlug = std::thread([this] {loopPlug(); });
  naoth::ThreadUtil::setPriority(thrPlug, naoth::ThreadUtil::Priority::lowest);
  naoth::ThreadUtil::setName(thrPlug, "HIDHotplug");
  return 0;
}
//--------------------------------------------------------------------------------------------------
int Joypad::stopHIDPlugDetection()
{
  if (udevError > 0)
  {
    std::fprintf(stderr, "[stopPlugDetection ERROR] udevError > 0!\n"); // error msg
    return -udevError;
  }
  set(CTRL_READ_STOP); // in case that there is a read still going on
  if (fdPollPipe[1] > 0)
  {
    close(fdPollPipe[1]);  // unblocking epoll_pwait by simply closing the write end of the pipe
    fdPollPipe[1] = 0;
  }
  if (thrPlug.joinable())
  {
    thrPlug.join();
  }
  if (fdPollPipe[0] > 0)
  {
    close(fdPollPipe[0]); // the pipe is not needed any longer - closing the read end
    fdPollPipe[0] = 0;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------
Joypad::Joypad()
  : udevError(UDEV_NOERROR)
{
  // initialized empty by default through SupportedJoypad
  //  supportedJoypad.deviceNode.clear(); // invalidate Joypad properties data
  initUDev();
  if (udevError == UDEV_NOERROR)
  {
    initMonitor();
    set(CTRL_PLUG_START);
  }
}
//--------------------------------------------------------------------------------------------------
Joypad::~Joypad()
{
  set(CTRL_PLUG_STOP);
  deinitMonitor();
  deinitUDev();
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
