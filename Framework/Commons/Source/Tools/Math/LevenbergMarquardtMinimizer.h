#ifndef LEVENBERG_MARQUARDT_MINIMIZER_H
#define LEVENBERG_MARQUARDT_MINIMIZER_H

#include <Tools/Math/Common.h>
#include <Tools/Math/Vector2.h>

#include <Tools/naoth_eigen.h>

#include <tuple>

template<class ErrorFunction, int init_lambda, int init_v>
class LevenbergMarquardtMinimizer
{
public:
    LevenbergMarquardtMinimizer(){
        lambda = init_lambda;
        v = init_v;
    }

    double lambda;
    double v;

    void reset(){
        lambda = init_lambda;
        v = init_v;
    }

    template<class T>
    T minimizeOneStep(const ErrorFunction& errorFunction, const T& x, const T& epsilon, double& error)
    {
        T zero = T::Zero();
        Eigen::VectorXd r = errorFunction(x);
        Eigen::MatrixXd J = determineJacobian(errorFunction, x, epsilon);
        Eigen::MatrixXd JtJ = J.transpose()*J;
        Eigen::MatrixXd JtJdiag = (JtJ).diagonal().asDiagonal();
        error = r.sum();

        auto a = (JtJ + lambda   * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();
        auto b = (JtJ + lambda/v * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();
        T offset1 = mapOffsetToXDims(epsilon, a);
        T offset2 = mapOffsetToXDims(epsilon, b);

        if(offset1.hasNaN() || offset2.hasNaN()){
            return zero;
        }

        double r_o1_sum = errorFunction((x + offset1).eval()).sum();
        double r_o2_sum = errorFunction((x + offset2).eval()).sum();

        if(r_o1_sum > error && r_o2_sum > error)
        {
            while(true){//for( int i = 0; i < 10; i++){ // retry at most 10 times
                lambda *= v; // didn't get a better result so increase damping and retry
                offset1 = mapOffsetToXDims(epsilon, (JtJ + lambda * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval());

                if(offset1.hasNaN()) {
                    return zero;
                }

                r_o1_sum = errorFunction((x + offset1).eval()).sum();

                if(r_o1_sum < error){
                    error = r_o1_sum;
                    return offset1;
                }
            }
        } else if(r_o1_sum > r_o2_sum) {
            lambda /= v; // got better result with decreased damping
            error = r_o2_sum;
            return offset2;
        } else {
            error = r_o1_sum;
            return offset1; // got better result with current damping
        }

        return zero;
    }

private:
    template<class T>
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> determineJacobian(const ErrorFunction& errorFunction, const T& x, const T& epsilon)
    {
        Eigen::Matrix<double, T::RowsAtCompileTime, 1> parameterVector = Eigen::Matrix<double, T::RowsAtCompileTime, 1>::Zero();
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> mat(errorFunction.getNumberOfResudials(), epsilon.rows() - epsilon.array().isNaN().count());

        Eigen::VectorXd dg1(errorFunction.getNumberOfResudials());
        Eigen::VectorXd dg2(errorFunction.getNumberOfResudials());

        int idx = 0;
        for(int p = 0; p < T::RowsAtCompileTime; ++p)
        {
            if (std::isnan(epsilon(p))) continue;

            parameterVector(p) = epsilon(p);
            dg1 << errorFunction((x + parameterVector).eval());

            parameterVector(p) = -epsilon(p);
            dg2 << errorFunction((x + parameterVector).eval());

            mat.col(idx) = (dg1-dg2) / (2*epsilon(p));
            ++idx;

            parameterVector(p) = 0;
        }

        return mat;
    }

    template<class T>
    T mapOffsetToXDims(const T& epsilon, const Eigen::Matrix<double, Eigen::Dynamic, 1>& offset)
    {
        T r;

        int idx = 0;
        for(int p = 0; p < T::RowsAtCompileTime; ++p) {
            if(std::isnan(epsilon(p))) {
                r(p) = 0;
            } else {
                r(p) = offset(idx);
                ++idx;
            }
        }

        return r;
    }
};

#endif // LEVENBERG_MARQUARDT_MINIMIZER_H
