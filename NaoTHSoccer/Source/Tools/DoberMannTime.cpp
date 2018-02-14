#include "DoberMannTime.h"

#include <chrono>


DoberMannTime::DoberMannTime()
{

}

std::uint32_t DoberMannTime::getSystemTimeMixedTeam()
{
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

  // the time relative to the start of the current day
  return static_cast<std::uint32_t>(time % (24 * 60 * 60 * 1000));
}
