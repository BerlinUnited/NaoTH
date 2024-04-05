#pragma once
#include "Tools/DataStructures/Spline.h"
#include "Tools/Math/CubicSpline.h"
class KickType {
   public:
    // X trajectory
    std::vector<double> t_X_Points;
    std::vector<double> f_X_Points;

    // Z trajectory
    std::vector<double> t_Z_Points;
    std::vector<double> f_Z_Points;
    KickType(std::vector<double> t_X, std::vector<double> f_X,
             std::vector<double> t_Z, std::vector<double> f_Z);
    KickType();
};
