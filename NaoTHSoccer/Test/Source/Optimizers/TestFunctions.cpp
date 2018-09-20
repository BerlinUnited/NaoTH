#include "TestFunctions.h"

Eigen::Vector2d nielsen::Rosenbrock::operator ()(const Eigen::Matrix<double,2 , 1>& parameter) const {
    Eigen::Vector2d f;
    f << 10*(parameter(0,0)-parameter(1,0)*parameter(1,0)),
         1 - parameter(1,0);

    return f;
}


Eigen::Vector2d wikipedia::Himmelblau::operator ()(const Eigen::Vector2d& p) const {
    Eigen::Vector2d f;
    f << std::pow(p(0,0) * p(0,0) + p(1,0) - 11, 2),
         std::pow(p(0,0) + p(1,0) * p(1,0) -  7, 2);

    return f;
}
