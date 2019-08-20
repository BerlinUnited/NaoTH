
#ifndef _CPU_DATA_H
#define _CPU_DATA_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class CpuData: public Printable
  {
  public:
    double temperature;

    CpuData();
    virtual void print(std::ostream& stream) const;

    virtual ~CpuData();
  };
}

#endif  /* _CPU_DATA_H */
