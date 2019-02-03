//--------------------------------------------------------------------------------------------------
//
// @file configs.cpp
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Definitions for config file handling
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include "configs.h"
//--------------------------------------------------------------------------------------------------
//const std::string ConfigFilePath="File";
//--------------------------------------------------------------------------------------------------
// for multi platform compatibility
#ifdef  WIN32
// Windows part
#include <windows.h>
//------
// the following are "dummy" data eventually replaced by data read from config files
const std::vector<unsigned char> ReportDefault_0000_0000_0000 =
  {0x00    // dummy data
  };
//------ Windows part ends
#else
// Linux part
#include <unistd.h>
#include <fcntl.h>
//--------------------------------------------------------------------------------------------------
// the following are "dummy" data eventually replaced by data read from config files
const std::vector<unsigned char> ReportDefault_0000_0000_0000 =
  {0x00    // dummy data
  };
const std::vector<unsigned char> ReportDefault_0003_06A3_5F0D =
  {0x01,   // report nr ???
   0x80,   // left stick X  = range(0x00..0xFF)
   0x80,   // left stick Y  = range(0x00..0xFF)
   0x80,   // right stick X = range(0x00..0xFF)
   0x80,   // right stick Y = range(0x00..0xFF)
   0x00,   // low nibble = buttons 1..4, high nibble = buttons 5..8
   0xF0};  // low nibble = buttons 9..12, high nibble = dpad 0..7 (default 0xF.)
//------
const std::vector<unsigned char> ReportDefault_0003_0079_0126 =
  {0x00,  // X=00000001, A=00000010, B=00000100, Y=00001000, L=00010000, R=00100000
   0x00,  // SELECT=00000001, START=00000010
   0x0F,  // unknown / unused
   0x80,  // Pad Left pressed = 0x00, Pad Right pressed = 0xFF
   0x80,  // Pad Up pressed = 0x00, Pad D pressed = 0xFF
   0x80,  // unknown / unused
   0x80,  // unknown / unused
   0x00,  // Pad Right off = 0x00, pressed = 0xFF
   0x00,  // Pad Left off = 0x00, pressed = 0xFF
   0x00,  // Pad Up ...
   0x00,  // Pad Down ...
   0x00,  // X off = 0x00, pressed = 0xFF
   0x00,  // A off = 0x00, pressed = 0xFF
   0x00,  // B ...
   0x00,  // Y ...
   0x00,  // L off = 0x00, pressed = range(0x00..0xFF)
   0x00,  // R off = 0x00, pressed = range(0x00..0xFF)
   0x00,  // unknown / unused
   0x00,  // unknown / unused
   0x00,  // unknown / unused
   0x02,  // unknown / unused
   0x00,  // unknown / unused
   0x02,  // unknown / unused
   0x00,  // unknown / unused
   0x02,  // unknown / unused
   0x00,  // unknown / unused
   0x02}; // unknown / unused
//------
sctJoypadData HIDData[3]=
{
  {"0000:00000000:00000000", "Dummy Device", NULL, 1, ReportDefault_0000_0000_0000}, 
  {"0003:000006A3:00005F0D", "Saitek P2600 Rumble Force Pad", NULL, 7, ReportDefault_0003_06A3_5F0D}, 
  {"0003:00000079:00000126", "Wireless USB Gamepad (0079/0126)", NULL, 27, ReportDefault_0003_0079_0126}
};

//------ Linux part ends
#endif //WIN32
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int GetJoypadConfigFromFile(sctJoypadData& rJoyData, const std::string HIDId)
{
  int isDataFound;
  int ConfigFileNr;

  ConfigFileNr=0;
  isDataFound=0;
  while (isDataFound == 0)
  { // search directory for config files
    // read a config file if found
    // !!! HERE !!!
    // and match file data with input data
    ConfigFileNr++;
    if (HIDData[ConfigFileNr].HID_Id == HIDId)
    {
      rJoyData.HID_Id=HIDData[ConfigFileNr].HID_Id;
      rJoyData.FriendlyName=HIDData[ConfigFileNr].FriendlyName;
      rJoyData.InputReportLen=HIDData[ConfigFileNr].InputReportLen;
      rJoyData.vInputReportData.reserve(rJoyData.InputReportLen);
      rJoyData.vInputReportData=HIDData[ConfigFileNr].vInputReportData;
      isDataFound=1;
    }
  }
  return isDataFound;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
