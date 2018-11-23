#ifndef _OPTIMIZER_H
#define _OPTIMIZER_H

#include "Eigen/Eigen"

namespace Optimizer {

template<class ErrorFunction, class T>
class GaussNewtonMinimizer
{
public:
    double error;

protected:
    double init_lambda;
    double lambda;
    double v;
    double max_step_size;
    bool   failed;

private:
    bool enforce_reduction; // this is GN specific because a reduction isn't enforced by the algorithm itself

public:
    GaussNewtonMinimizer():
        error(0),
        init_lambda(1),
        lambda(init_lambda),
        v(1.25),
        max_step_size(0.005),
        failed(false),
        enforce_reduction(false)
    {}

    GaussNewtonMinimizer(double init_lambda, double v, double max_step_size, bool enforce_reduction):
        error(0),
        init_lambda(init_lambda),
        lambda(init_lambda),
        v(v),
        max_step_size(max_step_size),
        failed(false),
        enforce_reduction(enforce_reduction)
    {}

    virtual std::string getName() {
        return "Gauss-Newton";
    }

    virtual void reset(){
        lambda = init_lambda;
        error  = 0;
        failed = false;
    }

    bool step_failed(){
        return failed;
    }

    virtual bool minimizeOneStep(const ErrorFunction& errorFunction, const T& x, const T& epsilon, T& offset){
        Eigen::MatrixXd J = determineJacobian(errorFunction, x, epsilon);
        Eigen::VectorXd r = errorFunction(x);

        double current_error = r.transpose() * r;

        auto a = (J.transpose() * J).colPivHouseholderQr().solve(-J.transpose() * r).eval();

        if(a.hasNaN()){
            failed = true;
            return false;
        }

        offset = mapOffsetToXDims(epsilon, a);

        if (max_step_size > 0 && offset.norm() > max_step_size) {
            offset = offset.normalized()*max_step_size;
        }

        if(enforce_reduction){
            offset *= lambda;
            r = errorFunction(x+offset);
            error = r.transpose() * r;
            if(error > current_error){
                lambda /= v;
                return false;
            } else {
                lambda = init_lambda;
            }
        } else {
            error = current_error;
        }

        return true;
    }

protected:
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

    Eigen::MatrixXd determineJacobian(const ErrorFunction& errorFunction, const T& x, const T& epsilon)
    {
        Eigen::Matrix<double, T::RowsAtCompileTime, 1> parameterVector = Eigen::Matrix<double, T::RowsAtCompileTime, 1>::Zero();
        Eigen::MatrixXd mat(errorFunction.getNumberOfResudials(), epsilon.rows() - epsilon.array().isNaN().count());

        Eigen::VectorXd dg1(errorFunction.getNumberOfResudials());
        Eigen::VectorXd dg2(errorFunction.getNumberOfResudials());

        int idx = 0;
        for(int p = 0; p < T::RowsAtCompileTime; ++p)
        {
            if (std::isnan(epsilon(p))) continue;

            parameterVector(p) = epsilon(p);
            dg1 << errorFunction((x + parameterVector).eval());
            dg2 << errorFunction((x - parameterVector).eval());

            mat.col(idx) = (dg1-dg2) / (2*epsilon(p));
            ++idx;

            parameterVector(p) = 0;
        }

        return mat;
    }
};

template<class ErrorFunction, class T>
class LevenbergMarquardtMinimizer : public GaussNewtonMinimizer<ErrorFunction, T>
{
// (optional) TODO: stopping criteria || J*r || < eps1, || offset || < eps2 * x, iter > iter_max
public:
    LevenbergMarquardtMinimizer():
        GaussNewtonMinimizer<ErrorFunction, T>(1, 2, -1.0, false)
    {
    }

    // init_lambda: > 0, the smaller this value the more we believe that we are already close to the optimum (closeness)
    LevenbergMarquardtMinimizer(double init_lambda, double v, double max_step_size):
        GaussNewtonMinimizer<ErrorFunction, T>(init_lambda, v, max_step_size, false)
    {
    }

    virtual std::string getName() {
        return "Levenberg-Marquardt";
    }

    virtual void reset(){
        GaussNewtonMinimizer<ErrorFunction, T>::reset();
    }

    virtual bool minimizeOneStep(const ErrorFunction& errorFunction, const T& x, const T& epsilon, T& offset)
    {
        Eigen::VectorXd r = errorFunction(x);
        Eigen::MatrixXd J = this->determineJacobian(errorFunction, x, epsilon);
        Eigen::MatrixXd JtJ = J.transpose()*J;
        Eigen::MatrixXd JtJdiag = (JtJ).diagonal().asDiagonal();
        this->error = r.transpose() * r;

        auto a = (JtJ + this->lambda   * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();
        auto b = (JtJ + this->lambda/this->v * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();

        if(a.hasNaN() || b.hasNaN()){
            this->failed = true;
            return false;
        }

        T offset1 = this->mapOffsetToXDims(epsilon, a);
        T offset2 = this->mapOffsetToXDims(epsilon, b);

        Eigen::VectorXd r_o1 = errorFunction((x + offset1).eval());
        Eigen::VectorXd r_o2 = errorFunction((x + offset2).eval());

        double e_r_o1 = r_o1.transpose() * r_o1;
        double e_r_o2 = r_o2.transpose() * r_o2;

        if(e_r_o1 > this->error && e_r_o2 > this->error)
        {
            this->lambda *= this->v; // didn't get a better result so increase damping and retry
        } else if(e_r_o1 > e_r_o2) {
            this->lambda /= this->v; // got better result with decreased damping
            this->error = e_r_o2;
            offset = offset2;
            if (this->max_step_size > 0 && offset.norm() > this->max_step_size) {
                offset = offset.normalized()*this->max_step_size;
            }
            return true;
        } else {
            this->error = e_r_o1;
            offset = offset1;
            if (this->max_step_size > 0 && offset.norm() > this->max_step_size) {
                offset = offset.normalized()*this->max_step_size;
            }
            return true; // got better result with current damping
        }

        return false;
    }
};

//Based on H.B. Nielson, Damping Parameter In Marquardt’s Method, Technical Report IMM-REP-1999-05, Dept. of Mathematical Modeling, Technical University Denmark.
template<class ErrorFunction, class T>
class LevenbergMarquardtMinimizer2 : public LevenbergMarquardtMinimizer<ErrorFunction, T>
{
protected:
    double init_v;

    const double beta;
    const double gamma;
    const double p;

// (optional) TODO: stopping criteria || J*r || < eps1, || offset || < eps2 * x, iter > iter_max
public:
    LevenbergMarquardtMinimizer2():
        LevenbergMarquardtMinimizer<ErrorFunction, T>(),
        init_v(2),
        beta(2),
        gamma(2),
        p(3)
    {}

    // init_lambda: > 0, the smaller this value the more we believe that we are already close to the optimum (closeness)
    LevenbergMarquardtMinimizer2(double init_lambda, double init_v, double max_step_size, double beta, double gamma, double p):
        LevenbergMarquardtMinimizer<ErrorFunction, T>(init_lambda, init_v, max_step_size),
        init_v(init_v),
        beta(beta),
        gamma(gamma),
        p(p)
    {}

    virtual std::string getName() {
        return "Levenberg-Marquardt-2";
    }

    virtual bool minimizeOneStep(const ErrorFunction& errorFunction, const T& x, const T& epsilon, T& offset)
    {
        Eigen::VectorXd r = errorFunction(x);
        Eigen::MatrixXd J = this->determineJacobian(errorFunction, x, epsilon);
        Eigen::MatrixXd JtJ = J.transpose()*J;
        Eigen::MatrixXd JtJdiag = (JtJ).diagonal().asDiagonal();
        this->error = r.transpose() * r;

        auto a = (JtJ + this->lambda * JtJdiag).colPivHouseholderQr().solve(-J.transpose() * r).eval();

        if(a.hasNaN()){
            this->failed = true;
            return false;
        }

        offset = this->mapOffsetToXDims(epsilon, a);

        Eigen::VectorXd r_new = errorFunction(x + offset);

        double g;
        g = (r.transpose() * r - r_new.transpose() * r_new)(0,0) / (offset.transpose() * (this->lambda * JtJdiag * offset - J.transpose()*r))(0,0);

        if(g>0) {
            this->lambda *= std::max(1/gamma, 1-(beta-1)*std::pow(2*g-1,p)); // update strategy with beta = v = gamma = 2, p =3
            this->v = beta;

            if (this->max_step_size > 0 && offset.norm() > this->max_step_size) {
                offset = offset.normalized()*this->max_step_size;
            }

            return true;
        } else {
            this->lambda *= this->v;
            this->v *= 2;
        }

        return false;
    }

    virtual void reset(){
        LevenbergMarquardtMinimizer<ErrorFunction, T>::reset();
        this->v = init_v;
    }
};

}

#endif // _OPTIMIZER_H
