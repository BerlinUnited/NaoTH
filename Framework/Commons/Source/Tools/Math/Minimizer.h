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

    template<class T>
    std::tuple<Eigen::Matrix<double, T::RowsAtCompileTime, 1>, double > minimizeOneStep(ErrorFunction& errorFunction, T epsilon){
        T zero = T::Zero();
        Eigen::VectorXd r = errorFunction(zero);
        Eigen::MatrixXd J = determineJacobian(errorFunction, epsilon);
        Eigen::Matrix<double, T::RowsAtCompileTime,1> offset = -(J.transpose()*J).inverse()*J.transpose()*r;

        //beware the inverse!
        assert(!offset.hasNaN());

        return std::make_tuple(offset, r.sum());
    }

private:
    template<class T>
    Eigen::Matrix<double, Eigen::Dynamic, T::RowsAtCompileTime> determineJacobian(ErrorFunction& errorFunction, T epsilon){
        Eigen::Matrix<double, T::RowsAtCompileTime, 1> parameterVector = Eigen::Matrix<double, T::RowsAtCompileTime, 1>::Zero();
        Eigen::Matrix<double, Eigen::Dynamic, T::RowsAtCompileTime> mat(errorFunction.getNumberOfResudials(), epsilon.rows());

        Eigen::VectorXd dg1(errorFunction.getNumberOfResudials());
        Eigen::VectorXd dg2(errorFunction.getNumberOfResudials());

        for(int p = 0; p < T::RowsAtCompileTime; ++p){
            if(p > 0) {
                parameterVector(p-1) = 0;
            }

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
