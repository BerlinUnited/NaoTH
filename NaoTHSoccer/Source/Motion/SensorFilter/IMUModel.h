#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"

#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include <Representations/Modeling/IMUData.h>

#include <Tools/naoth_eigen.h>

// TODO: remove pragma
#pragma GCC diagnostic ignored "-Wconversion"
    #include <Eigen/Geometry>
    #include <Eigen/Dense>
#pragma GCC diagnostic pop

#include <vector>

BEGIN_DECLARE_MODULE(IMUModel)

    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings3D)
    PROVIDE(DebugPlot)

    REQUIRE(FrameInfo)

    REQUIRE(GyrometerData)
    REQUIRE(AccelerometerData)

    PROVIDE(IMUData)
END_DECLARE_MODULE(IMUModel)

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

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov, class S = State<dim_state,dim_state_cov>, class M = Measurement<dim_measurement,dim_measurement_cov> >
class UKF {
    public:
// TODO: enable different updates like only acceleration and rotation or all three
//        enum class UpdateType{
//            acc_rot_rotvel,
//            acc_rot
//        };

    public:
        UKF():
            Q(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity()),
            P(Eigen::Matrix<double,dim_state_cov,dim_state_cov>::Identity())
        {
            // bias_acceleration (x,y,z) [m/s^2], too small?
//            P(9,9)   = 10e-10;
//            P(10,10) = 10e-10;
//            P(11,11) = 10e-10;

            // bias_rotational_velocity (x,y,z) [m/s^2], too small?
//            P(18,18) = 10e-10;
//            P(19,19) = 10e-10;
//            P(20,20) = 10e-10;

            // set covariance matrix of process noise
            // acceleration (x,y,z) [m/s^2]
            Q(0,0) = 0.01;
            Q(1,1) = 0.01;
            Q(2,2) = 0.01;

            // bias_acceleration (x,y,z) [m/s^2], too small?
//            Q(9,9)   = 10e-10;
//            Q(10,10) = 10e-10;
//            Q(11,11) = 10e-10;

            // rotation (x,y,z) [rad]?
            Q(3,3) = 0.001;
            Q(4,4) = 0.001;
            Q(5,5) = 0.001;

            // rotational_velocity (x,y,z) [rad/s], too small?
            Q(6,6) = 0.1;
            Q(7,7) = 0.1;
            Q(8,8) = 0.1;

            // bias_rotational_velocity (x,y,z) [m/s^2], too small?
//            Q(18,18) = 10e-10;
//            Q(19,19) = 10e-10;
//            Q(20,20) = 10e-10;

            // set covariance matrix of measurement noise
            // measured covariance of acceleration and rotational velocity (motion log, 60 seconds)
//            R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, 0     , 0     , 0     , -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,
//                -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, 0     , 0     , 0     , -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,
//                 1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04, 0     , 0     , 0     ,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,
//                 0                       ,  0                       ,  0                       , 10e-11, 0     , 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 10e-11, 0     ,  0                       ,  0                       ,  0                       ,
//                 0                       ,  0                       ,  0                       , 0     , 0     , 10e-11,  0                       ,  0                       ,  0                       ,
//                -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07, 0     , 0     , 0     ,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,
//                -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, 0     , 0     , 0     , -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,
//                -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07, 0     , 0     , 0     ,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;

            // synthetic measurement covariance matrix used for testing
            R << 10e-5, 0, 0, 0, 0, 0, 0, 0,
                 0, 10e-5, 0, 0, 0, 0, 0, 0,
                 0, 0, 10e-5, 0, 0, 0, 0, 0,
                 0, 0, 0, 10e-5, 0, 0, 0, 0,
                 0, 0, 0, 0, 10e-5, 0, 0, 0,
                 0, 0, 0, 0, 0, 10e-5, 0, 0,
                 0, 0, 0, 0, 0, 0, 10e-5, 0,
                 0, 0, 0, 0, 0, 0, 0, 10e-5;

        }

    public:

        void reset();
        void predict(double dt);
        void update(M z);

    public:

        //--- setter ---//
        //void setState(Eigen::Vector4d& state);
        //void setCovarianceOfState(const Eigen::Matrix4d& p1);
        //void setCovarianceOfProcessNoise(const Eigen::Matrix2d& q);
        //void setCovarianceOfMeasurementNoise(const Eigen::Matrix2d& r);

        //const Eigen::Matrix4d& getProcessCovariance() const { return P; }
        //const Eigen::Matrix2d& getMeasurementCovariance() const { return R; }
        //const Eigen::Vector4d& getState() const { return x; }
        //Eigen::Vector2d        getStateInMeasurementSpace(const Measurement_Function_H& h) const { return h(x(0),x(2)); } // horizontal, vertical
        //Eigen::Matrix2d        getStateCovarianceInMeasurementSpace(const Measurement_Function_H& h) const; // horizontal, vertical
        //const Ellipse2d&       getEllipseLocation() const { return ellipse_location; }
        //const Ellipse2d&       getEllipseVelocity() const { return ellipse_velocity; }

    private:
        //Eigen::Matrix<double,2,4> approximateH(const Measurement_Function_H& h) const;
        //void updateEllipses();

    private:
        const double alpha  = 10E-3;
        const double kapa   = 0;
        const double beta   = 2;
        const double lambda = alpha * alpha * (dim_state + kapa) - dim_state;

        std::vector<S> sigmaPoints;

    public:
        S state;

        void transitionFunction(S &state, double dt); // state transition function
        M stateToMeasurementSpaceFunction(S& state);  // state to measurement transformation function
        S measurementToStateSpaceFunction(M& measurement);
        void generateSigmaPoints();

        // covariances
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> Q;         // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> P;         // covariance matrix of current state

        // TODO: check whether this concept of averageing can be applied directly on the rotations (average axis and average angle)
        // TODO: adjust maximum of iterations
        Eigen::Vector3d averageRotation(std::vector<Eigen::Quaterniond> rotations, Eigen::Quaterniond mean){
            std::vector<Eigen::Vector3d> rotational_differences;

            for(int i = 0; i < 10; ++i){
                // calculate difference between the mean and the sigma points rotation by means of a rotation
                rotational_differences.clear();
                for (typename std::vector<Eigen::Quaterniond>::iterator i = rotations.begin(); i != rotations.end(); ++i){
                    Eigen::AngleAxis<double> rotational_difference = Eigen::AngleAxis<double>((*i) * mean.inverse());

                    // store as rotation vector representation
                    rotational_differences.push_back(rotational_difference.angle() * rotational_difference.axis());
                }

                // average difference quaternions in their 3d vectorial representation (length = angle, direction = axis)
                Eigen::Vector3d averaged_rotational_difference = Eigen::Vector3d::Zero();
                for (typename std::vector<Eigen::Vector3d >::iterator i = rotational_differences.begin(); i != rotational_differences.end(); ++i){
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
};

class IMUModel: private IMUModelBase
{
public:
    IMUModel();

    void execute();
    void writeIMUData();
    void plots();

private:
    FrameInfo lastFrameInfo;

    UKF<9,9,8,8> ukf;

    typedef Measurement<8,8> IMU_Measurement;

    Eigen::Vector3d quaternionToRotationVector(const Eigen::Quaterniond& q) const{
        Eigen::AngleAxisd temp(q);
        return temp.angle() * temp.axis();
    }
};

#endif // IMUMODEL_H
