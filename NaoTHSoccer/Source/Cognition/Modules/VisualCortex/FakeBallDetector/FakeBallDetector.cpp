#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector():
    active(false)
{
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:ballWasSeen", "whether ball was seen or not", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:stationaryBallOnField", "perception of a ball, which is not moving on field", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:constMovingOnField", "perception of a ball, which moves in a direction with constant velocity on field", false);

    position << 2500, 0;
    velocity = Eigen::Vector2d::Zero();
}

FakeBallDetector::~FakeBallDetector()
{

}

void FakeBallDetector::execute(){

    BallPerceptTop ballPercept;

    DEBUG_REQUEST("Vision:FakeBallDetector:ballWasSeen",
        ballPercept.ballWasSeen = true;
    );


    DEBUG_REQUEST("Vision:FakeBallDetector:stationaryBallOnField",
        ballPercept.bearingBasedOffsetOnField = Vector2d(position(0), position(1));
    );

    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vx", velocity(0));
    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vy", velocity(1));

    DEBUG_REQUEST_ON_DEACTIVE ("Vision:FakeBallDetector:constMovingOnField",
        active = false;
    );

    DEBUG_REQUEST("Vision:FakeBallDetector:constMovingOnField",
        if(!active)
        {
            activationFrame = getFrameInfo();
            active = true;
        }

        Eigen::Vector2d currentPosition = simulateConstantMovementOnField(getFrameInfo().getTimeInSeconds()-activationFrame.getTimeInSeconds(), velocity);
        ballPercept.bearingBasedOffsetOnField = Vector2d(currentPosition(0), currentPosition(1));
    );

    getBallPercept()    = ballPercept;
    getBallPerceptTop() = ballPercept;
}

Eigen::Vector2d FakeBallDetector::simulateConstantMovementOnField(double dt, const Eigen::Vector2d &velocity)
{
    Eigen::Matrix2d time = Eigen::Matrix2d::Identity() * dt;
    return position + time*velocity;
}
