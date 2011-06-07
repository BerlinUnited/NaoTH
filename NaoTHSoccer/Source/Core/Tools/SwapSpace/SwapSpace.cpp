
#include <vector>

#include "SwapSpace.h"

SwapSpace::SwapSpace()
{
}

SwapSpace::~SwapSpace()
{
}

SwapSpace::CognitionCache::CognitionCache()
{
  theMutex = g_mutex_new();
  updated = false;
}

SwapSpace::CognitionCache::~CognitionCache()
{
  g_mutex_free(theMutex);
}

void SwapSpace::CognitionCache::pull(HeadMotionRequest& hmr, MotionRequest& mr)
{
  g_mutex_lock(theMutex);
  if ( updated )
  {
    hmr = theHeadMotionRequest;
    mr = theMotionRequest;
    updated = false;
  }
  g_mutex_unlock(theMutex);
}

void SwapSpace::CognitionCache::push(const HeadMotionRequest& hmr, const MotionRequest& mr)
{
  g_mutex_lock(theMutex);
  theHeadMotionRequest = hmr;
  theMotionRequest = mr;
  updated = true;
  g_mutex_unlock(theMutex);
}

SwapSpace::MotionCache::MotionCache()
{
  theMutex = g_mutex_new();
}

SwapSpace::MotionCache::~MotionCache()
{
  g_mutex_free(theMutex);
}

void SwapSpace::MotionCache::pull(MotionStatus& ms, OdometryData& od)
{
  g_mutex_lock(theMutex);
  od = theOdometryData;
  ms = theMotionStatus;
  g_mutex_unlock(theMutex);
}

void SwapSpace::MotionCache::push(const MotionStatus& ms, const OdometryData& od)
{
  g_mutex_lock(theMutex);
  theMotionStatus = ms;
  theOdometryData = od;
  g_mutex_unlock(theMutex);
}
