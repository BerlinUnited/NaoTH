//--------------------------------------------------------------------------------------------------
//
// @file USBJoypad.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Joypad support for Aldebaran's Nao
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <iostream>
#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include "udevIf.h"
//--------------------------------------------------------------------------------------------------
//using namespace std;
//--------
struct sctHIDDevice
{
int DeviceHandle;
int blocking;
int UsesNumberedReports;
};

int EnumResult;
int EnumScanResult;

int main()
{
  sctJoypadData JoypadData;
  const char* JoypadId="0000:00000000:00000000";
 
  
  clUDevInterface UDevInterface;
  
  UDevInterface.GetDeviceDataFromHIDId(JoypadData, JoypadId);
  
  UDevInterface.StartMonitoring();
  
  UDevInterface.StopMonitoring();
//    pDummy=udev_device_get_devpath(pParentDeviceRaw);
//    pDummy=udev_device_get_subsystem(pParentDeviceRaw);
//    pDummy=udev_device_get_devtype(pParentDeviceRaw);
//    pDummy=udev_device_get_syspath(pParentDeviceRaw);
//    pDummy=udev_device_get_sysnum(pParentDeviceRaw);
//    pDummy=udev_device_get_devnode(pParentDeviceRaw);


/*
    if ((0 == strcmp(pSysName, "hidraw3")))
    {
      handle=open(pSysPath, O_RDONLY);

      for (int i=0; i < 1000; i++)
      {
        BytesRead=read(handle, &Buffer, 100);
        printf("%05i: ", i);
        for (int i2=0; i2 < BytesRead; i2++)
        {
          printf("%02x ", Buffer[i2]);
        }
        printf("\n");
      }

      close(handle);
    }
*/
//    printf("VID: %s\n", udev_device_get_sysattr_value(pRawDevice,"idVendor"));
//    printf("PID: %s\n", udev_device_get_sysattr_value(pRawDevice, "idProduct"));
//    printf("Man: %s\n", udev_device_get_sysattr_value(pRawDevice,"manufacturer"));
//    printf("Pro: %s\n", udev_device_get_sysattr_value(pRawDevice,"product"));
//    printf("Ser: %s\n", udev_device_get_sysattr_value(pHIDDevice, "serial"));


  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
