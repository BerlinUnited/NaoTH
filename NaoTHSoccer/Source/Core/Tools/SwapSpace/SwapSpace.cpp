
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
    pthread_mutex_init(&theMutex, NULL);
    updated = false;
}

SwapSpace::CognitionCache::~CognitionCache()
{
    pthread_mutex_destroy(&theMutex);
}

void SwapSpace::CognitionCache::pull(HeadMotionRequest& hmr, MotionRequest& mr)
{
    pthread_mutex_lock(&theMutex);
    if ( updated )
    {
        hmr = theHeadMotionRequest;
        mr = theMotionRequest;
        updated = false;
    }
    pthread_mutex_unlock(&theMutex);
}

void SwapSpace::CognitionCache::push(const HeadMotionRequest& hmr, const MotionRequest& mr)
{
    pthread_mutex_lock(&theMutex);
    theHeadMotionRequest = hmr;
    theMotionRequest = mr;
    updated = true;
    pthread_mutex_unlock(&theMutex);
}

SwapSpace::MotionCache::MotionCache()
{
    pthread_mutex_init(&theMutex, NULL);
}

SwapSpace::MotionCache::~MotionCache()
{
    pthread_mutex_destroy(&theMutex);
}

void SwapSpace::MotionCache::pull(MotionStatus& ms, OdometryData& od)
{
    pthread_mutex_lock(&theMutex);
    od = theOdometryData;
    ms = theMotionStatus;
    pthread_mutex_unlock(&theMutex);
}

void SwapSpace::MotionCache::push(const MotionStatus& ms, const OdometryData& od)
{
    pthread_mutex_lock(&theMutex);
    theMotionStatus = ms;
    theOdometryData = od;
    pthread_mutex_unlock(&theMutex);
}
