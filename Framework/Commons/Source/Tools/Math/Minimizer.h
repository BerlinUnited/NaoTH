#ifndef MINIMIZER_H
#define MINIMIZER_H

#include "Tools/Math/Vector2.h"
#include <Eigen/Eigen>

class ErrorFunction{
public:
    virtual double operator()(double, double) = 0;
};

class GaussNewtonMinimizer
{
public:
    GaussNewtonMinimizer();

    Vector2d minimizeOneStep(ErrorFunction *errorFunction, double epsilon);

private:
    Vector2d determineJacobian(ErrorFunction *errorFunction, double epsilon);

};

#endif // MINIMIZER_H
