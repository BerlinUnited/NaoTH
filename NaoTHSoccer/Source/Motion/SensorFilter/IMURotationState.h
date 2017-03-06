#ifndef IMUROTATIONSTATE_H
#define IMUROTATIONSTATE_H

#include <Tools/naoth_eigen.h>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

// state in global reference frame
template <int dim_state, int dim_state_cov>
class State : public Eigen::Matrix<double,dim_state,1> {
    public:
            State(): Eigen::Matrix<double,dim_state,1>(Eigen::Matrix<double,dim_state,1>::Zero()){
            }

            State(Eigen::Matrix<double,dim_state,1> x): Eigen::Matrix<double,dim_state,1>(x){
            }

            Eigen::Block<Eigen::Matrix<double,dim_state,1> > acceleration(){
                return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 0, 0, 3, 1);
            }

            Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotation(){
                return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 3, 0, 3, 1);
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
                return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 6, 0, 3, 1);
            }

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

            // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "this" and then by "other"
            // eigen vectorization and alignment problem: other should be a reference
            State& operator +=(State other){
                Eigen::Quaterniond temp_rotation = other.getRotationAsQuaternion() * this->getRotationAsQuaternion();
                Eigen::Matrix<double,dim_state,1>::operator+=(other);

                // replace with correct rotation
                Eigen::AngleAxisd rot(temp_rotation);
                this->rotation() = rot.angle() * rot.axis();

                return *this;
            }

            State& operator=(const Eigen::Matrix<double,dim_state,1> rhs) {
                this->Eigen::Matrix<double,dim_state,1>::operator=(rhs);
                return *this;
            }

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

            State scale(double s){
                State temp;
                temp << s * (*this);
                return temp;
            }
};

#endif // IMUROTATIONSTATE_H
