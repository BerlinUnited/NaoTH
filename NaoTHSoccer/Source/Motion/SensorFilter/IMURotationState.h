/**
* @file IMURotationState.h
* 
* Declaration of class IMURotationState
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/ 

#ifndef _IMUROTATIONSTATE_H
#define _IMUROTATIONSTATE_H

#include <Eigen/Core>
#include <Eigen/Geometry>

// state for rotation and rotational velocity
template <class M1, class M2, class M3, int dim, int dim_cov = dim, int rotation_index = 0>
class RotationState : public Eigen::Matrix<double,dim,1> //public RotationState<RotationState<M1,/* M2,*/ dim, dim_cov>, rotation_index, dim>
{
    public: // constructors
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        RotationState(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<double,dim,1>(other) {}

        // This method allows you to assign Eigen expressions to MyVectorType
        template<typename OtherDerived>
        RotationState& operator=(const Eigen::MatrixBase <OtherDerived>& other)
        {
            this->Eigen::Matrix<double,dim,1>::operator=(other);
            return *this;
        }

        // inital state (zero rotation, zero angular velocity)
        RotationState(): Eigen::Matrix<double,dim,1>(Eigen::Matrix<double,dim,1>::Zero()){}

    public: // accessors
        Eigen::Block<Eigen::Matrix<double,dim,1>, 3, 1> rotation(){
            return Eigen::Block<Eigen::Matrix<double,dim,1>, 3, 1>(this->derived(), 0, 0);
        }

        const Eigen::Block<const Eigen::Matrix<double,dim,1>, 3, 1> rotation() const{
            return Eigen::Block<const Eigen::Matrix<double,dim,1>,3 , 1>(this->derived(), 0, 0);
        }

        Eigen::Block<Eigen::Matrix<double,dim,1>, 3, 1> rotational_velocity(){
            return Eigen::Block<Eigen::Matrix<double,dim,1>, 3, 1>(this->derived(), 3, 0);
        }

        const Eigen::Block<const Eigen::Matrix<double,dim,1>, 3, 1> rotational_velocity() const{
            return Eigen::Block<const Eigen::Matrix<double,dim,1>, 3, 1>(this->derived(), 3, 0);
        }

    public: // some helper functions
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

        Eigen::AngleAxisd getRotationAsAngleAxisd() const {
            double norm = rotation().norm();
            if(norm > 0){
                return Eigen::AngleAxisd(norm, rotation().normalized());
            }
                return Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
        }

    public: // define arithmetics
        // unary operator
        RotationState operator-() const{
            return this->Eigen::Matrix<double, dim,1>::operator-();
        }

        // binary operators
        RotationState operator-(const RotationState& other) const{
            RotationState temp(*this);
            temp -= other;
            return temp;
        }

        RotationState& operator -=(const RotationState& other){
            *this += -other;
            return *this;
        }

        RotationState operator+(const RotationState& other) const{
            RotationState temp(*this);
            temp += other;
            return temp;
        }

        // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "other" and then by "this"
        RotationState& operator +=(const RotationState& other){
            Eigen::Quaterniond temp_rotation = this->getRotationAsAngleAxisd() * other.getRotationAsAngleAxisd();
            Eigen::Matrix<double,dim,1>::operator+=(other);

            // replace with correct rotation
            Eigen::AngleAxisd rot(temp_rotation);
            rotation() = rot.angle() * rot.axis();

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

            // does not make sense
            for(int i = 0; i < 10; ++i) {
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
        static RotationState calcMean(std::vector<RotationState, Eigen::aligned_allocator<RotationState> >& states) {
            RotationState mean;
            std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> > rotations;

            // calculate new state (weighted mean of sigma points)
            for(typename std::vector<RotationState, Eigen::aligned_allocator<RotationState> >::iterator i = states.begin(); i != states.end(); ++i) {
                rotations.push_back((*i).getRotationAsQuaternion());
                mean += 1.0 / static_cast<double>(states.size()) * (*i);
            }

            // more correct determination of the mean rotation
            mean.rotation() = averageRotation(rotations, rotations.back());

            return mean;
        }

    public:
        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(const Eigen::Vector3d& /*u*/,const double dt) {
            // continue rotation assuming constant velocity
            // rotational_velocity to quaternion
            Eigen::Vector3d rot_vel;
            rot_vel << this->rotational_velocity() * dt;

            Eigen::Matrix3d rot_vel_mat;
            rot_vel_mat << 1         , -rot_vel(2),  rot_vel(1),
                           rot_vel(2),           1, -rot_vel(0),
                          -rot_vel(1),  rot_vel(0),           1;
            Eigen::Quaterniond rotation_increment(rot_vel_mat);

            Eigen::Quaterniond new_rotation = this->getRotationAsQuaternion() * rotation_increment;
            Eigen::AngleAxisd  new_angle_axis(new_rotation); 
            this->rotation() = new_angle_axis.angle() * new_angle_axis.axis();
        }

        // state to measurement transformation function
        // HACK: add return type as parameter to enable overloading...
        M1 asMeasurement(const M1& /*z*/) const {
            return this->getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,1));
        }

        M2 asMeasurement(const M2& /*z*/) const {
            return this->rotational_velocity();
        }

        M3 asMeasurement(const M3& /*z*/) const {
            M3 return_val;
            return_val << this->getRotationAsQuaternion().inverse()._transformVector(Eigen::Vector3d(0,0,1)), this->rotational_velocity();
            return return_val;
        }

        static const size_t size = dim;
};

// state for acceleration in "global" reference frame
template <class M1, int dim, int dim_cov = dim>
class State : public Eigen::Matrix<double,dim,1>
{
    public: // constructors
        // This constructor allows you to construct MyVectorType from Eigen expressions
        template<typename OtherDerived>
        State(const Eigen::MatrixBase<OtherDerived>& other)
            : Eigen::Matrix<double,dim,1>(other)
        { }

        // inital state (zero rotation, zero angular velocity)
        State(): Eigen::Matrix<double,dim,1>(Eigen::Matrix<double,dim,1>::Zero()) {
        }

        // This method allows you to assign Eigen expressions to MyVectorType
        template<typename OtherDerived>
        State& operator=(const Eigen::MatrixBase <OtherDerived>& other)
        {
            this->Eigen::Matrix<double,dim,1>::operator=(other);
            return *this;
        }

    public: // accessors
        Eigen::Block<Eigen::Matrix<double,dim,1> > acceleration() {
            return Eigen::Block<Eigen::Matrix<double,dim,1> >(this->derived(), 0, 0, 3, 1);
        }

        const Eigen::Block<const Eigen::Matrix<double,dim,1> > acceleration() const {
            return Eigen::Block<const Eigen::Matrix<double,dim,1> >(this->derived(), 0, 0, 3, 1);
        }

    public:
        // functions requiered by the unscented kalman filter
        // TODO: should these functions be part of the filter?

        // state transition function
        void predict(const Eigen::Vector3d& /*u*/, const double /*dt*/) {
            // assume constant acceleration
        }

        // state to measurement transformation function
        // HACK: add return type as parameter to enable overloading...
        M1 asMeasurement(const M1& /*z*/) const {
            return acceleration();
        }

        static State calcMean(std::vector<State, Eigen::aligned_allocator<State> >& states) {
            State mean;

            // calculate new state (weighted mean of sigma points)
            for(typename std::vector<State, Eigen::aligned_allocator<State> >::iterator i = states.begin(); i != states.end(); ++i) {
                mean += 1.0 / static_cast<double>(states.size()) * (*i);
            }

            return mean;
        }

        static const int size = dim;
};

#endif // _IMUROTATIONSTATE_H
