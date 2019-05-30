#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector():
    active(false)
{
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:ballWasSeen", "whether ball was seen or not", true);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:stationaryBallOnField", "perception of a ball, which is not moving on field", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:constMovingOnField", "perception of a ball, which moves in a direction with constant velocity on field", true);

    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:determineCenterInImage", "determines of the ball's center in the \"image\"", true);

    startPosition = Vector2d(2500, 0);
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
        ballPercept.bearingBasedOffsetOnField = startPosition;
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
        ballPercept.bearingBasedOffsetOnField = simulateConstantMovementOnField(getFrameInfo().getTimeInSeconds()-lastFrame.getTimeInSeconds(), velocity);
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

const Vector2d FakeBallDetector::simulateConstantMovementOnField(double dt, const Vector2d &velocity)
{
    return position += velocity*dt;
}
