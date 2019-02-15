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
#define CONFIGS_DEFAULT_COUNT   2
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

// the following are "dummy" data eventually replaced by data read from config files
//--------------------------------------------------------------------------------------------------
// this is a dummy default config - it DOES NOT count in CONFIGS_DEFAULT_CNT
const std::vector<unsigned char> reportDefault_0000_0000_0000
  {0x00    // dummy data
  };
//------
// DragonRise Gamepad 
const std::vector<unsigned char> reportDefault_0003_0079_0126
  {0x00,  // X=00000001, A=00000010, B=00000100, Y=00001000, L=00010000, R=00100000
   0x00,  // SELECT=00000001, START=00000010
   0x0F,  // unused
   0x80,  // Pad Left pressed = 0x00, Pad Right pressed = 0xFF
   0x80,  // Pad Up pressed = 0x00, Pad Down pressed = 0xFF
   0x80,  // unused
   0x80,  // unused
   0x00,  // Pad Right off = 0x00, pressed = 0xFF
   0x00,  // Pad Left off = 0x00, pressed = 0xFF
   0x00,  // Pad Up off = 0x00, pressed = 0xFF
   0x00,  // Pad Down off = 0x00, pressed = 0xFF
   0x00,  // X off = 0x00, pressed = 0xFF
   0x00,  // A off = 0x00, pressed = 0xFF
   0x00,  // B off = 0x00, pressed = 0xFF
   0x00,  // Y off = 0x00, pressed = 0xFF
   0x00,  // L off = 0x00, pressed = range(0x00..0xFF)
   0x00,  // R off = 0x00, pressed = range(0x00..0xFF)
   0x00,  // unused
   0x00,  // unused
   0x00,  // unused
   0x02,  // unused
   0x00,  // unused
   0x02,  // unused
   0x00,  // unused
   0x02,  // unused
   0x00,  // unused
   0x02}; // unused
//------
// Saitek Pad
const std::vector<unsigned char> reportDefault_0003_06A3_5F0D
  {0x01,   // report nr ???
   0x80,   // left stick X  = range(0x00..0xFF)
   0x80,   // left stick Y  = range(0x00..0xFF)
   0x80,   // right stick X = range(0x00..0xFF)
   0x80,   // right stick Y = range(0x00..0xFF)
   0x00,   // low nibble = buttons 1..4, high nibble = buttons 5..8
   0xF0};  // low nibble = buttons 9..12, high nibble = dpad 0..7
//------
// put more predefined pads in here ...

//--------------------------------------------------------------------------------------------------
JoypadProperties hidDefaultData[1+CONFIGS_DEFAULT_COUNT]=
{
  {"0000:00000000:00000000", "Dummy Device", "", 1, reportDefault_0000_0000_0000}, 
  {"0003:00000079:00000126", "Wireless USB Gamepad (0079/0126)", "", 27, reportDefault_0003_0079_0126},
  {"0003:000006A3:00005F0D", "Saitek P2600 Rumble Force Pad", "", 7, reportDefault_0003_06A3_5F0D}
// put more predefined pads in here ...
};
//------ Linux part ends
#endif
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int getJoypadConfigFromFile(JoypadProperties& rJoypadProps, const std::string hidId)
{
  int isConfigFound=0;
  int configFileNr=0;

  while ((isConfigFound == 0) && (configFileNr < CONFIGS_DEFAULT_COUNT))
  { // search directory for config files
    // read a config file if found
    // !!! HERE !!!
    // and match file data with input data
    configFileNr++;
    if (hidDefaultData[configFileNr].hidId == hidId)
    {
      rJoypadProps.hidId=hidDefaultData[configFileNr].hidId;
      rJoypadProps.friendlyName=hidDefaultData[configFileNr].friendlyName;
//      rJoypadProps.deviceNode=hidDefaultData[configFileNr].deviceNode;
      rJoypadProps.inputReportLen=hidDefaultData[configFileNr].inputReportLen;
      rJoypadProps.inputReportData.reserve(rJoypadProps.inputReportLen);
      rJoypadProps.inputReportData=hidDefaultData[configFileNr].inputReportData;
      isConfigFound=1;
    }
  }

  return isConfigFound;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
