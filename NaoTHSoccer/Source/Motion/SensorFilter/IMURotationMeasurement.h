#ifndef IMUROTATIONMEASUREMENT_H
#define IMUROTATIONMEASUREMENT_H

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

public:
    static Measurement calcMean(std::vector<Measurement, Eigen::aligned_allocator<Measurement> >& states){
        Measurement mean;

        // calculate new state (weighted mean of sigma points)
        for(typename std::vector<Measurement, Eigen::aligned_allocator<Measurement> >::iterator i = states.begin(); i != states.end(); ++i){
            mean += 1.0 / static_cast<double>(states.size()) * (*i);
        }

        return mean;
    }

    static const int size = dim;
};

#endif // IMUROTATIONMEASUREMENT_H
