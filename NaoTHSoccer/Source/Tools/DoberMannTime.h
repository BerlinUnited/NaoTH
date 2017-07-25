#ifndef DOBERMANNTIME_H
#define DOBERMANNTIME_H

#include <cstdint>

class DoberMannTime
{
public:
  DoberMannTime();

  static std::uint32_t getSystemTimeMixedTeam();
};

#endif // DOBERMANNTIME_H
