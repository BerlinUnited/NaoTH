#ifndef MINIMIZER_H
#define MINIMIZER_H

#include <Tools/naoth_eigen.h>

template<int numOfFunctions, int numOfParameter, class ErrorFunction>
class GaussNewtonMinimizer
{
public:
    GaussNewtonMinimizer();

    Eigen::Matrix<double, numOfParameter, 1> minimizeOneStep(ErrorFunction& errorFunction, double epsilon);

private:
    Eigen::Matrix<double, numOfFunctions, numOfParameter> determineJacobian(ErrorFunction& errorFunction, double epsilon);

};

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
GaussNewtonMinimizer<numOfFunctions, numOfParameters, ErrorFunction>::GaussNewtonMinimizer()
{
}

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
Eigen::Matrix<double, numOfFunctions, numOfParameters> GaussNewtonMinimizer<numOfFunctions,  numOfParameters, ErrorFunction>::determineJacobian(ErrorFunction& errorFunction, double epsilon){

    Eigen::Matrix<double, 1, numOfParameters> parameterVector = Eigen::Matrix<double, 1, numOfParameters>::Zero();
    Eigen::Matrix<double, numOfFunctions, numOfParameters> mat;

    for(int p = 0; p < numOfParameters; ++p){
        if(p > 0) {
            parameterVector(p-1) = 0;
        }

        Eigen::Matrix<double, numOfFunctions,1> dg1,dg2;

        parameterVector(p) = epsilon;
        dg1 << errorFunction(parameterVector);

        parameterVector(p) = -epsilon;
        dg2 << errorFunction(parameterVector);

        mat.col(p) = (dg1-dg2) / (2*epsilon);
    }

    return mat;
}

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
Eigen::Matrix<double, numOfParameters, 1> GaussNewtonMinimizer<numOfFunctions, numOfParameters, ErrorFunction>::minimizeOneStep(ErrorFunction& errorFunction, double epsilon){

    Eigen::Matrix<double, numOfParameters, 1> offset = Eigen::Matrix<double, numOfParameters, 1>::Zero();

    Eigen::Matrix<double, numOfFunctions, numOfParameters> dg = determineJacobian(errorFunction, epsilon);

    // g(x) - target
    double w = errorFunction(offset);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Eigen::Matrix<double, numOfFunctions, numOfParameters> z_GN = -((dg*dg.transpose()).inverse()*dg*w);

    //beware the inverse!
    assert(!z_GN.hasNaN());

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
