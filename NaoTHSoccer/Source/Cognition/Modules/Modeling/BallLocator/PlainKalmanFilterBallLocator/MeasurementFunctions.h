#ifndef MEASUREMENTFUNCTIONS
#define MEASUREMENTFUNCTIONS


#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Infrastructure/CameraInfo.h>

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>

#include <Eigen/Eigen>

#include <Tools/CameraGeometry.h>

struct Measurement_Function_H {
    CameraMatrix camMat;
    naoth::CameraInfo camInfo;
    double ball_height;

    Eigen::Vector2d operator()(double x, double y) const{

        bool in_front_of_plane;

        Vector2d angles;
        Vector3d point(x, y, ball_height);
        Vector2d pointInImage;
        Eigen::Vector2d z;

        in_front_of_plane = CameraGeometry::relativePointToImage(camMat, camInfo, point, pointInImage);

        if(!in_front_of_plane) {
            z << std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity();
            return z;
        }

        angles = CameraGeometry::angleToPointInImage(camMat,camInfo,pointInImage.x,pointInImage.y);

        z << angles.x, angles.y;// angle horizontal, angle vertical

        return z;
    }
};

#endif // MEASUREMENTFUNCTIONS

