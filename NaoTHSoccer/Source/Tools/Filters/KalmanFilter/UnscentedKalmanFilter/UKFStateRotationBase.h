#ifndef UKFSTATEBASE_H
#define UKFSTATEBASE_H

#include <Tools/naoth_eigen.h>

template <class Derived, int rotation_index, int dim_state>
class UKFStateRotationBase : public Eigen::Matrix<double,dim_state,1>
{
public:
    // This constructor allows you to construct MyVectorType from Eigen expressions
    template<typename OtherDerived>
    UKFStateRotationBase(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Matrix<double,dim_state,1>(other)
    { }

    // This method allows you to assign Eigen expressions to MyVectorType
    template<typename OtherDerived>
    UKFStateRotationBase& operator=(const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Eigen::Matrix<double,dim_state,1>::operator=(other);
        return *this;
    }

    // inital state (zero rotation, zero angular velocity)
    UKFStateRotationBase(): Eigen::Matrix<double,dim_state,1>(Eigen::Matrix<double,dim_state,1>::Zero()){
    }

public:
    Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotation(){
        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), rotation_index, 0, 3, 1);
    }

    const Eigen::Block<const Eigen::Matrix<double,dim_state,1> > rotation() const{
        return Eigen::Block<const Eigen::Matrix<double,dim_state,1> >(this->derived(), rotation_index, 0, 3, 1);
    }

    Eigen::Block<Eigen::Matrix<double,dim_state,1> > accessElements(int i, int j, int rows, int columns){
        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), i, j, rows, columns);
    }

    const Eigen::Block<const Eigen::Matrix<double,dim_state,1> > accessElements(int i, int j, int rows, int columns) const{
        return Eigen::Block<const Eigen::Matrix<double,dim_state,1> >(this->derived(), i, j, rows, columns);
    }

    Eigen::Quaterniond getRotationAsQuaternion() const {
        Eigen::Vector3d   rot(rotation());
        Eigen::AngleAxisd rot2;
        if(rot.norm() > 0){
            rot2 = Eigen::AngleAxisd(rot.norm(), rot.normalized());
        } else {
            rot2 = Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
        }
        return Eigen::Quaterniond(rot2);
    }

    void setRotationFromRotationVector(Eigen::Vector3d rotVector){
        this->rotation() = rotVector;
    }

public: // operators
    // unary operator
    UKFStateRotationBase operator-() const{
        UKFStateRotationBase temp = *this;
        Eigen::Quaterniond temp_rotation = temp.getRotationAsQuaternion();

        // invert directions
        temp = Eigen::Matrix<double,dim_state,1>::operator-();

        // replace with correct inverse rotation
        Eigen::AngleAxisd rot(temp_rotation.inverse());
        temp.setRotationFromRotationVector(rot.angle()*rot.axis());

        return temp;
    }

    // binary operators
    UKFStateRotationBase operator-(const UKFStateRotationBase& other) const{
        UKFStateRotationBase temp(*this);
        temp -= other;
        return temp;
    }

    UKFStateRotationBase& operator -=(const UKFStateRotationBase& other){
        *this += -other;
        return *this;
    }

    UKFStateRotationBase operator+(const UKFStateRotationBase& other) const{
        UKFStateRotationBase temp(*this);
        temp += other;
        return temp;
    }

    // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "other" and then by "this"
    UKFStateRotationBase& operator +=(const UKFStateRotationBase& other){
        Eigen::Quaterniond temp_rotation = this->getRotationAsQuaternion() * other.getRotationAsQuaternion();
        Eigen::Matrix<double,dim_state,1>::operator+=(other);

        // replace with correct rotation
        Eigen::AngleAxisd rot(temp_rotation);
        setRotationFromRotationVector(rot.angle() * rot.axis());

        return *this;
    }

protected:
    // TODO: check whether this concept of averageing can be applied directly on the rotations (average axis and average angle)
    // TODO: adjust maximum of iterations
    static Eigen::Vector3d averageRotation(std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >& rotations, const Eigen::Quaterniond& m) {
        Eigen::Quaterniond mean(m);

//        Eigen::MatrixXd rotational_differences;
//        rotational_differences.resize(3,rotations.size());

//        for(int i = 0; i < 10; ++i){
//            // calculate difference between the mean and the sigma points rotation by means of a rotation
//            for (size_t j = 0; j < rotations.size(); j++){
//                Eigen::AngleAxis<double> rotational_difference = Eigen::AngleAxis<double>(rotations[j] * mean.inverse());
//                // store as rotation vector representation
//                rotational_differences.col(j) = rotational_difference.angle() * rotational_difference.axis();
//            }

//            // average difference quaternions in their 3d vectorial representation (length = angle, direction = axis)
//            Eigen::Vector3d averaged_rotational_difference = 1.0 / static_cast<double>(rotations.size()) * rotational_differences.rowwise().sum();

//            mean = Eigen::Quaterniond(Eigen::AngleAxis<double>(averaged_rotational_difference.norm(), averaged_rotational_difference.normalized())) * mean;
//            if(averaged_rotational_difference.norm() < 10e-4){
//                break;
//            }
//        }

        std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > rotational_differences;

        for(int i = 0; i < 10; ++i){
            // calculate difference between the mean and the sigma points rotation by means of a rotation
            rotational_differences.clear();
            for (typename std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >::iterator i = rotations.begin(); i != rotations.end(); ++i){
                Eigen::AngleAxis<double> rotational_difference = Eigen::AngleAxis<double>((*i) * mean.inverse());

                // store as rotation vector representation
                rotational_differences.push_back(rotational_difference.angle() * rotational_difference.axis());
            }

            // average difference quaternions in their 3d vectorial representation (length = angle, direction = axis)
            Eigen::Vector3d averaged_rotational_difference = Eigen::Vector3d::Zero();
            for (typename std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >::iterator i = rotational_differences.begin(); i != rotational_differences.end(); ++i){
                averaged_rotational_difference += *i;
            }
            averaged_rotational_difference = 1.0 / static_cast<double>(rotational_differences.size()) * averaged_rotational_difference;

            mean = Eigen::Quaterniond(Eigen::AngleAxis<double>(averaged_rotational_difference.norm(), averaged_rotational_difference.normalized())) * mean;
            if(averaged_rotational_difference.norm() < 10e-4){
                break;
            }
        }

        Eigen::Vector3d mean_rotation_vector;
        mean_rotation_vector << Eigen::AngleAxisd(mean).angle() * Eigen::AngleAxisd(mean).axis();

        return mean_rotation_vector;
    }

public:
    static Derived calcMean(std::vector<Derived, Eigen::aligned_allocator<Derived> >& states){
        Derived mean;
        std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> > rotations;

        // calculate new state (weighted mean of sigma points)
        for(typename std::vector<Derived, Eigen::aligned_allocator<Derived> >::iterator i = states.begin(); i != states.end(); ++i){
            rotations.push_back((*i).getRotationAsQuaternion());
            mean += 1.0 / static_cast<double>(states.size()) * (*i);
        }

        // more correct determination of the mean rotation
        mean.rotation() = averageRotation(rotations, rotations.back());

        return mean;
    }
};

#endif // UKFSTATEBASE_H
