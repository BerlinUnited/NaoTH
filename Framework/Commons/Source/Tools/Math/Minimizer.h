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
   Eigen::Matrix<double, T::RowsAtCompileTime, 1> minimizeOneStep(const ErrorFunction& errorFunction, const T& epsilon, double& error){
        T zero = T::Zero();
        Eigen::VectorXd r = errorFunction(zero);
        Eigen::MatrixXd J = determineJacobian(errorFunction, epsilon);
        //Eigen::Matrix<double, T::RowsAtCompileTime,1> offset = -(J.transpose()*J).inverse()*J.transpose()*r;
        Eigen::Matrix<double, T::RowsAtCompileTime,1> offset = (J.transpose()*J).ldlt().solve(-J.transpose() * r);

        //beware the inverse!
        assert(!offset.hasNaN());

        error = r.sum();
        return offset; // used std::make_tuple(offset,r.sum())>, but maybe problems with alignment of the eigen object offset?
    }

private:
    template<class T>
    Eigen::Matrix<double, Eigen::Dynamic, T::RowsAtCompileTime> determineJacobian(const ErrorFunction& errorFunction, const T& epsilon){
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
