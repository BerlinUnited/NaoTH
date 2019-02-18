//
// @file configs.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Config data handler - data read from files or statically predefined
//
#ifndef _SupportedJoypads_H_
#define _SupportedJoypads_H_
#include <string>
#include <vector>
#include <map>

#include "USBJoypadData.h"


class Joypad
{
public:
  const std::string id;
  const std::string name;
  
  Joypad(const std::string& id, const std::string& name) : id(id), name(name) {}
  
  bool read(const std::vector<unsigned char>& data, USBJoypadData& result) = 0;
};


class DragonRise: public Joypad
{
public:
  DragonRise() 
    : Joypad("0003:00000079:00000126", "Wireless USB Gamepad (0079/0126)")
  {}
  
  bool read(const std::vector<unsigned char>& data, USBJoypadData& result) 
  {
    result.button.A = data[10] > 0;
  }
}


class SupportedJoypads
{
private:
  std::map<std::string, Joypad> joypads;
  
public:

    SupportedJoypads() {
      
    }

    bool getJoypad(std::string id, const std::string& name, Joypad& joypad) 
    {
      auto iter = joypads.find(id);
      if(iter == joypads.end()){
        return false;
      }
      
      joypad = *iter;
      
      return true;
    }  
};


#endif // _SupportedJoypads_H_
