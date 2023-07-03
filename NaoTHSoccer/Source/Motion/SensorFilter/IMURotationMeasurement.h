/**
* @file IMUModel.h
* 
* Declaration of class IMUModel
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/ 


#ifndef _IMUROTATIONMEASUREMENT_H
#define _IMUROTATIONMEASUREMENT_H

#include <Eigen/Core>

// measurement vectors which are elements of a euclidean vector space, e.g. velocity, accelerations
template <int dim>
class Measurement : public Eigen::Matrix<double,dim,1>
{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    Measurement(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<double,dim,1>(other) {}

    // initial measurement vector (zero rotation, zero angular velocity)
    Measurement() : Eigen::Matrix<double,dim,1>(Eigen::Matrix<double,dim,1>::Zero()) {}

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    Measurement& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
        this->Eigen::Matrix<double,dim,1>::operator=(other);
        return *this;
    }

public:
    static Measurement calcMean(const std::vector<Measurement, Eigen::aligned_allocator<Measurement> >& states)
    {
        Measurement mean;

        if(states.empty()) {
          return mean;
        }

        // calculate new state (weighted mean of sigma points)
        for(const auto& s : states) {
            mean += s;
        }

        return (1.0 / static_cast<double>(states.size())) * mean;
    }

    static const int size = dim;
};

// TODO: copying the class multiple times and giving it different names can not be the solution... or?
class GyroMeasurement : public Eigen::Vector3d
{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    GyroMeasurement(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Vector3d(other) {}

    // initial measurement vector (zero rotation, zero angular velocity)
    GyroMeasurement() : Eigen::Vector3d(Eigen::Vector3d::Zero()) {}

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    GyroMeasurement& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
        this->Eigen::Vector3d::operator=(other);
        return *this;
    }

public:
    static GyroMeasurement calcMean(const std::vector<GyroMeasurement, Eigen::aligned_allocator<GyroMeasurement> >& states)
    {
        GyroMeasurement mean;

        if(states.empty()) {
          return mean;
        }

        // calculate new state (weighted mean of sigma points)
        for(const auto& s : states) {
            mean += s;
        }

        return (1.0 / static_cast<double>(states.size())) * mean;
    }

    static const int size = 3;
};

class AccMeasurement : public Eigen::Vector3d
{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    AccMeasurement(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Vector3d(other) {}

    // initial measurement vector (zero rotation, zero angular velocity)
    AccMeasurement() : Eigen::Vector3d(Eigen::Vector3d::Zero()) {}

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    AccMeasurement& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
        this->Eigen::Vector3d::operator=(other);
        return *this;
    }

public:
    static AccMeasurement calcMean(const std::vector<AccMeasurement, Eigen::aligned_allocator<AccMeasurement> >& states)
    {
        AccMeasurement mean;

        if(states.empty()) {
          return mean;
        }

        // calculate new state (weighted mean of sigma points)
        for(const auto& s : states) {
            mean += s;
        }

        return (1.0 / static_cast<double>(states.size())) * mean;
    }

    static const int size = 3;
};
#endif // _IMUROTATIONMEASUREMENT_H
