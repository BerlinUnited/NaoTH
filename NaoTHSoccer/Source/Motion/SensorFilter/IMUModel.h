#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"

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
    PROVIDE(DebugPlot)

    REQUIRE(FrameInfo)

    REQUIRE(GyrometerData)
    REQUIRE(AccelerometerData)

    PROVIDE(IMUData)
END_DECLARE_MODULE(IMUModel)

template <int dim_state, int dim_state_cov, int dim_measurement, int dim_measurement_cov>
class UKF {
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
            // location (x,y,z) [m]
            Q(0,0) = 0.0001;
            Q(1,1) = 0.0001;
            Q(2,2) = 0.0001;

            // velocity (x,y,z) [m/s]
            Q(3,3) = 0.001;
            Q(4,4) = 0.001;
            Q(5,5) = 0.001;

            // acceleration (x,y,z) [m/s^2]
            Q(6,6) = 0.01;
            Q(7,7) = 0.01;
            Q(8,8) = 0.01;

            // bias_acceleration (x,y,z) [m/s^2], too small?
//            Q(9,9)   = 10e-10;
//            Q(10,10) = 10e-10;
//            Q(11,11) = 10e-10;

            // rotation (x,y,z) [rad]?
            Q(9,9) = 0.001;
            Q(10,10) = 0.001;
            Q(11,11) = 0.001;

            // rotational_velocity (x,y,z) [rad/s], too small?
            Q(12,12) = 0.1;
            Q(13,13) = 0.1;
            Q(14,14) = 0.1;

            // bias_rotational_velocity (x,y,z) [m/s^2], too small?
//            Q(18,18) = 10e-10;
//            Q(19,19) = 10e-10;
//            Q(20,20) = 10e-10;

            // set covariance matrix of measurement noise
            // measured covariance of acceleration and rotational velocity (motion log, 60 seconds)
            R << 5.074939351879890342e-04, -1.561730283237946278e-05,  1.012849085655689321e-04, -3.078687958578659292e-08, -1.132513004663809251e-06, -6.485352375515866273e-07,// 0   , 0   , 0   ,
                -1.561730283237946278e-05,  2.570436087068024501e-04, -4.159091012580820026e-05, -3.013278205585369588e-07,  1.736820285922189584e-06, -4.599219827687661978e-07,// 0   , 0   , 0   ,
                 1.012849085655689321e-04, -4.159091012580820026e-05,  4.727921819788054878e-04,  5.523361976811979815e-07, -1.730307422507887473e-07, -3.030009469390110280e-07,// 0   , 0   , 0   ,
                -3.078687958578659292e-08, -3.013278205585369588e-07,  5.523361976811979815e-07,  3.434758685147043306e-06, -8.299226917536411892e-08,  5.842662059539863827e-08,// 0   , 0   , 0   ,
                -1.132513004663809251e-06,  1.736820285922189584e-06, -1.730307422507887473e-07, -8.299226917536411892e-08,  1.006052718494827880e-05,  1.346681994776136150e-06,// 0   , 0   , 0   ,
                -6.485352375515866273e-07, -4.599219827687661978e-07, -3.030009469390110280e-07,  5.842662059539863827e-08,  1.346681994776136150e-06,  3.242298821157115427e-06;// 0   , 0   , 0   ,
//                 0                       ,  0                       ,  0                       ,  0                       ,  0                       ,  0                       , 0.01, 0   , 0   ,
//                 0                       ,  0                       ,  0                       ,  0                       ,  0                       ,  0                       , 0   , 0.01, 0   ,
//                 0                       ,  0                       ,  0                       ,  0                       ,  0                       ,  0                       , 0   , 0   , 0.01;
        }

    public:
        // measurement, local
        class Measurement : public Eigen::Matrix<double, dim_measurement,1> {
        public:
            Measurement(): Eigen::Matrix<double,dim_measurement,1>(Eigen::Matrix<double,dim_measurement,1>::Zero()){
                //rotation(3,0) = 1;
            }

            Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > acceleration(){
                return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 0, 0, 3, 1);
            }

            Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotational_velocity(){
                return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 3, 0, 3, 1);
            }

//            Eigen::Block<Eigen::Matrix<double,dim_measurement,1> > rotation(){
//                //eigen's order of components of a quaterion: x,y,z,w
//                return Eigen::Block<Eigen::Matrix<double,dim_measurement,1> >(this->derived(), 6, 0, 4, 1);
//            }

//            Eigen::Quaterniond getRotationAsQuaternion() /*const*/ {
//                return Eigen::Quaterniond(Eigen::Vector4d(rotation()));
//            }

            Eigen::Matrix<double, dim_measurement_cov,1> toCovarianceCompatibleState(){
//                Eigen::Matrix<double, dim_measurement_cov,1> return_val;
//                Eigen::AngleAxisd rotation;
//                Eigen::Vector3d rotation_vector;

//                rotation = Eigen::AngleAxisd(Eigen::Quaterniond(Eigen::Vector4d(this->rotation())));

//                rotation_vector = rotation.angle()*rotation.axis();

//                return_val << this->acceleration(),
//                              this->rotational_velocity(),
//                              rotation_vector;

//                return return_val;
                return *this;
            }
        };

        void reset();
        void predict(double dt);
        void update(Measurement z);

        // state in global reference frame
        class State : public Eigen::Matrix<double,dim_state,1> {
            public:
                    State(): Eigen::Matrix<double,dim_state,1>(Eigen::Matrix<double,dim_state,1>::Zero()){
                        rotation()(3,0) = 1;
                    }

                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > location(){
                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 0, 0, 3, 1);
                    }

                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > velocity(){
                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 3, 0, 3, 1);
                    }

                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > acceleration(){
                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 6, 0, 3, 1);
                    }

//                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > bias_acceleration(){
//                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 9, 0, 3, 1);
//                    }

                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotation(){
                        //eigen's order of components of a quaterion: x,y,z,w
                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 9, 0, 4, 1);
                    }

                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotational_velocity(){
                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 13, 0, 3, 1);
                    }

//                    Eigen::Block<Eigen::Matrix<double,dim_state,1> > bias_rotational_velocity(){
//                        return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 19, 0, 3, 1);
//                    }

                    State& operator=(Eigen::Matrix<double,dim_state,1> rhs) {
                        this->Eigen::Matrix<double,dim_state,1>::operator=(rhs);
                        return *this;
                    }

                    // add other to this state, i.e., the resulting rotation describes a rotation which rotates at first by "this" and then by "other"
                    State& operator +=(State /*const*/& other){
                        Eigen::Quaterniond temp_rotation = other.getRotationAsQuaternion() * this->getRotationAsQuaternion();
                        Eigen::Matrix<double,dim_state,1>::operator+=(other);

                        // replace with correct rotation
                        this->rotation() = temp_rotation.coeffs();

                        return *this;
                    }

                    // TODO: make it more beautiful
                    State& operator +=(const Eigen::CwiseBinaryOp<Eigen::internal::scalar_product_op<double, double>,
                                                                  const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, const Eigen::Matrix<double, dim_state, 1, 0, dim_state, 1> >,
                                                                  const Eigen::Matrix<double, dim_state, 1, 0, dim_state, 1> > other){
                        // use + operator of states base class
                        *this = *this + other;
                        return *this;
                    }

                    State operator-() const{
                        State temp = *this;
                        Eigen::Quaterniond temp_rotation = temp.getRotationAsQuaternion();

                        // invert directions
                        temp = Eigen::Matrix<double, dim_state,1>::operator-();

                        // replace with correct inverse rotation
                        temp.rotation() = temp_rotation.inverse().coeffs();

                        return temp;
                    }

                    Eigen::Quaterniond getRotationAsQuaternion() /*const*/ {
                        Eigen::Quaterniond return_val = Eigen::Quaterniond(Eigen::Vector4d(rotation()));
                        return return_val;
                    }

                    Eigen::Matrix<double, dim_state_cov,1> toCovarianceCompatibleState(){
                        Eigen::Matrix<double, dim_state_cov,1> return_val;
                        Eigen::AngleAxisd rotation;
                        Eigen::Vector3d rotation_vector;

                        rotation = Eigen::AngleAxisd(Eigen::Quaterniond(Eigen::Vector4d(this->rotation())));

                        rotation_vector = rotation.angle()*rotation.axis();

                        return_val << this->location(),
                                      this->velocity(),
                                      this->acceleration(),
                                      //this->bias_acceleration(),
                                      rotation_vector,
                                      this->rotational_velocity();
                                      //this->bias_rotational_velocity();

                        return return_val;
                    }
        };

        // TODO: get rid of direct matrix accesses
        State toFullState(Eigen::Matrix<double, dim_state_cov,1> covarianceCompatibleState){
            State return_val;

            Eigen::Vector3d rotation_vector;
            rotation_vector << covarianceCompatibleState(9,0), covarianceCompatibleState(10,0), covarianceCompatibleState(11,0);

            Eigen::Quaterniond rotation;

            if(rotation_vector.norm() > 0) {
                rotation = Eigen::Quaterniond(Eigen::AngleAxisd(rotation_vector.norm(), rotation_vector.normalized()));
            } else {
                // zero rotation quaternion
                rotation = Eigen::Quaterniond(1,0,0,0);
            }

            return_val << covarianceCompatibleState.block(0,0,9,1), //location,velocity,acceleration
                          rotation.coeffs(),
                          covarianceCompatibleState.block(12,0,dim_state_cov - 12, 1);

            return return_val;
        }

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

        std::vector<State> sigmaPoints;

    public:
        State state;

        void transitionFunction(State &state, double dt);          // state transition function
        Measurement stateToMeasurementSpaceFunction(State& state); // state to measurement transformation function
        void generateSigmaPoints();

        // covariances
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> Q;         // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement_cov,dim_measurement_cov> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state_cov,dim_state_cov> P;         // covariance matrix of current state

        // TODO: check whether this concept of averageing can be applied directly on the rotations (average axis and average angle)
        // TODO: adjust maximum of iterations
        Eigen::Quaterniond averageRotation(std::vector<Eigen::Quaterniond> rotations, Eigen::Quaterniond mean){
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

            return mean;
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

    UKF<16,15,6,6> ukf;

    typedef UKF<16,15,6,6>::Measurement Measurement;


};

#endif // IMUMODEL_H
