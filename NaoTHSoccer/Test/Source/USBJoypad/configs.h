//--------------------------------------------------------------------------------------------------
//
// @file configs.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Header for handling of config files
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _CONFIGS_H
#define _CONFIGS_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
//--------------------------------------------------------------------------------------------------
struct JoypadDefaultData
{
  std::string                 hid_id;
  std::string                 friendlyName;   // not needed (yet)
  std::string                 deviceNode;
  int                         inputReportLen;
  std::vector<unsigned char>  inputReportData;
};
//--------------------------------------------------------------------------------------------------
//int GetVendorDataFromFile(const std::string ConfigFilePath,
int GetJoypadConfigFromFile(JoypadDefaultData& rJoyDefaultData,
                            const std::string HIDId);
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _CONFIGS_H
