#include "TestFunctions.h"

Eigen::Vector2d Rosenbrock::operator ()(const Eigen::Matrix<double,2 , 1>& parameter) const {
    Eigen::Vector2d f;
    f << 10*(parameter(0,0)-parameter(1,0)*parameter(1,0)), 1 - parameter(1,0);

    return f;
}
