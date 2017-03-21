#ifndef IMUROTATIONMEASUREMENT_H
#define IMUROTATIONMEASUREMENT_H

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UKFStateRotationBase.h"

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

template <int dim, int dim_cov = dim, int rotation_index = 0>
class Measurement : public UKFStateRotationBase< Measurement<dim, dim_cov>, rotation_index, dim>{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    Measurement(const Eigen::MatrixBase<OtherDerived>& other)
        : UKFStateRotationBase<Measurement<dim, dim_cov>, rotation_index, dim>(other)
    { }

    // initial measurement vector (zero rotation, zero angular velocity)
    Measurement(): UKFStateRotationBase<Measurement<dim, dim_cov>, rotation_index, dim>(){
    }

    // "down casting"
    Measurement(const UKFStateRotationBase<Measurement<dim, dim_cov>, rotation_index, dim>& other):
        UKFStateRotationBase<Measurement<dim, dim_cov>, rotation_index, dim>(other)
    {}

public: // operators
    // This method allows you to assign Eigen expressions to MyVectorType
    // TODO: needed?
    template<typename OtherDerived>
    Measurement& operator=(const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->UKFStateRotationBase<Measurement<dim, dim_cov>, rotation_index, dim>::operator=(other);
        return *this;
    }

public: // additional accessors

    Eigen::Block<Eigen::Matrix<double,dim,1> > rotational_velocity(){
        return Eigen::Block<Eigen::Matrix<double,dim,1> >(this->derived(), 3, 0, 3, 1);
    }
};

#endif // IMUROTATIONMEASUREMENT_H
