#ifndef UNSCENTEDKALMANFILTER_H
#define UNSCENTEDKALMANFILTER_H

#include <Tools/naoth_eigen.h>

#include <vector>
#include <Eigen/StdVector>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
#include <Eigen/Geometry>
#include <Eigen/Dense>

template <class S>
class UKF {

    public:
        // covariances
        Eigen::Matrix<double,S::size,S::size> P;             // covariance matrix of current state
        Eigen::Matrix<double,S::size,S::size> Q;             // covariance matrix of process noise
        //Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise

    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        UKF()
        {
        }

    public:
        void reset(){
            P     = Eigen::Matrix<double,S::size,S::size>::Identity();
            state = S();
        }

        template<typename U>
        void predict(U& u, double dt){
            // transit the sigma points to the next state
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                (*i).predict(u, dt);
            }

            S mean = S::calcMean(sigmaPoints);

            // calculate new process covariance
            Eigen::Matrix<double, S::size, S::size> cov = Eigen::Matrix<double, S::size, S::size>::Zero();
            S temp;

            for(typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
                temp = (*i) - mean;
                cov += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            state = mean;
            P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
        }

        template<typename M, typename Derived>
        void update(const M& z, const Eigen::MatrixBase<Derived>& R){
            std::vector<M, Eigen::aligned_allocator<M> > sigmaMeasurements;

            // map sigma points to measurement space
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                sigmaMeasurements.push_back((*i).asMeasurement(z));
            }

            // calculate predicted measurement z (weighted mean of sigma points)
            M predicted_z = M::calcMean(sigmaMeasurements);

            // calculate current measurement covariance
            Eigen::Matrix<double,M::size,M::size> Pzz(Eigen::Matrix<double, M::size,M::size>::Zero());
            M temp;

            for(typename std::vector<M, Eigen::aligned_allocator<M> >::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
                temp = (*i) - predicted_z;
                Pzz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            // apply measurement noise covariance
            Eigen::Matrix<double,M::size,M::size> Pvv = Pzz + R;

            // calculate state-measurement cross-covariance
            Eigen::Matrix<double,S::size,M::size> Pxz(Eigen::Matrix<double,S::size,M::size>::Zero());
            S temp1;
            M temp2;

            for(unsigned int i = 0; i < sigmaPoints.size(); i++){
                temp1 = sigmaPoints[i] - state;
                temp2 = sigmaMeasurements[i] - predicted_z;

                Pxz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp1) * (temp2).transpose();
            }

            // calculate kalman gain
            Eigen::Matrix<double,S::size,M::size> K = Pxz*Pvv.inverse();

            // calculate new state and covariance
            M z_innovation = z - predicted_z;

            S state_innovation(K*z_innovation);

            state = state_innovation + state;

            Eigen::Matrix<double,S::size,S::size> P_wiki;
            P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter

            P = P_wiki;
        }

    private:
        const double alpha  = 10E-3;
        const double kapa   = 0;
        const double beta   = 2;
        const double lambda = alpha * alpha * (S::size + kapa) - S::size;

        std::vector<S, Eigen::aligned_allocator<S> > sigmaPoints;

    public:
        S state;

        void generateSigmaPoints(){
            sigmaPoints.resize(2*S::size+1);

            sigmaPoints[2*S::size] = state;

            Eigen::LLT<Eigen::Matrix<double,S::size,S::size> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
            Eigen::Matrix<double,S::size,S::size> L = choleskyDecompositionOfCov.matrixL();

            for(int i = 0; i < S::size; i++){
                S noise(std::sqrt(2*S::size) * L.col(i));

                sigmaPoints[i]  = noise;
                sigmaPoints[i] += state;

                // generate "opposite" sigma point
                sigmaPoints[i + S::size]  = -noise;
                sigmaPoints[i + S::size] += state;
            }
        }

};

#pragma GCC diagnostic pop

#endif // UNSCENTEDKALMANFILTER_H
