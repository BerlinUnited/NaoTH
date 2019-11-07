#include "MultiUnifiedObstacleLocator.h"

MultiUnifiedObstacleLocator::MultiUnifiedObstacleLocator() {

}

MultiUnifiedObstacleLocator::~MultiUnifiedObstacleLocator() {

}

void MultiUnifiedObstacleLocator::execute() {
    // 0. clear obstacle model

    // 1. apply odometry
    for(Obstacle& obs : obstacles) {
        apply_odometry(obs);
    }

    // 2. predict if a motion model exists

    // 3. update using sensors
    // 3.1 Ultrasound
    // 3.2 Vision

    // 4. provide model
    provide_obstacle_model();

    // 5. remember stuff for next frame
    lastRobotOdometry = getOdometryData();
}

void MultiUnifiedObstacleLocator::provide_obstacle_model() {
    getObstacleModel().obstacleList = obstacles;
}

void MultiUnifiedObstacleLocator::apply_odometry(Obstacle &obs) {
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
    obs.center = odometryDelta * obs.center;
    obs.shape_points.transform(odometryDelta);
}
