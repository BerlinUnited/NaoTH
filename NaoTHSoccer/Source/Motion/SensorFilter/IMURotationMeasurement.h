#ifndef IMUROTATIONMEASUREMENT_H
#define IMUROTATIONMEASUREMENT_H

#include "Tools/Filters/KalmanFilter/UnscentedKalmanFilter/UKFStateRotationBase.h"

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

// measurement vector which have components which didn't belong to a vector space, e.g. here a rotation vector
template <int dim, int dim_cov = dim, int rotation_index = 0>
class RotationMeasurement : public UKFStateRotationBase< RotationMeasurement<dim, dim_cov>, rotation_index, dim>{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    RotationMeasurement(const Eigen::MatrixBase<OtherDerived>& other)
        : UKFStateRotationBase<RotationMeasurement<dim, dim_cov>, rotation_index, dim>(other)
    { }

    // initial measurement vector (zero rotation, zero angular velocity)
    RotationMeasurement(): UKFStateRotationBase<RotationMeasurement<dim, dim_cov>, rotation_index, dim>(){
    }

    // "down casting"
    RotationMeasurement(const UKFStateRotationBase<RotationMeasurement<dim, dim_cov>, rotation_index, dim>& other):
        UKFStateRotationBase<RotationMeasurement<dim, dim_cov>, rotation_index, dim>(other)
    {}

public: // additional accessors
//    Eigen::Block<Eigen::Matrix<double,dim,1> > rotational_velocity(){
//        return Eigen::Block<Eigen::Matrix<double,dim,1> >(this->derived(), 3, 0, 3, 1);
//    }
};

// measurement vectors which are elements of a euclidean vector space, e.g. velocity, accelerations
template <int dim>
class Measurement : public Eigen::Matrix<double,dim,1>{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    Measurement(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Matrix<double,dim,1>(other)
    { }

    // inital state (zero rotation, zero angular velocity)
    Measurement(): Eigen::Matrix<double,dim,1>(Eigen::Matrix<double,dim,1>::Zero()){
    }

    // initial measurement vector (zero rotation, zero angular velocity)
    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    Measurement& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
        this->Eigen::Matrix<double,dim,1>::operator=(other);
        return *this;
    }

/*public: // additional accessors
    Eigen::Block<Eigen::Matrix<double,dim,1> > velocity(){
        return Eigen::Block<Eigen::Matrix<double,dim,1> >(this->derived(), 0, 0, 3, 1);
    }*/

public:
    static Measurement calcMean(std::vector<Measurement, Eigen::aligned_allocator<Measurement> >& states){
        Measurement mean;

        // calculate new state (weighted mean of sigma points)
        for(typename std::vector<Measurement, Eigen::aligned_allocator<Measurement> >::iterator i = states.begin(); i != states.end(); ++i){
            mean += 1.0 / static_cast<double>(states.size()) * (*i);
        }

        return mean;
    }
};

#endif // IMUROTATIONMEASUREMENT_H
