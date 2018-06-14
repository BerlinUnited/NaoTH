#ifndef BALLHYPOTHESIS_H
#define BALLHYPOTHESIS_H

#include "Tools/Filters/KalmanFilter/ExtendedKalmanFilter4d.h"
#include "Tools/Filters/AssymetricalBoolFilter.h"
#include "Representations/Infrastructure/FrameInfo.h"

class BallHypothesis : public ExtendedKalmanFilter4d
{
    private:
        naoth::FrameInfo lastUpdateFrame;
        naoth::FrameInfo createFrame;

    public:
        AssymetricalBoolHysteresisFilter ballSeenFilter;
        int stall_count;

    public:
        BallHypothesis(const naoth::FrameInfo& createFrame, const Eigen::Vector4d& state, const Eigen::Matrix2d& processNoiseStdSingleDimension, const Eigen::Matrix2d& measurementNoiseCovariances, const Eigen::Matrix2d& initialStateStdSingleDimension):
              ExtendedKalmanFilter4d(state, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension),
              lastUpdateFrame(createFrame),
              createFrame(createFrame),
              ballSeenFilter(0.01, 0.1), // some default params
              stall_count(0)
        {}

        void update(const Eigen::Vector2d &z, const Measurement_Function_H& h, const naoth::FrameInfo frameInfo){
            lastUpdateFrame = frameInfo;
            ExtendedKalmanFilter4d::update(z,h);
        }

        const naoth::FrameInfo& getLastUpdateFrame() const { return lastUpdateFrame; }

        const naoth::FrameInfo& getFrameOfCreation() const { return createFrame; }
};

#endif // BALLHYPOTHESIS_H
