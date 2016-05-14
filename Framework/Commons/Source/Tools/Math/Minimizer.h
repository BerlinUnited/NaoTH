#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif

#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

#ifndef MINIMIZER_H
#define MINIMIZER_H

#include "Tools/Math/Vector2.h"
#include <Eigen/Eigen>

class ErrorFunction{
public:
    virtual double operator()(double, double) = 0;
};

template<int numOfFunctions, int numOfParameter>
class GaussNewtonMinimizer
{
public:
    GaussNewtonMinimizer();

    Eigen::Matrix<double, numOfParameter, 1> minimizeOneStep(ErrorFunction *errorFunction, double epsilon);

private:
    Eigen::Matrix<double, numOfFunctions, numOfParameter> determineJacobian(ErrorFunction *errorFunction, double epsilon);

};

template<int numOfFunctions, int numOfParameters>
GaussNewtonMinimizer<numOfFunctions, numOfParameters>::GaussNewtonMinimizer()
{

}

template<int numOfFunctions, int numOfParameters>
Eigen::Matrix<double, numOfFunctions, numOfParameters> GaussNewtonMinimizer<numOfFunctions,  numOfParameters>::determineJacobian(ErrorFunction* errorFunction, double epsilon){
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = (*errorFunction)( epsilon, 0);
    double dg12 = (*errorFunction)(-epsilon, 0);

    double dg21 = (*errorFunction)(0,  epsilon);
    double dg22 = (*errorFunction)(0, -epsilon);

    double dg1  = (dg11 - dg12) / (2 * epsilon);
    double dg2  = (dg21 - dg22) / (2 * epsilon);

    Eigen::Matrix<double, numOfFunctions, numOfParameters> mat;

    mat << dg1,dg2;

    return mat;
}

template<int numOfFunctions, int numOfParameters>
Eigen::Matrix<double, numOfParameters, 1> GaussNewtonMinimizer<numOfFunctions, numOfParameters>::minimizeOneStep(ErrorFunction* errorFunction, double epsilon){

    Eigen::Matrix<double, numOfParameters, 1> offset = Eigen::Matrix<double, numOfParameters, 1>::Zero();

    Eigen::Matrix<double, numOfFunctions, numOfParameters> dg = determineJacobian(errorFunction, epsilon);

    // g(x) - target
    double w = (*errorFunction)(offset(0), offset(1));

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Eigen::Matrix<double, numOfFunctions, numOfParameters> z_GN = -((dg.transpose()*dg).inverse()*dg.transpose()*w);

    //Vector2d z_GN = dg * (-w / (dg * dg));
    offset += z_GN;

    double lambda = 0.005;
    //MODIFY("CameraMatrixCorrectorV2:lambda", lambda);
    if (offset.norm() > lambda) {
        offset = offset.normalized()*lambda;
    }

    return offset;
}

#endif // MINIMIZER_H
