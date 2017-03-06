#ifndef UNSCENTEDKALMANFILTER_H
#define UNSCENTEDKALMANFILTER_H

#include <Tools/naoth_eigen.h>

#include <vector>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S, class M>
class UKF {
    public:
// TODO: enable different updates like only acceleration and rotation or all three
//        enum class UpdateType{
//            acc_rot_rotvel,
//            acc_rot
//        };

    public:
        UKF():
            Q(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity()),
            P(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity())
        {

            // set covariance matrix of process noise
            // acceleration (x,y,z) [m/s^2]
            Q(0,0) = 0.01;
            Q(1,1) = 0.01;
            Q(2,2) = 0.01;

            // bias_acceleration (x,y,z) [m/s^2], too small?
//            Q(9,9)   = 10e-10;
//            Q(10,10) = 10e-10;
//            Q(11,11) = 10e-10;

            // rotation (x,y,z) [rad]?
            Q(3,3) = 0.001;
            Q(4,4) = 0.001;
            Q(5,5) = 0.001;

            // rotational_velocity (x,y,z) [rad/s], too small?
            Q(6,6) = 0.1;
            Q(7,7) = 0.1;
            Q(8,8) = 0.1;

            // bias_rotational_velocity (x,y,z) [m/s^2], too small?
//            Q(18,18) = 10e-10;
//            Q(19,19) = 10e-10;
//            Q(20,20) = 10e-10;

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
            R << 10e-5, 0, 0, 0, 0, 0, 0, 0,
                 0, 10e-5, 0, 0, 0, 0, 0, 0,
                 0, 0, 10e-5, 0, 0, 0, 0, 0,
                 0, 0, 0, 10e-5, 0, 0, 0, 0,
                 0, 0, 0, 0, 10e-5, 0, 0, 0,
                 0, 0, 0, 0, 0, 10e-5, 0, 0,
                 0, 0, 0, 0, 0, 0, 10e-5, 0,
                 0, 0, 0, 0, 0, 0, 0, 10e-5;

        }

    public:

        void reset(){
            P     = Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity();
            state = S();
        }

        void predict(double dt){
            // transit the sigma points to the next state
            for (typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                transitionFunction(*i,dt);
            }

            std::vector<Eigen::Quaterniond> rotations;
            S mean;
            // calculate new state (weighted mean of sigma points)
            for(typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
                rotations.push_back((*i).getRotationAsQuaternion());
                mean += (*i).scale(1.0 / static_cast<double>(sigmaPoints.size()));
            }

            // more correct determination of the mean rotation
            mean.rotation() = averageRotation(rotations, rotations.back());

            // calculate new process covariance
            Eigen::Matrix<double, dim_state_cov, dim_state_cov> cov = Eigen::Matrix<double, dim_state_cov, dim_state_cov>::Zero();
            Eigen::Matrix<double, dim_state_cov,1> temp;

            for(typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); ++i){
                temp = (*i) - mean;
                // replace wrong rotational difference
                Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * mean.getRotationAsQuaternion().inverse());
                temp.block(3,0,3,1) = error.angle() * error.axis();
                cov += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            //assert (mean.allFinite());
            //assert (cov.allFinite());
            state = mean;
            P     = cov/* + Q*/; // process covariance is applied before the process model (while generating the sigma points)
        }

        void update(M z){
            std::vector<M> sigmaMeasurements;

            // map sigma points to measurement space
            for (typename std::vector<S>::iterator i = sigmaPoints.begin(); i != sigmaPoints.end(); i++){
                sigmaMeasurements.push_back(stateToMeasurementSpaceFunction(*i));
            }

            // calculate predicted measurement z (weighted mean of sigma points)
            std::vector<Eigen::Quaterniond> rotations;
            M predicted_z;

            for(typename std::vector<M>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
                rotations.push_back((*i).getRotationAsQuaternion());
                predicted_z += 1.0 / static_cast<double>(sigmaMeasurements.size()) * (*i);
            }

            // more correct determination of the mean rotation
            Eigen::Vector3d rotational_mean = averageRotation(rotations, rotations.back());
            predicted_z.rotation() << rotational_mean(0), rotational_mean(1);

            // calculate current measurement covariance
            Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pzz = Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov>::Zero();

            for(typename std::vector<M>::iterator i = sigmaMeasurements.begin(); i != sigmaMeasurements.end(); ++i){
                Eigen::Matrix<double, dim_measurement_cov,1> temp((*i)- predicted_z);
                // replace wrong rotational difference
                Eigen::AngleAxis<double> error((*i).getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
                temp.block(3,0,2,1) << error.angle() * error.axis()(0), error.angle() * error.axis()(1);
                Pzz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp)*(temp).transpose();
            }

            // apply measurement noise covariance
            Eigen::Matrix<double, dim_measurement_cov, dim_measurement_cov> Pvv = Pzz + R;

            // calculate state-measurement cross-covariance
            Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> Pxz = Eigen::Matrix<double, dim_state_cov, dim_measurement_cov>::Zero();

            for(unsigned int i = 0; i < sigmaPoints.size(); i++){
                Eigen::Matrix<double, dim_state_cov,1> temp1(sigmaPoints[i] - state);
                // replace wrong rotational difference
                Eigen::AngleAxis<double> error(sigmaPoints[i].getRotationAsQuaternion() * state.getRotationAsQuaternion().inverse());
                temp1.block(3,0,3,1) = error.angle() * error.axis();

                Eigen::Matrix<double, dim_measurement_cov,1> temp2(sigmaMeasurements[i] - predicted_z);
                // replace wrong rotational difference
                error = Eigen::AngleAxis<double>(sigmaMeasurements[i].getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
                temp2.block(3,0,2,1) << error.angle() * error.axis()(0), error.angle() * error.axis()(1);

                Pxz += 1.0 / static_cast<double>(sigmaPoints.size()) * (temp1) * (temp2).transpose();
            }

            // calculate kalman gain
            Eigen::Matrix<double, dim_state_cov, dim_measurement_cov> K;
            K = Pxz*Pvv.inverse();

            // calculate new state and covariance

            Eigen::Matrix<double, dim_measurement_cov,1> z_innovation = z - predicted_z;
            // replace wrong rotational difference
            Eigen::AngleAxis<double> error(z.getRotationAsQuaternion() * predicted_z.getRotationAsQuaternion().inverse());
            z_innovation.block(3,0,2,1) << error.angle() * error.axis()(0), error.angle() * error.axis()(1);

            S state_innovation(K*z_innovation);

            // HACK: z rotation is only measured by gyrometer => so the fifth row of K should be almost zero except for two entries
            state_innovation(5,0) = 0.0;

            state += state_innovation;

            //assert (state.allFinite());

            Eigen::Matrix<double,dim_state_cov,dim_state_cov> P_wiki;
            P_wiki   = P - K*Pzz*K.transpose(); // https://en.m.wikipedia.org/wiki/Kalman_filter
            //P_gh     = P - Pxz*Pzz.inverse()*Pxz.transpose(); // from "Performance and Implementation Aspects of Nonlinear Filtering" by Gustaf Hendeby

            P = P_wiki;
            //assert (P.allFinite());
        }

    private:
        const double alpha  = 10E-3;
        const double kapa   = 0;
        const double beta   = 2;
        const double lambda = alpha * alpha * (dim_state + kapa) - dim_state;

        std::vector<S> sigmaPoints;

    public:
        S state;

        // state transition function
        void transitionFunction(S &state, double dt) {
            // rotational_velocity to quaternion
            Eigen::Vector3d rotational_velocity = state.rotational_velocity();
            Eigen::Quaterniond rotation_increment;
            if(rotational_velocity.norm() > 0) {
                rotation_increment = Eigen::Quaterniond(Eigen::AngleAxisd(rotational_velocity.norm()*dt, rotational_velocity.normalized()));
            } else {
                // zero rotation quaternion
                rotation_increment = Eigen::Quaterniond(1,0,0,0);
            }

            // continue rotation assuming constant velocity
            // TODO: compare with rotation_increment*rotation which sounds more reasonable
            Eigen::Quaterniond new_rotation = state.getRotationAsQuaternion() * rotation_increment; // follows paper
            Eigen::AngleAxisd new_angle_axis(new_rotation);

            state.rotation() = new_angle_axis.angle() * new_angle_axis.axis();

            state.acceleration() = rotation_increment.inverse()._transformVector(state.acceleration());
        }

        // state to measurement transformation function
        M stateToMeasurementSpaceFunction(S& state) {
            // state to measurement function
            // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
            Eigen::Vector3d acceleration_in_measurement_space        = state.acceleration() /*+ state.bias_acceleration()*/; // TODO: check for rotation._transformVector(...) ...
            Eigen::Vector3d rotational_velocity_in_measurement_space = state.rotational_velocity() /*+ state.bias_rotational_velocity()*/;

            // determine rotation around x and y axis
            Eigen::Quaterniond q;
            q.setFromTwoVectors(state.getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,-1)),Eigen::Vector3d(0,0,-1));
            Eigen::AngleAxisd temp(q);
            Eigen::Vector3d   rotation_in_measurement_space(temp.angle() * temp.axis());

            M return_val;
            return_val << acceleration_in_measurement_space, rotation_in_measurement_space(0), rotation_in_measurement_space(1), rotational_velocity_in_measurement_space;

            return return_val;
        }

        S measurementToStateSpaceFunction(M& z){
            S return_val;
            return_val << z.acceleration(), z.rotation(), 0, z.rotational_velocity();

            return return_val;
        }


        void generateSigmaPoints(){
            sigmaPoints.resize(2*dim_state_cov+1);

            sigmaPoints[2*dim_state_cov] = state;

            // HACK: ensure positiv semidefinity of P
            // P = 0.5*P+0.5*P.transpose();
            // P = P + 10E-4*Eigen::Matrix<double,dim_state,dim_state>::Identity();

            Eigen::LLT<Eigen::Matrix<double,dim_state_cov,dim_state_cov> > choleskyDecompositionOfCov(P+Q); // apply Q befor the process model
            Eigen::Matrix<double,dim_state_cov,dim_state_cov> L = choleskyDecompositionOfCov.matrixL();

            //assert (L.allFinite());

            for(int i = 0; i < dim_state_cov; i++){
                S noise(std::sqrt(2*dim_state_cov) * L.col(i));

                // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
                sigmaPoints[i]  = noise;
                sigmaPoints[i] += state;

                // generate "opposite" sigma point
                // TODO: check which order is correct (matters because of the rotation part), order as used in the paper
                sigmaPoints[i + dim_state_cov]  = -noise;
                sigmaPoints[i + dim_state_cov] += state;
            }
        }

        // covariances
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> Q;         // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> P;         // covariance matrix of current state

        // TODO: check whether this concept of averageing can be applied directly on the rotations (average axis and average angle)
        // TODO: adjust maximum of iterations
        Eigen::Vector3d averageRotation(std::vector<Eigen::Quaterniond> rotations, Eigen::Quaterniond mean){
            std::vector<Eigen::Vector3d> rotational_differences;

            for(int i = 0; i < 10; ++i){
                // calculate difference between the mean and the sigma points rotation by means of a rotation
                rotational_differences.clear();
                for (typename std::vector<Eigen::Quaterniond>::iterator i = rotations.begin(); i != rotations.end(); ++i){
                    Eigen::AngleAxis<double> rotational_difference = Eigen::AngleAxis<double>((*i) * mean.inverse());

                    // store as rotation vector representation
                    rotational_differences.push_back(rotational_difference.angle() * rotational_difference.axis());
                }

                // average difference quaternions in their 3d vectorial representation (length = angle, direction = axis)
                Eigen::Vector3d averaged_rotational_difference = Eigen::Vector3d::Zero();
                for (typename std::vector<Eigen::Vector3d >::iterator i = rotational_differences.begin(); i != rotational_differences.end(); ++i){
                    averaged_rotational_difference += *i;
                }
                averaged_rotational_difference = 1.0 / static_cast<double>(rotational_differences.size()) * averaged_rotational_difference;

                mean = Eigen::Quaterniond(Eigen::AngleAxis<double>(averaged_rotational_difference.norm(), averaged_rotational_difference.normalized())) * mean;
                if(averaged_rotational_difference.norm() < 10e-4){
                    break;
                }
            }

            Eigen::Vector3d mean_rotation_vector;
            mean_rotation_vector << Eigen::AngleAxisd(mean).angle() * Eigen::AngleAxisd(mean).axis();

            return mean_rotation_vector;
        }
};

#endif // UNSCENTEDKALMANFILTER_H
