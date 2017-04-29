
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
  };
}

#endif  /* _CPU_DATA_H */
