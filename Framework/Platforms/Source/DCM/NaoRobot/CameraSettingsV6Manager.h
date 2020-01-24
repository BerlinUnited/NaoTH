#ifndef _CameraSettingsV6Manager_H_
#define _CameraSettingsV6Manager_H_

#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>
#include <bitset>

class CameraSettingsV6Manager : public V4LCameraSettingsManager 
{  
public:

  CameraSettingsV6Manager();

  virtual void query(int cameraFd, const std::string& cameraName, naoth::CameraSettings& settings);
  virtual void apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings& settings, bool force=false);

private:
  naoth::CameraSettings current;
  bool initialized;

  #pragma pack(1)
  struct Register {
    uint8_t id;
    uint16_t addr; 
    uint16_t value;
  };//__attribute__((packed)); // very gcc specific
  #pragma pack()
  
  void print_bytes(const uint8_t* data, size_t size) {
    for(size_t i = 0; i < size; ++i) {
      std::cout << std::bitset<8>(data[i]);
    }
    std::cout << std::endl;
  }
  
  // convert between big-endian and little-endian
  uint16_t swapBytes(uint16_t v) {
    return static_cast<uint16_t>(((v & 0xff) << 8) | ((v & 0xff00) >> 8));
  }
  
  uint16_t getRegister(int cameraFd, uint16_t addr);
  bool setRegister(int cameraFd, uint16_t addr, uint16_t val);
};

#endif // _CameraSettingsV6Manager_H_