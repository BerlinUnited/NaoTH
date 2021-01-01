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
  //naoth::CameraSettings current;
  naoth::V6CameraSettings current;
  bool initialized;

  /* 
  NOTE: this structure is used to write and read registers in setRegister and getRegister.
  In the process is is converted to (unsigned char*) and read bytewise.  
  For this the exact array lenghts has to be 
     size(Register) == 5
  which only holds if the alignment 
  is set to 1 byte. This can be achieved with #pragma pack(1) or with GCC speciffic __attribute__((packed)).
  #pragma pack(1) is also avaliable in VS and seems more general.
  */
  #pragma pack(1)
  struct Register {
    uint8_t id;
    uint16_t addr; 
    uint16_t value;
  };//__attribute__((packed)); // very gcc specific
  #pragma pack()
  
  // make sure the alignment is correct
  static_assert(sizeof(Register) == 5, "Register has to have alignment 1, sich that size(Register) == 5.");
  
  
  void print_bytes(const uint8_t* data, size_t size) {
    for(size_t i = 0; i < size; ++i) {
      std::cout << std::bitset<8>(data[i]);
    }
    std::cout << std::endl;
  }
  
  inline uint16_t set( size_t bitNum, size_t n ) {
      return static_cast<uint16_t>(bitNum | (1 << n));
  }

  inline uint16_t reset( size_t bitNum, size_t n ) {
      return static_cast<uint16_t>(bitNum & (~(1 << n )) );
  }
  
  // convert between big-endian and little-endian
  uint16_t swapBytes(uint16_t v) {
    return static_cast<uint16_t>(((v & 0xff) << 8) | ((v & 0xff00) >> 8));
  }
  
  uint16_t getRegister(int cameraFd, uint16_t addr);
  uint32_t getRegister32(int cameraFd, uint16_t addr);
  bool setRegister32(int cameraFd, uint16_t addr, uint32_t value);
  bool setRegister(int cameraFd, uint16_t addr, uint16_t val);
};

#endif // _CameraSettingsV6Manager_H_