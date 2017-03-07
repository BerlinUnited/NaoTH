#ifndef IMUROTATIONMEASUREMENT_H
#define IMUROTATIONMEASUREMENT_H

#include <Tools/naoth_eigen.h>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

template <int dim_measurement, int dim_measurement_cov>
class Measurement : public Eigen::Matrix<double, dim_measurement,1> {
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    Measurement(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Matrix<double,dim_measurement,1>(other)
    { }

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    Measurement& operator=(const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Eigen::Matrix<double,dim_measurement,1>::operator=(other);
        return *this;
    }

    // initial measurement vector (zero rotation, zero angular velocity)
    Measurement(): Eigen::Matrix<double,dim_measurement,1>(Eigen::Matrix<double,dim_measurement,1>::Zero()){
    }

    // create state from vector
    Measurement(Eigen::Matrix<double,dim_measurement,1> z): Eigen::Matrix<double,dim_measurement,1>(z){
    }

    Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotation(){
        return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 0, 0, 3, 1);
    }

    Eigen::Quaterniond getRotationAsQuaternion() /*const*/ {
        Eigen::Vector3d   rot;
        rot << rotation();
        Eigen::AngleAxisd rot2;
        if(rot.norm() > 0){
            rot2 = Eigen::AngleAxisd(rot.norm(), rot.normalized());
        } else {
            rot2 = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
        }
        return Eigen::Quaterniond(rot2);
    }

    Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotational_velocity(){
        return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 3, 0, 3, 1);
    }

    // unary operator
    Measurement operator-() const{
        Measurement temp = *this;
        Eigen::Quaterniond temp_rotation = temp.getRotationAsQuaternion();

        // invert directions
        temp = Eigen::Matrix<double, dim_measurement,1>::operator-();

        // replace with correct inverse rotation
        Eigen::AngleAxisd rot(temp_rotation.inverse());
        temp.rotation() = rot.angle()*rot.axis();

        return temp;
    }

    // binary operators
    Measurement operator-(const Measurement& other) const{
        Measurement temp(*this);
        temp -= other;
        return temp;
    }

    Measurement& operator -=(const Measurement& other){
        *this += -other;
        return *this;
    }

    Measurement operator+(const Measurement& other) const{
        Measurement temp(*this);
        temp += other;
        return temp;
    }

    // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "this" and then by "other"
    // eigen vectorization and alignment problem: other should be a reference
    Measurement& operator +=(Measurement other){
        Eigen::Quaterniond temp_rotation = this->getRotationAsQuaternion() * other.getRotationAsQuaternion();
        Eigen::Matrix<double,dim_measurement,1>::operator+=(other);

        // replace with correct rotation
        Eigen::AngleAxisd rot(temp_rotation);
        this->rotation() = rot.angle() * rot.axis();

        return *this;
    }
};

#endif // IMUROTATIONMEASUREMENT_H
