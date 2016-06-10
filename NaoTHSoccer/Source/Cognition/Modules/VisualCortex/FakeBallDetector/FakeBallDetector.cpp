#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector():
    active(false)
{
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:ballWasSeen", "whether ball was seen or not", true);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:stationaryBallOnField", "perception of a ball, which is not moving on field", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:constMovingOnField", "perception of a ball, which moves in a direction with constant velocity on field", true);

    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:determineCenterInImage", "determines of the ball's center in the \"image\"", true);

    startPosition << 2500, 0;
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
        ballPercept.bearingBasedOffsetOnField = Vector2d(startPosition(0), startPosition(1));
    );

    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vx", velocity(0));
    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vy", velocity(1));

    DEBUG_REQUEST_ON_DEACTIVE("Vision:FakeBallDetector:constMovingOnField",
        active = false;
    );

    DEBUG_REQUEST("Vision:FakeBallDetector:constMovingOnField",
        if(!active)
        {
            position << startPosition;
            active = true;
        }

        Eigen::Vector2d currentPosition = simulateConstantMovementOnField(getFrameInfo().getTimeInSeconds()-lastFrame.getTimeInSeconds(), velocity);
        ballPercept.bearingBasedOffsetOnField = Vector2d(currentPosition(0), currentPosition(1));
    );

    lastFrame = getFrameInfo();

    getBallPercept()    = ballPercept;
    getBallPerceptTop() = ballPercept;

    DEBUG_REQUEST("Vision:FakeBallDetector:determineCenterInImage",

        Vector3d point(ballPercept.bearingBasedOffsetOnField.x, ballPercept.bearingBasedOffsetOnField.y, 32.5);
        Vector2d pointInImage;

        bool in_front_of_cam = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), point, pointInImage);

        if(in_front_of_cam)
            getBallPercept().centerInImage = pointInImage;
        else
            getBallPercept().ballWasSeen = false;

        in_front_of_cam = CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfoTop(), point, pointInImage);

        if(in_front_of_cam)
            getBallPerceptTop().centerInImage = pointInImage;
        else
            getBallPerceptTop().ballWasSeen = false;
    );
}

const Eigen::Vector2d FakeBallDetector::simulateConstantMovementOnField(double dt, const Eigen::Vector2d &velocity)
{
    Eigen::Matrix2d time = Eigen::Matrix2d::Identity() * dt;
    return position += time*velocity;
}
