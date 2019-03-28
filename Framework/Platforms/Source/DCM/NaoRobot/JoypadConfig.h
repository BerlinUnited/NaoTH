//--------------------------------------------------------------------------------------------------
//
// @file JoypadConfig.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Configs for custom gamepads/joypads
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _JOYPADCONFIG_H
#define _JOYPADCONFIG_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <map>
#include <memory>
#include "Representations/Infrastructure/JoypadData.h"
//--------------------------------------------------------------------------------------------------
namespace naoth
{
  namespace joypads
  {
    class GenericJoypad
    {
    private:
      struct VendorData
      {
        const std::string idHID;
        const std::string friendlyName;
        const std::vector<unsigned char> defaultInputReport;
      } vendor;
      ssize_t lenInputReport{0};
    public:
      virtual std::string getVendorId(void);
      virtual std::string getDeviceName(void);
      virtual std::vector<unsigned char> getDefaultReport(void);
      virtual std::size_t getDefaultReportLen(void);
      virtual int getReportAsControls(const std::vector<unsigned char>&, 
                                      UnifiedJoypadControls&) = 0;
      GenericJoypad(const std::string& idVendor, 
                    const std::string& nameVendor, 
                    const std::vector<unsigned char> defaultReport);
    };
//--------------------------------------------------------------------------------------------------
//  this pseudo device provides well defined data if no registered joypad is found
    class ErrorDevice : public GenericJoypad
    {
    public:
      virtual int getReportAsControls(const std::vector<unsigned char>&,
                                      UnifiedJoypadControls&);
      ErrorDevice();
    };
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//  idHID("0003:00000079:00000126");
    class DragonRise0126 : public GenericJoypad
    {
    public:
      virtual int getReportAsControls(const std::vector<unsigned char>&, 
                                      UnifiedJoypadControls&);
      DragonRise0126();
    };
//--------------------------------------------------------------------------------------------------
//  idHID("0003:000006A3:00005F0D");
    class Saitek5F0D : public GenericJoypad
    {
    public:
      virtual int getReportAsControls(const std::vector<unsigned char>&, 
                                      UnifiedJoypadControls&);
      Saitek5F0D();
    };
//--------
// put more supported Joypads here
//--------------------------------------------------------------------------------------------------
    class SupportedJoypad
    {
    private:
      std::map<std::string, std::shared_ptr<GenericJoypad>> databaseJoypads;
      std::map<std::string, std::shared_ptr<GenericJoypad>>::const_iterator resultFind;
      std::vector<unsigned char> reportInput;
    public:
      int findDevice(const std::string& idToSearchFor);
      std::size_t getDefaultReportLen(void);
      int setReport(const std::vector<unsigned char>&);
      std::vector<unsigned char> getReportRaw(void);
      int getReportAsControls(UnifiedJoypadControls&);
      SupportedJoypad();
    };
  } // namespace joypads
} // namespace naoth
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif // _JOYPADCONFIG_H
