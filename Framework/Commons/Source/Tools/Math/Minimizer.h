#ifndef EIGEN_DONT_VECTORIZE
#define EIGEN_DONT_VECTORIZE
#endif

#ifndef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

#ifndef MINIMIZER_H
#define MINIMIZER_H

#include <Tools/Math/Common.h>
#include <Tools/Math/Vector2.h>

#include <Tools/naoth_eigen.h>

template<int numOfFunctions, int numOfParameter, class ErrorFunction>
class GaussNewtonMinimizer
{
public:
    GaussNewtonMinimizer();

    double error;

    bool NaNed(){
        return naned;
    }

    Eigen::Matrix<double, numOfParameter, 1> minimizeOneStep(ErrorFunction& errorFunction, double epsilon);
    Eigen::Matrix<double, numOfParameter, 1> minimizeOneStep(ErrorFunction& errorFunction, const Eigen::Matrix<double, numOfParameter, 1>& x, double epsilon);

private:
    bool naned;

    Eigen::Matrix<double, numOfFunctions, numOfParameter> determineJacobian(ErrorFunction& errorFunction, double epsilon);
    Eigen::Matrix<double, numOfFunctions, numOfParameter> determineJacobian(ErrorFunction& errorFunction, const Eigen::Matrix<double, numOfParameter, 1>& x, double epsilon);

};

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
GaussNewtonMinimizer<numOfFunctions, numOfParameters, ErrorFunction>::GaussNewtonMinimizer():
    error(0),
    naned(false)
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

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
Eigen::Matrix<double, numOfFunctions, numOfParameters> GaussNewtonMinimizer<numOfFunctions,  numOfParameters, ErrorFunction>::determineJacobian(ErrorFunction& errorFunction, const Eigen::Matrix<double, numOfParameters, 1>& x, double epsilon){

    Eigen::Matrix<double, 1, numOfParameters> parameterVector = Eigen::Matrix<double, 1, numOfParameters>::Zero();
    Eigen::Matrix<double, numOfFunctions, numOfParameters> mat;

    for(int p = 0; p < numOfParameters; ++p){
        if(p > 0) {
            parameterVector(p-1) = 0;
        }

        Eigen::Matrix<double, numOfFunctions,1> dg1,dg2;

        parameterVector(p) = epsilon;
        dg1 << errorFunction((x.transpose()+parameterVector).eval());

        parameterVector(p) = -epsilon;
        dg2 << errorFunction((x.transpose()+parameterVector).eval());

        mat.col(p) = (dg1-dg2) / (2*epsilon);
    }

    return mat;
}

template<int numOfFunctions, int numOfParameters, class ErrorFunction>
Eigen::Matrix<double, numOfParameters, 1> GaussNewtonMinimizer<numOfFunctions, numOfParameters, ErrorFunction>::minimizeOneStep(ErrorFunction& errorFunction, const Eigen::Matrix<double, numOfParameters, 1>& x, double epsilon){

    Eigen::Matrix<double, numOfFunctions, numOfParameters> dg = determineJacobian(errorFunction, x, epsilon);

    // g(x) - target
    Eigen::VectorXd w = errorFunction(x);

    error = w.transpose() * w;

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    //Vector2d z_GN = dg * (-w / (dg * dg));
    Eigen::Matrix<double, numOfParameters, 1> offset = -((dg*dg.transpose()).inverse()*dg*w);

    //beware the inverse!
    if(offset.hasNaN()){
        naned = true;
        return Eigen::Matrix<double, numOfParameters,1>::Zero();
    }

    double lambda = 0.005;
    //MODIFY("CameraMatrixCorrectorV2:lambda", lambda);
    if (offset.norm() > lambda) {
        offset = offset.normalized()*lambda;
    }

    return offset;
}

#endif // MINIMIZER_H
