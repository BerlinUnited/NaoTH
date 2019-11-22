#include "MultiUnifiedObstacleLocator.h"

MultiUnifiedObstacleLocator::MultiUnifiedObstacleLocator() {
    DEBUG_REQUEST_REGISTER("MulitUnifiedObstacleLocator:debug:create_virtual_obstacle",
                           "Adds a virtual obstacle. Initial place and dimensions can be modified using DEBUGMODIFY",
                           false);
    DEBUG_REQUEST_REGISTER("MulitUnifiedObstacleLocator:debug:delete_all_obstacles",
                           "Deletes all obstacles hypotheses", false);
    DEBUG_REQUEST_REGISTER("MulitUnifiedObstacleLocator:debug:draw_obstacles",
                           "Draws all obstacles on the field", false);
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

    do_debug();
}

void MultiUnifiedObstacleLocator::provide_obstacle_model() {
    getObstacleModel().obstacleList = obstacles;
}

void MultiUnifiedObstacleLocator::apply_odometry(Obstacle &obs) {
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
    obs.center = odometryDelta * obs.center;
    obs.shape_points.transform(odometryDelta);
}

void MultiUnifiedObstacleLocator::do_debug() {
    double x = 1000;
    double y = 0;
    double size = 500;

    MODIFY("MulitUnifiedObstacleLocator:virtual_obstacle:x", x);
    MODIFY("MulitUnifiedObstacleLocator:virtual_obstacle:y", y);
    MODIFY("MulitUnifiedObstacleLocator:virtual_obstacle:size", size);


    DEBUG_REQUEST_ON_DEACTIVE("MulitUnifiedObstacleLocator:debug:create_virtual_obstacle",
        Obstacle obs;
        obs.center = Vector2d(x,y);
        obs.shape_points.add(-size/2, -size/2);
        obs.shape_points.add( size/2, -size/2);
        obs.shape_points.add( size/2,  size/2);
        obs.shape_points.add(-size/2,  size/2);
        obs.shape_points.add(-size/2, -size/2); // closing polygon
        obs.shape_points.transform(Pose2D(0.0, obs.center));

        obstacles.push_back(std::move(obs));
    );

    DEBUG_REQUEST("MulitUnifiedObstacleLocator:debug:delete_all_obstacles",
        obstacles.clear();
    );

    DEBUG_REQUEST("MulitUnifiedObstacleLocator:debug:draw_obstacles",
        FIELD_DRAWING_CONTEXT;
        for(const Obstacle& obs : obstacles) {
            auto points = obs.shape_points.getPoints();
            for(size_t i = 0; i+1 < points.size(); ++i) {
                LINE(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
            }

            LINE(points.back().x, points.back().y, points.front().x, points.front().y);
        }
    );

}
