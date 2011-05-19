/**
 * @file SwapSpace.h
 * 
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _SWAPSPACE_H
#define	_SWAPSPACE_H

#include <vector>
#include <Tools/DataStructures/Singleton.h>

// cognition -> motion
#include "Core/Representations/Motion/Request/HeadMotionRequest.h"
#include "Core/Representations/Motion/Request/MotionRequest.h"
#include "Core/Representations/Motion/MotionStatus.h"

// motion -> cognition
#include <Representations/Perception/CameraMatrix.h>
#include "Core/Representations/Modeling/OdometryData.h"

class SwapSpace : public Singleton<SwapSpace> {
private:
    friend class Singleton<SwapSpace>;

    SwapSpace();
    ~SwapSpace();

public:
    // cognition -> motion

    class CognitionCache {
    public:
        CognitionCache();
        ~CognitionCache();

        void pull(HeadMotionRequest& hmr, MotionRequest& mr);

        void push(const HeadMotionRequest& hmr, const MotionRequest& mr);

    private:
        pthread_mutex_t theMutex;

        bool updated;
        HeadMotionRequest theHeadMotionRequest;
        MotionRequest theMotionRequest;
        WalkRequest theWalkRequest;
    };

    // motion -> cognition
    class MotionCache {
    public:
        MotionCache();

        ~MotionCache();

        void pull(MotionStatus& ms, OdometryData& od);

        void push(const MotionStatus& ms, const OdometryData& od);

    private:
        pthread_mutex_t theMutex;

        MotionStatus theMotionStatus;
        OdometryData theOdometryData;
    };

    CognitionCache theCognitionCache;
    MotionCache theMotionCache;
};

#endif	/* _SWAPSPACE_H */

