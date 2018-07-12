#ifndef LEVENBERG_MARQUARDT_MINIMIZER_H
#define LEVENBERG_MARQUARDT_MINIMIZER_H

#include <Tools/Math/Common.h>
#include <Tools/Math/Vector2.h>

#include <Tools/naoth_eigen.h>

#include <tuple>

template<class ErrorFunction>
class LevenbergMarquardtMinimizer
{
public:
    double error;

private:
    double init_lambda;
    double init_v;

    double lambda;
    double v;

    bool naned;

    const double beta;
    const double gamma;
    const double p;

// (optional) TODO: stopping criteria || J*r || < eps1, || offset || < eps2 * x, iter > iter_max
public:

    // init_lambda: > 0, the smaller this value the more we believe that we are already close to the optimum (closeness)
    LevenbergMarquardtMinimizer(double init_lambda):
        error(0),
        init_lambda(init_lambda),
        init_v(2),
        naned(false),
        beta(2),
        gamma(2),
        p(3)
    {
        lambda = init_lambda;
        v = init_v;
    }

    void reset(){
        lambda = init_lambda;
        v      = init_v;
        naned  = false;
        error  = 0;
    }

    bool NaNed(){
        return naned;
    }

    // first version
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

    //Based on H.B. Nielson, Damping Parameter In Marquardt’s Method, Technical Report IMM-REP-1999-05, Dept. of Mathematical Modeling, Technical University Denmark.
    template<class T>
    bool minimizeOneStep2(const ErrorFunction& errorFunction, const T& x, const T& epsilon, T& offset)
    {
        Eigen::VectorXd r = errorFunction(x);
        Eigen::MatrixXd J = determineJacobian(errorFunction, x, epsilon);
        Eigen::MatrixXd JtJ = J.transpose()*J;
        Eigen::MatrixXd JtJdiag = (JtJ).diagonal().asDiagonal();
        error = r.transpose() * r;

        auto a = (JtJ + lambda * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();

        if(a.hasNaN()){
            naned = true;
            return false;
        }

        offset = mapOffsetToXDims(epsilon, a);

        Eigen::VectorXd r_new = errorFunction(x + offset);

        double g;
        g = (r.transpose() * r - r_new.transpose() * r_new)(0,0) / (offset.transpose() * (lambda * JtJdiag * offset - J.transpose()*r))(0,0);

        if(g>0) {
            lambda *= std::max(1/gamma, 1-(beta-1)*std::pow(2*g-1,p)); // update strategy with beta = v = gamma = 2, p =3
            v = beta;
        } else {
            lambda *= v;
            v *= 2;
        }

        return true;
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
