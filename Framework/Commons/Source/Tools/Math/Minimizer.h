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

#include <tuple>

template<class ErrorFunction>
class GaussNewtonMinimizer
{
public:
    GaussNewtonMinimizer(){}

    // TODO: fix dimension problem
    template<class T>
    std::tuple<Eigen::Matrix<double, 1, T::ColsAtCompileTime>, double > minimizeOneStep(ErrorFunction& errorFunction, T epsilon){
        Eigen::Matrix<double, 1, T::ColsAtCompileTime> offset(Eigen::Matrix<double, 1, T::ColsAtCompileTime>::Zero());

        Eigen::Matrix<double, T::RowsAtCompileTime, T::ColsAtCompileTime> dg = determineJacobian(errorFunction, epsilon);

        // g(x) - target
        double w = errorFunction(offset);

        //Eigen::Matrix<double, T::RowsAtCompileTime, T::ColsAtCompileTime> z_GN = (-((dg.transpose()*dg).inverse()*dg.transpose()*w));
        Eigen::Matrix<double, T::RowsAtCompileTime, T::ColsAtCompileTime> z_GN = -((dg*dg.transpose()).inverse()*dg*w);

        //beware the inverse!
        assert(!z_GN.hasNaN());

        //Vector2d z_GN = dg * (-w / (dg * dg));
        offset += z_GN;

        /*double lambda = 0.005;
        //MODIFY("CameraMatrixCorrectorV2:lambda", lambda);
        if (offset.norm() > lambda) {
            offset = offset.normalized()*lambda;
        }*/

        return std::make_tuple(offset, w);
    }

private:
    template<class T>
    Eigen::Matrix<double, T::RowsAtCompileTime,T::ColsAtCompileTime> determineJacobian(ErrorFunction& errorFunction, T epsilon){
        Eigen::Matrix<double, 1, T::ColsAtCompileTime> parameterVector = Eigen::Matrix<double, 1, T::ColsAtCompileTime>::Zero();
        Eigen::Matrix<double, T::RowsAtCompileTime, T::ColsAtCompileTime> mat;

        for(int p = 0; p < T::ColsAtCompileTime; ++p){
            if(p > 0) {
                parameterVector(p-1) = 0;
            }

            Eigen::Matrix<double, T::RowsAtCompileTime,1> dg1,dg2;

            parameterVector(p) = epsilon(p);
            dg1 << errorFunction(parameterVector);

            parameterVector(p) = -epsilon(p);
            dg2 << errorFunction(parameterVector);

            mat.col(p) = (dg1-dg2) / (2*epsilon(p));
        }

        return mat;
    }

};

#endif // MINIMIZER_H
