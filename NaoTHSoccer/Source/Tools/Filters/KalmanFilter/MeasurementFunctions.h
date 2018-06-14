#ifndef MEASUREMENTFUNCTIONS
#define MEASUREMENTFUNCTIONS


#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Infrastructure/CameraInfo.h>

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>

#include <Tools/naoth_eigen.h>

#include <Tools/CameraGeometry.h>

struct Measurement_Function_H 
{
    CameraMatrix camMat;
    naoth::CameraInfo camInfo;
    double ballRadius;

    Eigen::Vector2d operator()(double x, double y) const
    {
        Vector3d point(x, y, ballRadius);
        Vector2d angles = CameraGeometry::relativePointToCameraAngle(camMat, camInfo, point);

        Eigen::Vector2d z;
        z << angles.x, angles.y;// angle horizontal, angle vertical
        return z;
    }
};

#endif // MEASUREMENTFUNCTIONS

