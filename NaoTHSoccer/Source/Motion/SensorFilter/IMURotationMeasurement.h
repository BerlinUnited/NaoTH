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
    Measurement(): Eigen::Matrix<double,dim_measurement,1>(Eigen::Matrix<double,dim_measurement,1>::Zero()){
    }

    Measurement(Eigen::Matrix<double,dim_measurement,1> z): Eigen::Matrix<double,dim_measurement,1>(z){
    }

    Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > acceleration(){
        return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 0, 0, 3, 1);
    }

    Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotation(){
        return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 3, 0, 2, 1);
    }

    Eigen::Quaterniond getRotationAsQuaternion() /*const*/ {
        Eigen::Vector3d   rot;
        rot << rotation(), 0;
        Eigen::AngleAxisd rot2;
        if(rot.norm() > 0){
            rot2 = Eigen::AngleAxisd(rot.norm(), rot.normalized());
        } else {
            rot2 = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
        }
        return Eigen::Quaterniond(rot2);
    }

    Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotational_velocity(){
        return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 5, 0, 3, 1);
    }
};

#endif // IMUROTATIONMEASUREMENT_H
