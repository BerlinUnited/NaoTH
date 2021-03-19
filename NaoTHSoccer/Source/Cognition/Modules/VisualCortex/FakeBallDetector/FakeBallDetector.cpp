#include "FakeBallDetector.h"

FakeBallDetector::FakeBallDetector() {
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:disable", "disable fake balls", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:clear", "clear all balls", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:add_new_ball", "add a new ball - position, velocity can be set by Vision:FakeBallDetector:new_ball before creation", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:ignore_image_size", "ignore the image width and height during back projection", false);
    DEBUG_REQUEST_REGISTER("Vision:FakeBallDetector:draw_fake_balls", "draw the fake balls on the field", false);
}

void FakeBallDetector::execute() {
    getMultiBallPercept().reset();

    DEBUG_REQUEST("Vision:FakeBallDetector:clear",
        clearFakeBalls();
    );

    FakeBall fb;
    static double mean = 0;
    static Vector2d std(std::sqrt(8e-4), std::sqrt(8e-4));
    static double detection_rate = 0.9;
    MODIFY("Vision:FakeBallDetector:new_ball:x", fb.position.x);
    MODIFY("Vision:FakeBallDetector:new_ball:y", fb.position.y);
    MODIFY("Vision:FakeBallDetector:new_ball:vx", fb.velocity.x);
    MODIFY("Vision:FakeBallDetector:new_ball:vy", fb.velocity.y);
    MODIFY("Vision:FakeBallDetector:new_ball:const_velocity", fb.const_velocity);
    MODIFY("Vision:FakeBallDetector:new_ball:enable_pixel_noise", fb.enabled_pixel_noise);
    MODIFY("Vision:FakeBallDetector:new_ball:enable_detection_noise", fb.enabled_detection_noise);
    MODIFY("Vision:FakeBallDetector:new_ball:noise:pixel_mean", mean);
    MODIFY("Vision:FakeBallDetector:new_ball:noise:pixel_std_x", std.x);
    MODIFY("Vision:FakeBallDetector:new_ball:noise:pixel_std_y", std.y);
    MODIFY("Vision:FakeBallDetector:new_ball:noise:detection_rate", detection_rate);

    DEBUG_REQUEST_ON_DEACTIVE("Vision:FakeBallDetector:add_new_ball",
        fb.top_pixel_noise.x.dist = std::normal_distribution<double>(mean, std.x);
        fb.top_pixel_noise.y.dist = std::normal_distribution<double>(mean, std.y);
        fb.bottom_pixel_noise.x.dist = std::normal_distribution<double>(mean, std.x);
        fb.bottom_pixel_noise.y.dist = std::normal_distribution<double>(mean, std.y);
        fb.detected_in_top.dist = std::bernoulli_distribution(detection_rate);
        fb.detected_in_bottom.dist = std::bernoulli_distribution(detection_rate);
        addFakeBall(fb);
    );

    DEBUG_REQUEST("Vision:FakeBallDetector:disable",
        lastFrame = getFrameInfo();
        return;
    );

    simulateMovementOnField(getFrameInfo().getTimeInSeconds()-lastFrame.getTimeInSeconds());
    lastFrame = getFrameInfo();

    DEBUG_REQUEST("Vision:FakeBallDetector:draw_fake_balls",
        FIELD_DRAWING_CONTEXT;
        PEN("8b0000FF", 20);
        for(const FakeBall& f: fakeBalls){
            CIRCLE(f.position.x, f.position.y, getFieldInfo().ballRadius-10);
            ARROW(f.position.x, f.position.y,
                  f.position.x + f.velocity.x,
                  f.position.y + f.velocity.y);
        }

        PEN("7f7f7fFF", 20);
        CIRCLE(fb.position.x, fb.position.y, getFieldInfo().ballRadius-10);
        ARROW(fb.position.x, fb.position.y,
              fb.position.x + fb.velocity.x,
              fb.position.y + fb.velocity.y);
    );

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
void FakeBallDetector::provideMultiBallPercept() {
    MultiBallPercept::BallPercept ballPercept;
    Vector2d pointInImage;
    Vector3d point(0, 0, getFieldInfo().ballRadius);
    bool ignore_image_size = false;

    DEBUG_REQUEST("Vision:FakeBallDetector:ignore_image_size",
        ignore_image_size = true;
    );

    for(FakeBall& fb: fakeBalls) {
        point.x = fb.position.x;
        point.y = fb.position.y;

        bool in_image_bottom = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), point, pointInImage);
        if(fb.enabled_pixel_noise)
            pointInImage += fb.bottom_pixel_noise();

        if(in_image_bottom
            && (! fb.enabled_detection_noise || fb.detected_in_bottom())
            && (ignore_image_size
                || ( 0 <= pointInImage.x && pointInImage.x <= getCameraInfo().resolutionWidth
                    && 0 <= pointInImage.y && pointInImage.y <= getCameraInfo().resolutionHeight)))
        {
            CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(),
                                                   pointInImage.x, pointInImage.y,
                                                   getFieldInfo().ballRadius, ballPercept.positionOnField);
            ballPercept.centerInImage = pointInImage;
            ballPercept.cameraId = naoth::CameraInfo::Bottom;
            getMultiBallPercept().add(ballPercept);
        }

        bool in_image_top = CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfoTop(), point, pointInImage);
        if(fb.enabled_pixel_noise)
            pointInImage += fb.top_pixel_noise();

        if(in_image_top
            && (! fb.enabled_detection_noise || fb.detected_in_top())
            && (ignore_image_size
                || ( 0 <= pointInImage.x && pointInImage.x <= getCameraInfoTop().resolutionWidth
                    && 0 <= pointInImage.y && pointInImage.y <= getCameraInfoTop().resolutionHeight)))
        {
            CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(),
                                                   pointInImage.x, pointInImage.y,
                                                   getFieldInfo().ballRadius, ballPercept.positionOnField);
            ballPercept.centerInImage = pointInImage;
            ballPercept.cameraId = naoth::CameraInfo::Top;
            getMultiBallPercept().add(ballPercept);
        }

        if(in_image_bottom || in_image_top) {
            getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
        }
    }
}
