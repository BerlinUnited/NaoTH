
#ifndef _CPU_DATA_H
#define _CPU_DATA_H

#include <string>
#include <fstream>
#include <iostream>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class CpuData: public Printable
  {
  public:
    int temperature;

    CpuData();
    virtual void print(std::ostream& stream) const;

    virtual ~CpuData();

    void readDirect(std::ostream& stream) const {
      std::string val = "";

      std::ifstream temperatureFile ("/sys/class/thermal/thermal_zone0/temp");

      if (temperatureFile.is_open()) {
          // The temperature is stored in 5 digits.  The first two are degrees in C.  The rest are decimal precision.
          temperatureFile >> val;
          //std::cout << " value: " << val << std::endl;
          stream << " value: " << val << " °C\n";
          temperatureFile.close();
      } else {
        stream << "Failed to open temperatureFile!\n";
      }
    }
  };
}

#endif  /* _CPU_DATA_H */
