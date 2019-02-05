//--------------------------------------------------------------------------------------------------
//
// @file configs.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Config data handler - data read from files or statically predefined
//
//--------------------------------------------------------------------------------------------------
#ifndef _CONFIGS_H
#define _CONFIGS_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
//--------------------------------------------------------------------------------------------------
struct JoypadDefaultData
{
  std::string                 hidId;
  std::string                 friendlyName;   // not needed (yet)
  std::string                 deviceNode;
  int                         inputReportLen;
  std::vector<unsigned char>  inputReportData;
};
//--------------------------------------------------------------------------------------------------
//int GetVendorDataFromFile(const std::string ConfigFilePath,
int getJoypadConfigFromFile(JoypadDefaultData& rJoyDefaultData,
                            const std::string hidId);
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _CONFIGS_H
