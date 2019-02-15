//--------------------------------------------------------------------------------------------------
//
// @file configs.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Config data handler - data read from files or statically predefined
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
struct JoypadProperties
{
  std::string                 hidId;
  std::string                 friendlyName;   // not needed (yet)
  std::string                 deviceNode;
  ssize_t                     inputReportLen;
  std::vector<unsigned char>  inputReportData;
};
//--------------------------------------------------------------------------------------------------
//int GetVendorDataFromFile(const std::string ConfigFilePath,
int getJoypadConfigFromFile(JoypadProperties& rJoypadProps,
                            const std::string hidId);
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _CONFIGS_H
