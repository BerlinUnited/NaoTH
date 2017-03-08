#ifndef IMUROTATIONSTATE_H
#define IMUROTATIONSTATE_H

#include <Tools/naoth_eigen.h>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

// state in global reference frame
template <int dim_state, int dim_state_cov, class M>
class State : public Eigen::Matrix<double,dim_state,1> {
    public:
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        State(const Eigen::MatrixBase<OtherDerived>& other)
            : Eigen::Matrix<double,dim_state,1>(other)
        { }

        // This method allows you to assign Eigen expressions to MyVectorType
        template<typename OtherDerived>
        State& operator=(const Eigen::MatrixBase <OtherDerived>& other)
        {
            this->Eigen::Matrix<double,dim_state,1>::operator=(other);
            return *this;
        }

        // inital state (zero rotation, zero angular velocity)
        State(): Eigen::Matrix<double,dim_state,1>(Eigen::Matrix<double,dim_state,1>::Zero()){
        }

        // create state from vector
        State(Eigen::Matrix<double,dim_state,1> x): Eigen::Matrix<double,dim_state,1>(x){
        }

        Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotation(){
            return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 0, 0, 3, 1);
        }

        Eigen::Quaterniond getRotationAsQuaternion() /*const*/ {
            Eigen::Vector3d   rot(rotation());
            Eigen::AngleAxisd rot2;
            if(rot.norm() > 0){
                rot2 = Eigen::AngleAxisd(rot.norm(), rot.normalized());
            } else {
                rot2 = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
            }
            return Eigen::Quaterniond(rot2);
        }

        Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotational_velocity(){
            return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 3, 0, 3, 1);
        }

        Eigen::Block<Eigen::Matrix<double,dim_state,1> > bias_rotational_velocity(){
            return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 6, 0, 3, 1);
        }

        // unary operator
        State operator-() const{
            State temp = *this;
            Eigen::Quaterniond temp_rotation = temp.getRotationAsQuaternion();

            // invert directions
            temp = Eigen::Matrix<double, dim_state,1>::operator-();

            // replace with correct inverse rotation
            Eigen::AngleAxisd rot(temp_rotation.inverse());
            temp.rotation() = rot.angle()*rot.axis();

            return temp;
        }

        // binary operators
        State operator-(const State& other) const{
            State temp(*this);
            temp -= other;
            return temp;
        }

        State& operator -=(const State& other){
            *this += -other;
            return *this;
        }

        State operator+(const State& other) const{
            State temp(*this);
            temp += other;
            return temp;
        }

        // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "other" and then by "this"
        // eigen vectorization and alignment problem: other should be a reference
        State& operator +=(State other){
            Eigen::Quaterniond temp_rotation = this->getRotationAsQuaternion() * other.getRotationAsQuaternion();
            Eigen::Matrix<double,dim_state,1>::operator+=(other);

            // replace with correct rotation
            Eigen::AngleAxisd rot(temp_rotation);
            this->rotation() = rot.angle() * rot.axis();

            return *this;
        }

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
            Eigen::Quaterniond new_rotation = getRotationAsQuaternion() * rotation_increment; // follows paper
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
            q.setFromTwoVectors(getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,-1)),Eigen::Vector3d(0,0,-1));
            Eigen::AngleAxisd temp(q);
            Eigen::Vector3d   rotation_in_measurement_space(temp.angle() * temp.axis());

            M return_val;
            return_val << rotation_in_measurement_space, rotational_velocity_in_measurement_space;

            return return_val;
        }
};

#endif // IMUROTATIONSTATE_H
