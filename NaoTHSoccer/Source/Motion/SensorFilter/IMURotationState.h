#ifndef IMUROTATIONSTATE_H
#define IMUROTATIONSTATE_H

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UKFStateRotationBase.h"

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

// state in global reference frame
template <class M, int dim, int dim_cov = dim, int rotation_index = 0>
class State : public UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>{
    public: // constructors
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        State(const Eigen::MatrixBase<OtherDerived>& other)
            : UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>(other)
        { }

        // inital state (zero rotation, zero angular velocity)
        State(): UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>(){
        }

        // "down casting"
        State(const UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>& other):
            UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>(other)
        {}

    public: // operators
        // This method allows you to assign Eigen expressions to MyVectorType
        // TODO: needed?
        template<typename OtherDerived>
        State& operator=(const Eigen::MatrixBase <OtherDerived>& other)
        {
            this->UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>::operator=(other);
            return *this;
        }

    public: // accessors

        Eigen::Block<Eigen::Matrix<double,dim,1> > rotational_velocity(){
            return UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>::accessElements(3,0,3,1);
        }

        Eigen::Block<Eigen::Matrix<double,dim,1> > bias_rotational_velocity(){
            return UKFStateRotationBase<State<M, dim, dim_cov>, rotation_index, dim>::accessElements(6,0,3,1);
        }

    public:

        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(double dt) {
            // rotational_velocity to quaternion
            Eigen::Vector3d rotational_velocity = this->rotational_velocity();
            Eigen::Quaterniond rotation_increment;
            if(rotational_velocity.norm() > 0) {
                rotation_increment = Eigen::Quaterniond(Eigen::AngleAxisd(rotational_velocity.norm()*dt, rotational_velocity.normalized()));
            } else {
                // zero rotation quaternion
                rotation_increment = Eigen::Quaterniond(1,0,0,0);
            }

            // continue rotation assuming constant velocity
            // TODO: compare with rotation_increment*rotation which sounds more reasonable
            Eigen::Quaterniond new_rotation = this->getRotationAsQuaternion() * rotation_increment; // follows paper
            Eigen::AngleAxisd  new_angle_axis(new_rotation);

            this->rotation() = new_angle_axis.angle() * new_angle_axis.axis();

            this->rotational_velocity() = this->rotational_velocity() - this->bias_rotational_velocity();
            this->bias_rotational_velocity() = this->bias_rotational_velocity();
        }

        // state to measurement transformation function
        M asMeasurement() {
            // state to measurement function
            // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
            Eigen::Vector3d rotational_velocity_in_measurement_space = rotational_velocity();// + bias_rotational_velocity();

            // determine rotation around x and y axis
            Eigen::Quaterniond q;
            q.setFromTwoVectors(this->getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,-1)),Eigen::Vector3d(0,0,-1));
            Eigen::AngleAxisd temp(q);
            Eigen::Vector3d   rotation_in_measurement_space(temp.angle() * temp.axis());

            M return_val;
            return_val << rotation_in_measurement_space, rotational_velocity_in_measurement_space;

            return return_val;
        }
};

#endif // IMUROTATIONSTATE_H
