#ifndef UNSCENTEDKALMANFILTER_H
#define UNSCENTEDKALMANFILTER_H

#include <Tools/naoth_eigen.h>

#include <vector>
#include <Eigen/StdVector>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S>
class UKF {
    public:
// TODO: enable different updates like only acceleration and rotation or all three
//        enum class UpdateType{
//            acc_rot_rotvel,
//            acc_rot
//        };

    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        UKF():
            Q(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity()),
            P(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity())
        {
            // bias_acceleration (x,y,z) [m/s^2], too small?
//            Q(9,9)   = 10e-10;
//            Q(10,10) = 10e-10;
//            Q(11,11) = 10e-10;

            // rotation (x,y,z) [rad]?
            Q(0,0) = 0.01;
            Q(1,1) = 0.01;
            Q(2,2) = 0.01;

            // rotational_velocity (x,y,z) [rad/s], too small?
            Q(3,3) = 0.1;
            Q(4,4) = 0.1;
            Q(5,5) = 0.1;

            // bias_rotational_velocity (x,y,z) [m/s^2], too small?
            Q(6,6) = 10e-10;
            Q(7,7) = 10e-10;
            Q(8,8) = 10e-10;

            // set covariance matrix of measurement noise
            // measured covariance of acceleration and rotational velocity (motion log, 60 seconds)
//            R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, 0     , 0     , 0     , -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,
//                -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, 0     , 0     , 0     , -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,
//                 1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04, 0     , 0     , 0     ,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,
//                 0                       ,  0                       ,  0                       , 10e-11, 0     , 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 10e-11, 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 0     , 10e-11,  0                       ,  0                       ,  0                       ,
//                -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07, 0     , 0     , 0     ,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
//                -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, 0     , 0     , 0     , -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
//                -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07, 0     , 0     , 0     ,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

            // synthetic measurement covariance matrix used for testing
            R << 10e-5, 0, 0, 0, 0, 0,
                 0, 10e-5, 0, 0, 0, 0,
                 0, 0, 10e-5, 0, 0, 0,
                 0, 0, 0, 10e-5, 0, 0,
                 0, 0, 0, 0, 10e-5, 0,
                 0, 0, 0, 0, 0, 10e-5;
        }

    public:
        void reset(){
            P     = Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity();
            state = S();
        }

        void predict(double dt){
            // transit the sigma points to the next state
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                (*i).predict(dt);
            }

            S mean = S::calcMean(sigmaPoints);

            // calculate new process covariance
            Eigen::Matrix<double, dim_state_cov, dim_state_cov> cov = Eigen::Matrix<double, dim_state_cov, dim_state_cov>::Zero();
            S temp;

            for(typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
                temp = (*i) - mean;
                cov += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            //assert (mean.allFinite());
            //assert (cov.allFinite());
            state = mean;
            P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
        }

        template<typename M>
        void update(M& z){
            std::vector<M, Eigen::aligned_allocator<M> > sigmaMeasurements;

            // map sigma points to measurement space
            for (typename std::vector<S, Eigen::aligned_allocator<S> >::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                sigmaMeasurements.push_back((*i).asMeasurement(z));
            }

            // calculate predicted measurement z (weighted mean of sigma points)
            M predicted_z = M::calcMean(sigmaMeasurements);

            // calculate current measurement covariance
            Eigen::MatrixXd Pzz = Eigen::MatrixXd::Zero(z.rows(),z.rows());
            //        Eigen::MatrixXd rotational_differences;
            //        rotational_differences.resize(3,rotations.size());
            M temp;

            for(typename std::vector<M, Eigen::aligned_allocator<M> >::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
                temp = (*i) - predicted_z;
                Pzz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            // apply measurement noise covariance
            // TODO: use correct measurement noise covariance matrix R
            Eigen::MatrixXd Pvv = Pzz/* + R*/;

            // calculate state-measurement cross-covariance
            Eigen::MatrixXd Pxz = Eigen::MatrixXd::Zero(state.rows(),z.rows());
            S temp1;
            M temp2;

            for(unsigned int i = 0; i < sigmaPoints.size(); i++){
                temp1 = sigmaPoints[i] - state;
                temp2 = sigmaMeasurements[i] - predicted_z;

                Pxz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp1) * (temp2).transpose();
            }

            // calculate kalman gain
            Eigen::MatrixXd K = Pxz*Pvv.inverse();

            // calculate new state and covariance
            M z_innovation = z - predicted_z;

            S state_innovation(K*z_innovation);

            state = state_innovation + state;

            //assert (state.allFinite());

            Eigen::Matrix<double,dim_state_cov,dim_state_cov> P_wiki;
            P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter

            P = P_wiki;
            //assert (P.allFinite());
        }

    private:
        const double alpha  = 10E-3;
        const double kapa   = 0;
        const double beta   = 2;
        const double lambda = alpha * alpha * (dim_state + kapa) - dim_state;

        std::vector<S, Eigen::aligned_allocator<S> > sigmaPoints;

    public:
        S state;

        void generateSigmaPoints(){
            sigmaPoints.resize(2*dim_state_cov+1);

            sigmaPoints[2*dim_state_cov] = state;

            Eigen::LLT<Eigen::Matrix<double,dim_state_cov,dim_state_cov> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
            Eigen::Matrix<double,dim_state_cov,dim_state_cov> L = choleskyDecompositionOfCov.matrixL();

            //assert (L.allFinite());

            for(int i = 0; i < dim_state_cov; i++){
                S noise(std::sqrt(2*dim_state_cov) * L.col(i));

                sigmaPoints[i]  = noise;
                sigmaPoints[i] += state;

                // generate "opposite" sigma point
                sigmaPoints[i + dim_state_cov]  = -noise;
                sigmaPoints[i + dim_state_cov] += state;
            }
        }

        // covariances
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> Q;         // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> P;         // covariance matrix of current state

};

#endif // UNSCENTEDKALMANFILTER_H
