/**
* @file UnscentedKalmanFilter.h
* 
* Declaration of class UnscentedKalmanFilter
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/ 

#ifndef _UNSCENTEDKALMANFILTER_H
#define _UNSCENTEDKALMANFILTER_H

#include <Eigen/Core>
#include <Eigen/StdVector>

template <class S>
class UKF 
{

    public:
        // covariances
        Eigen::Matrix<double,S::size,S::size> P;             // covariance matrix of current state
        Eigen::Matrix<double,S::size,S::size> Q;             // covariance matrix of process noise
        //Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise

    public:
        UKF()
        {
        }

    public:
        void reset()
        {
            P     = Eigen::Matrix<double,S::size,S::size>::Identity();
            state = S();
        }

        template<typename U>
        void predict(const U& u, double dt)
        {
            // transit the sigma points to the next state
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i) {
                (*i).predict(u, dt);
            }

            S mean = S::calcMean(sigmaPoints);

            // calculate new process covariance
            Eigen::Matrix<double, S::size, 2*S::size + 1> temp;
            for(size_t idx = 0; idx < sigmaPoints.size(); ++idx) {
                temp.col(static_cast<Eigen::Index>(idx)) = sigmaPoints[idx] - mean;
            }

            state = mean;
            P     =  1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose() /* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
        }

        template<typename M, typename Derived>
        void update(const M& z, const Eigen::MatrixBase<Derived>& R)
        {
            std::vector<M, Eigen::aligned_allocator<M> > sigmaMeasurements;
            sigmaMeasurements.reserve(2*S::size+1);

            // map sigma points to measurement space
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i) {
                sigmaMeasurements.push_back((*i).asMeasurement(z));
            }

            // calculate predicted measurement z (weighted mean of sigma points)
            M predicted_z = M::calcMean(sigmaMeasurements);

            // calculate current measurement covariance
            Eigen::Matrix<double, M::size, 2*S::size+1> temp;
            for(size_t idx = 0; idx < sigmaMeasurements.size(); ++idx) {
                temp.col(static_cast<Eigen::Index>(idx)) = sigmaMeasurements[idx] - predicted_z;
            }
            Eigen::Matrix<double,M::size,M::size> Pzz(1.0 / static_cast<double>(sigmaPoints.size()) * temp * (temp).transpose());

            // calculate state-measurement cross-covariance
            Eigen::Matrix<double, S::size, 2*S::size + 1> temp2;
            for(size_t idx = 0; idx < sigmaPoints.size(); ++idx) {
                temp2.col(static_cast<Eigen::Index>(idx)) = sigmaPoints[idx] - state;
            }
            Eigen::Matrix<double,S::size,M::size> Pxz(1.0 / static_cast<double>(sigmaPoints.size()) * temp2 * (temp).transpose());

            // apply measurement noise covariance
            Eigen::Matrix<double,M::size,M::size> Pvv = Pzz + R;
            // calculate kalman gain
            Eigen::Matrix<double,S::size,M::size> K = Pxz*Pvv.inverse();

            // calculate new state and covariance
            M z_innovation = z - predicted_z;

            S state_innovation(K*z_innovation);

            state = state_innovation + state;

            P -= K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
        }

    private:
        const double alpha  = 10E-3;
        const double kapa   = 0;
        const double beta   = 2;
        const double lambda = alpha * alpha * (S::size + kapa) - S::size;

        std::vector<S, Eigen::aligned_allocator<S> > sigmaPoints;

        // decomposition object just need to be constructed once
        Eigen::LLT<Eigen::Matrix<double,S::size,S::size> > choleskyDecompositionOfCov; // apply Q befor the process model

    public:
        S state;

        void generateSigmaPoints()
        {
            sigmaPoints.resize(2*S::size+1);

            sigmaPoints[2*S::size] = state;

            choleskyDecompositionOfCov.compute(P+Q);
            Eigen::Matrix<double,S::size,S::size> L = choleskyDecompositionOfCov.matrixL();

            for(size_t i = 0; i < S::size; i++)
            {
                S noise(std::sqrt(2*S::size) * L.col(static_cast<Eigen::Index>(i)));

                sigmaPoints[i]  = noise;
                sigmaPoints[i] += state;

                // generate "opposite" sigma point
                sigmaPoints[i + S::size]  = -noise;
                sigmaPoints[i + S::size] += state;
            }
        }
};

#endif // UNSCENTEDKALMANFILTER_H
