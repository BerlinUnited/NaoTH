#ifndef IMUROTATIONSTATE_H
#define IMUROTATIONSTATE_H

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UKFStateRotationBase.h"
#include <Tools/naoth_eigen.h>
// TODO: remove pragma
/*
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wconversion"
#endif
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
*/

// state for rotation and rotational velocity
template <class M1,/* class M2,*/ int dim, int dim_cov = dim, int rotation_index = 0>
class RotationState : public UKFStateRotationBase<RotationState<M1,/* M2,*/ dim, dim_cov>, rotation_index, dim>{
    public: // constructors
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        RotationState(const Eigen::MatrixBase<OtherDerived>& other)
            : UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>(other)
        { }

        // inital state (zero rotation, zero angular velocity)
        RotationState(): UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>(){
        }

        // "down casting"
        RotationState(const UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>& other):
            UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>(other)
        {}

    public: // accessors
        Eigen::Block<Eigen::Matrix<double,dim,1> > rotational_velocity(){
            return UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>::accessElements(3,0,3,1);
        }

        const Eigen::Block<const Eigen::Matrix<double,dim,1> > rotational_velocity() const{
            return UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>::accessElements(3,0,3,1);
        }

    public:
        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(const Eigen::Vector3d& /*u*/,const double dt) {
            // continue rotation assuming constant velocity
            // rotational_velocity to quaternion
            Eigen::Vector3d rot_vel;
            rot_vel << this->rotational_velocity() * dt;

            Eigen::Matrix3d rot_vel_mat;
            rot_vel_mat << 1         , -rot_vel(2),  rot_vel(1),
                           rot_vel(2),           1, -rot_vel(0),
                          -rot_vel(1),  rot_vel(0),           1;
            Eigen::Quaterniond rotation_increment(rot_vel_mat);

            // TODO: compare with rotation_increment*rotation which sounds more reasonable
            Eigen::Quaterniond new_rotation = this->getRotationAsQuaternion() * rotation_increment; // follows paper
            Eigen::AngleAxisd  new_angle_axis(new_rotation); 
            this->rotation() = new_angle_axis.angle() * new_angle_axis.axis();
        }

        // state to measurement transformation function
        // HACK: add return type as parameter to enable overloading...
        M1 asMeasurement(const M1& /*z*/) const {
            M1 return_val;
            return_val << this->getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,1)), this->rotational_velocity();
            return return_val;
        }

        static const int size = dim;
};

// state for acceleration in "global" reference frame
template <class M1, int dim, int dim_cov = dim>
class State : public Eigen::Matrix<double,dim,1>{
    public: // constructors
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        State(const Eigen::MatrixBase<OtherDerived>& other)
            : Eigen::Matrix<double,dim,1>(other)
        { }

        // inital state (zero rotation, zero angular velocity)
        State(): Eigen::Matrix<double,dim,1>(Eigen::Matrix<double,dim,1>::Zero()){
        }

        // This method allows you to assign Eigen expressions to MyVectorType
        template<typename OtherDerived>
        State& operator=(const Eigen::MatrixBase <OtherDerived>& other)
        {
            this->Eigen::Matrix<double,dim,1>::operator=(other);
            return *this;
        }

    public: // accessors
        Eigen::Block<Eigen::Matrix<double,dim,1> > acceleration(){
            return Eigen::Block<Eigen::Matrix<double,dim,1> >(this->derived(), 0, 0, 3, 1);
        }

        const Eigen::Block<const Eigen::Matrix<double,dim,1> > acceleration() const {
            return Eigen::Block<const Eigen::Matrix<double,dim,1> >(this->derived(), 0, 0, 3, 1);
        }

    public:
        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(const Eigen::Vector3d& /*u*/, const double /*dt*/) {
            // assume constant acceleration
        }

        // state to measurement transformation function
        // HACK: add return type as parameter to enable overloading...
        M1 asMeasurement(const M1& /*z*/) const {
            return acceleration();
        }

        static State calcMean(std::vector<State, Eigen::aligned_allocator<State> >& states){
            State mean;

            // calculate new state (weighted mean of sigma points)
            for(typename std::vector<State, Eigen::aligned_allocator<State> >::iterator i = states.begin(); i != states.end(); ++i){
                mean += 1.0 / static_cast<double>(states.size()) * (*i);
            }

            return mean;
        }

        static const int size = dim;
};

#endif // IMUROTATIONSTATE_H
