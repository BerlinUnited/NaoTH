#ifndef IMUROTATIONSTATE_H
#define IMUROTATIONSTATE_H

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UKFStateRotationBase.h"

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

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
//        Eigen::Block<Eigen::Matrix<double,dim,1> > rotational_velocity(){
//            return UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>::accessElements(3,0,3,1);
//        }

//        const Eigen::Block<const Eigen::Matrix<double,dim,1> > rotational_velocity() const{
//            return UKFStateRotationBase<RotationState<M1, /*M2,*/ dim, dim_cov>, rotation_index, dim>::accessElements(3,0,3,1);
//        }

    public:
        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(Eigen::Vector3d& u, double dt) {
            // rotational_velocity to quaternion
//            Eigen::Vector3d rotational_velocity = this->rotational_velocity();
            Eigen::Quaterniond rotation_increment;
//            Eigen::Quaterniond rotation_increment2;
//            if(rotational_velocity.norm() > 0) {
//                rotation_increment2 = Eigen::Quaterniond(Eigen::AngleAxisd(rotational_velocity.norm()*dt, rotational_velocity.normalized()));

//                Eigen::Matrix3d rot_vel_mat;
//                rotational_velocity = rotational_velocity * dt;
//                rot_vel_mat << 1                     , -rotational_velocity(2),  rotational_velocity(1),
//                               rotational_velocity(2),                       1, -rotational_velocity(0),
//                              -rotational_velocity(1),  rotational_velocity(0),                       1;
//                rotation_increment = Eigen::Quaterniond(rot_vel_mat);
//            } else {
//                // zero rotation quaternion
//                rotation_increment = Eigen::Quaterniond(1,0,0,0);
//            }

            Eigen::Vector3d rotational_velocity = u * dt;
            Eigen::Matrix3d rot_vel_mat;
            rot_vel_mat << 1                     , -rotational_velocity(2),  rotational_velocity(1),
                           rotational_velocity(2),                       1, -rotational_velocity(0),
                          -rotational_velocity(1),  rotational_velocity(0),                       1;
            rotation_increment = Eigen::Quaterniond(rot_vel_mat);

            // continue rotation assuming constant velocity
            // TODO: compare with rotation_increment*rotation which sounds more reasonable
            Eigen::Quaterniond new_rotation = this->getRotationAsQuaternion() * rotation_increment; // follows paper
            //Eigen::Quaterniond new_rotation = rotation_increment * this->getRotationAsQuaternion();
            Eigen::AngleAxisd  new_angle_axis(new_rotation);

            this->rotation() = new_angle_axis.angle() * new_angle_axis.axis();

//            this->rotational_velocity() = this->rotational_velocity();
        }

        // state to measurement transformation function
        // HACK: add return type as parameter to enable overloading...
        M1 asMeasurement(const M1& z) const {
            // state to measurement function
            // transform acceleration part of the state into local measurement space (the robot's body frame = frame of accelerometer), the bias is already in this frame
            //Eigen::Vector3d rotational_velocity_in_measurement_space = rotational_velocity();// + bias_rotational_velocity();

            // determine rotation around x and y axis
            Eigen::Quaterniond q;
            q.setFromTwoVectors(this->getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,-1)),Eigen::Vector3d(0,0,-1));
            Eigen::AngleAxisd temp(q);
            Eigen::Vector3d   rotation_in_measurement_space(temp.angle() * temp.axis());

            M1 return_val(z); // HACK: prevent "unused parameter" warning
            return_val << rotation_in_measurement_space/*, rotational_velocity_in_measurement_space*/;

            return return_val;
        }

        // HACK: add return type as parameter to enable overloading...
//        M2 asMeasurement(const M2& z) const {
//            M2 return_val(z);
//            return_val << rotational_velocity();
//            return return_val;
//        }
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
        void predict(double /*u*/, double /*dt*/) {
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
//        static int size() {
//            return dim;
//        }
};

#endif // IMUROTATIONSTATE_H
