#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector():
    active(false)
{
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:disable", "disable fake ball", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:stationaryBallOnField", "perception of a ball, which is not moving on field", true);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:constMovingOnField", "perception of a ball, which moves in a direction with constant velocity on field", false);

    startPosition = Vector2d(2500, 0);
}

FakeBallDetector::~FakeBallDetector()
{

}

void FakeBallDetector::execute()
{
    getMultiBallPercept().reset();
    DEBUG_REQUEST("Vision:FakeBallDetector:disable",
                  return;
    );

    MultiBallPercept::BallPercept ballPercept;

    DEBUG_REQUEST("Vision:FakeBallDetector:stationaryBallOnField",
        ballPercept.positionOnField = startPosition;
    );

    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vx", velocity.x);
    MODIFY("Vision:FakeBallDetector:constMovingOnField:Vy", velocity.y);

    DEBUG_REQUEST_ON_DEACTIVE("Vision:FakeBallDetector:constMovingOnField",
        active = false;
    );

    DEBUG_REQUEST("Vision:FakeBallDetector:constMovingOnField",
        if(!active)
        {
            position = startPosition;
            active = true;
        }
        ballPercept.positionOnField = simulateConstantMovementOnField(getFrameInfo().getTimeInSeconds()-lastFrame.getTimeInSeconds(), velocity);
    );

    lastFrame = getFrameInfo();

    Vector3d point(ballPercept.positionOnField.x, ballPercept.positionOnField.y, 32.5);  // WHY 32.5? i would have set this to ballradius
    Vector2d pointInImage;

    bool in_image_bottom = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), point, pointInImage);

    if(in_image_bottom) {
        ballPercept.centerInImage = pointInImage;
        ballPercept.cameraId      = naoth::CameraInfo::Bottom;
        getMultiBallPercept().add(ballPercept);
    }

    bool in_image_top = CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfoTop(), point, pointInImage);

    if(in_image_top) {
        ballPercept.centerInImage = pointInImage;
        ballPercept.cameraId      = naoth::CameraInfo::Top;
        getMultiBallPercept().add(ballPercept);
    }

    if(in_image_bottom || in_image_top) {
        getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
    }
}

const Vector2d FakeBallDetector::simulateConstantMovementOnField(double dt, const Vector2d &velocity)
{
    return position += velocity*dt;
}
