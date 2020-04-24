#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector() {
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:disable", "disable fake balls", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:clear", "clear all balls", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:add_new_ball", "add a new ball - position, velocity can be set by Vision:FakeBallDetector:new_ball before creation", false);
}

void FakeBallDetector::execute() {
    getMultiBallPercept().reset();

    DEBUG_REQUEST("Vision:FakeBallDetector:clear",
        clearFakeBalls();
    );

    FakeBall fb;
    MODIFY("Vision:FakeBallDetector:new_ball:x", fb.position.x);
    MODIFY("Vision:FakeBallDetector:new_ball:y", fb.position.y);
    MODIFY("Vision:FakeBallDetector:new_ball:vx", fb.velocity.x);
    MODIFY("Vision:FakeBallDetector:new_ball:vy", fb.velocity.y);
    MODIFY("Vision:FakeBallDetector:new_ball:const_velocity", fb.const_velocity);

    DEBUG_REQUEST_ON_DEACTIVE("Vision:FakeBallDetector:add_new_ball",
        fakeBalls.push_back(fb);
    );

    DEBUG_REQUEST("Vision:FakeBallDetector:disable",
        lastFrame = getFrameInfo();
        return;
    );

    simulateMovementOnField(getFrameInfo().getTimeInSeconds()-lastFrame.getTimeInSeconds());
    lastFrame = getFrameInfo();

    provideMultiBallPercept();
}

void FakeBallDetector::simulateMovementOnField(double dt) {
    for(FakeBall& fb: fakeBalls) {
        if (fb.const_velocity) {
            fb.position += fb.velocity * dt;
            continue;
        }

        double time_until_vel_zero = 0;

        if(fb.velocity.abs() > 1e-5){
            time_until_vel_zero = -fb.velocity.abs() / getFieldInfo().ballDeceleration;
        }

        if(time_until_vel_zero > 1e-5) {
            Vector2d acceleration;
            acceleration = fb.velocity;
            acceleration.normalize();
            // ballDeceleration is negative so the deceleration will be in opposite direction of current velocity
            acceleration *= getFieldInfo().ballDeceleration;

            if(time_until_vel_zero < dt) {
                dt = time_until_vel_zero;
            }

            fb.position += acceleration * 0.5 * dt * dt + fb.velocity * dt;
            fb.velocity += acceleration * dt;
        }
    }
}

// TODO: provide MultiBallPercept::BallPercept::radiusInImage
void FakeBallDetector::provideMultiBallPercept() const {
    MultiBallPercept::BallPercept ballPercept;
    Vector2d pointInImage;
    Vector3d point(0, 0, getFieldInfo().ballRadius);
    for(const FakeBall& fb: fakeBalls) {
        point.x = fb.position.x;
        point.y = fb.position.y;

        bool in_image_bottom = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), point, pointInImage);
        if(in_image_bottom) {
            ballPercept.positionOnField = fb.position;
            ballPercept.centerInImage = pointInImage;
            ballPercept.cameraId = naoth::CameraInfo::Bottom;
            getMultiBallPercept().add(ballPercept);
        }

        bool in_image_top = CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfoTop(), point, pointInImage);
        if(in_image_top) {
            ballPercept.positionOnField = fb.position;
            ballPercept.centerInImage = pointInImage;
            ballPercept.cameraId = naoth::CameraInfo::Top;
            getMultiBallPercept().add(ballPercept);
        }

        if(in_image_bottom || in_image_top) {
            getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
        }
    }
}
