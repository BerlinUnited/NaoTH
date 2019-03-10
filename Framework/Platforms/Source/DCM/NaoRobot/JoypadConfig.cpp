//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include "JoypadConfig.h"
//--------------------------------------------------------------------------------------------------
namespace naoth
{
  namespace joypads
  {
//--------------------------------------------------------------------------------------------------
    GenericJoypad::GenericJoypad(const std::string& idVendor, 
                                 const std::string& nameVendor, 
                                 const std::vector<unsigned char> defaultReport)
      : vendor{idVendor, nameVendor, defaultReport}
    {
      lenInputReport = defaultReport.size();
    }
//--------
    std::string GenericJoypad::getVendorId()
    {
      return vendor.idHID;
    }
//--------
    std::string GenericJoypad::getDeviceName()
    {
      return vendor.friendlyName;
    }
//--------
    std::size_t GenericJoypad::getDefaultReportLen()
    {
      return lenInputReport;
    }
//--------
    std::vector<unsigned char> GenericJoypad::getDefaultReport()
    {
      return vendor.defaultInputReport;
    }
//--------------------------------------------------------------------------------------------------
    ErrorDevice::ErrorDevice()
      : GenericJoypad{"0000:00000000:00000000", "No Gamepad", {}}
    {}
//--------
    int ErrorDevice::getReportAsControls(const std::vector<unsigned char>& /* report (unused here) */, 
                                         UnifiedJoypadControls& uc)
    {
      uc.isValid = false;
      return -1;
    }
//--------------------------------------------------------------------------------------------------
    DragonRise0126::DragonRise0126()
      : GenericJoypad{"0003:00000079:00000126", "DragonRise Gamepad",
      { // input report defaults
        0x00,  // X=00000001, A=00000010, B=00000100, Y=00001000, L=00010000, R=00100000
        0x00,  // SELECT=00000001, START=00000010
        0x0F,  // unused (dpad)
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
        0x02}  // unused
      }
    {}
//--------
    int DragonRise0126::getReportAsControls(const std::vector<unsigned char>& report, 
                                            UnifiedJoypadControls& uc)
    {
      if (report.size() != getDefaultReportLen())
      {
        uc.isValid = false;
        return -1;
      }
      uc.isValid = true;
      uc.button.select = ((report[1] & 0x01) == 0x01);
      uc.button.start  = ((report[1] & 0x02) == 0x02);
      uc.button.A      = ((report[0] & 0x02) == 0x02);
      uc.button.B      = ((report[0] & 0x04) == 0x04);
      uc.button.X      = ((report[0] & 0x01) == 0x01);
      uc.button.Y      = ((report[0] & 0x08) == 0x08);
      uc.button.LT     = ((report[0] & 0x10) == 0x10);
      uc.button.RT     = ((report[0] & 0x20) == 0x20);
      uc.trigger.LT    = report[15] / 256.0;    // normalized 0..1
      uc.trigger.RT    = report[16] / 256.0;    // normalized 0..1
      uc.stick.L.x     = report[3] / 128.0 - 1; // normalized -1..1
      uc.stick.L.y     = report[4] / 128.0 - 1; // normalized -1..1
      return 0;
    }
//--------------------------------------------------------------------------------------------------
    Saitek5F0D::Saitek5F0D()
      : GenericJoypad{"0003:000006A3:00005F0D", "Saitek P2600",
      {
        0x01,   // report nr ???
        0x80,   // left stick X  = range(0x00..0xFF)
        0x80,   // left stick Y  = range(0x00..0xFF)
        0x80,   // right stick X = range(0x00..0xFF)
        0x80,   // right stick Y = range(0x00..0xFF)
        0x00,   // low nibble = buttons 1..4, high nibble = buttons 5..8
        0xF0}   // low nibble = buttons 9..12, high nibble = dpad 0..7
      }
    {}
//--------
    int Saitek5F0D::getReportAsControls(const std::vector<unsigned char>& report, 
                                        UnifiedJoypadControls& uc)
    {
      if (report.size() != getDefaultReportLen())
      {
        uc.isValid = false;
        return -1;
      }
      uc.isValid = true;
      uc.button.select = ((report[6] & 0x01) == 0x01);
      uc.button.start  = ((report[6] & 0x02) == 0x02);
      uc.button.X      = ((report[5] & 0x01) == 0x01);
      uc.button.A      = ((report[5] & 0x02) == 0x02);
      uc.button.B      = ((report[5] & 0x04) == 0x04);
      uc.button.Y      = ((report[5] & 0x08) == 0x08);
      uc.button.LB     = ((report[5] & 0x10) == 0x10);
      uc.button.RB     = ((report[5] & 0x20) == 0x20);
      uc.button.LT     = ((report[5] & 0x40) == 0x40);
      uc.button.RT     = ((report[5] & 0x80) == 0x80);
      uc.dpad          = report[6] >> 4;
      uc.stick.L.x     = report[1] / 128.0 - 1; // normalized -1..1
      uc.stick.L.y     = report[2] / 128.0 - 1; // normalized -1..1
      // right stick x/y converted to Nao x/y coordinates
      uc.stick.R.x     = 1 - report[4] / 128.0; // normalized -1..1
      uc.stick.R.y     = report[3] / 128.0 - 1; // normalized -1..1
      return 0;
    }
//--------
// put more supported joypads here
//--------------------------------------------------------------------------------------------------
    SupportedJoypad::SupportedJoypad()
    {
      auto j0 = std::make_shared<ErrorDevice>();
      databaseJoypads.insert(std::make_pair(j0->getVendorId(), j0));
      auto j1 = std::make_shared<DragonRise0126>();
      databaseJoypads.insert(std::make_pair(j1->getVendorId(), j1));
      auto j2 = std::make_shared<Saitek5F0D>();
      databaseJoypads.insert(std::make_pair(j2->getVendorId(), j2));

      resultFind = databaseJoypads.cbegin();
    }
//--------
    int SupportedJoypad::findDevice(const std::string& nodeToSearchFor)
    {
      resultFind = databaseJoypads.find(nodeToSearchFor);
      if (resultFind == databaseJoypads.cend())
      { // no joypad entry was found, point to "error device"
        resultFind = databaseJoypads.cbegin();
        reportInput = resultFind->second->getDefaultReport();
        return 0;
      }
      reportInput = resultFind->second->getDefaultReport();
      return 1;
    }
//--------
    std::size_t SupportedJoypad::getDefaultReportLen()
    {
      return resultFind->second->getDefaultReportLen();
    }
//--------
    int SupportedJoypad::setReport(const std::vector<unsigned char>& report)
    {
      reportInput = report;
      return 0;
    }
//--------
    std::vector<unsigned char> SupportedJoypad::getReportRaw()
    {
      return reportInput;
    }
//--------
    int SupportedJoypad::getReportAsControls(UnifiedJoypadControls& uc)
    {
      return resultFind->second->getReportAsControls(reportInput, uc);
    }
  } // namespace joypads
} // namespace naoth
//--------------------------------------------------------------------------------------------------
//==================================================================================================
