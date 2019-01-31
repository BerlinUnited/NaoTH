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
struct sctJoypadData
{
  std::string                 HID_Id;
  std::string                 FriendlyName;   // not needed (yet)
  int                         InputReportLen;
  std::vector<unsigned char>  vInputReportDefaults;
};
//--------------------------------------------------------------------------------------------------
//bool GetVendorDataFromFile(const std::string ConfigFilePath,
int GetJoypadConfigFromFile(sctJoypadData& rJoyData,
                            const std::string HIDId);
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _CONFIGS_H
