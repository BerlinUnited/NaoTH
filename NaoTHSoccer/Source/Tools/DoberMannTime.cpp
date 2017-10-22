#include "DoberMannTime.h"

// NOTE: we assume GCC version >= 4.9
#if defined(__GNUC__)
// save the current state
#pragma GCC diagnostic push
// ignore warnings
#pragma GCC diagnostic ignored "-Wconversion"

#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#endif

#include "date.h"

#if defined(__GNUC__)
// restore the old state
#pragma GCC diagnostic pop
#endif


DoberMannTime::DoberMannTime()
{

}

std::uint32_t DoberMannTime::getSystemTimeMixedTeam()
{
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

  // TODO: calculate base only once
  auto dp = date::floor<date::days>(now);
  auto base = std::chrono::duration_cast<std::chrono::milliseconds>(dp.time_since_epoch()).count();

  return static_cast<std::uint32_t>(time - base);
}
