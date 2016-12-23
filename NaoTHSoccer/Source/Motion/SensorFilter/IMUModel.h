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

template <int dim_state, int dim_measurement>
class UKF {
    public:
        typedef Eigen::Matrix<double,dim_measurement,1> Measurement;

        void predict(double dt);
        void update(Measurement z);

    // state in global reference frame
    class State : public Eigen::Matrix<double,dim_state,1> {
        public:
                Eigen::Block<Eigen::Matrix<double,dim_state,1> > location(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 0, 0, 3, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > velocity(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 3, 0, 3, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > acceleration(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 6, 0, 3, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > bias_acceleration(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 9, 0, 3, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotation(){
                    //eigen's order of components of a quaterion: x,y,z,w
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 12, 0, 4, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > rotational_velocity(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 16, 0, 3, 1);
                }

                Eigen::Block<Eigen::Matrix<double,dim_state,1> > bias_rotational_velocity(){
                    return Eigen::Block<Eigen::Matrix<double,dim_state,1> >(this->derived(), 19, 0, 3, 1);
                }

                State& operator=(Eigen::Matrix<double,dim_state,1> rhs) {
                    this->Eigen::Matrix<double,dim_state,1>::operator=(rhs);
                    return *this;
                }

                Eigen::Matrix<double, dim_state-1,1> toCovarianceCompatibleState(){
                    Eigen::Matrix<double, dim_state-1,1> return_val;
                    Eigen::AngleAxisd rotation;
                    Eigen::Vector3d rotation_vector;

                    rotation = Eigen::AngleAxisd(Eigen::Quaterniond(Eigen::Vector4d(this->rotation())));

                    rotation_vector = rotation.angle()*rotation.axis();

                    return_val << this->location(),
                                  this->velocity(),
                                  this->acceleration(),
                                  this->bias_acceleration(),
                                  rotation_vector,
                                  this->rotational_velocity(),
                                  this->bias_rotational_velocity();

                    return return_val;
                }
    };

    State toFullState(Eigen::Matrix<double, dim_state-1,1> covarianceCompatibleState){
        State return_val;

        Eigen::Vector3d rotation_vector;
        rotation_vector << covarianceCompatibleState(12,0), covarianceCompatibleState(13,0), covarianceCompatibleState(14,0);

        Eigen::Quaterniond rotation = Eigen::Quaterniond(Eigen::AngleAxisd(rotation_vector.norm(), rotation_vector.normalized()));

        return_val << covarianceCompatibleState.block(0,0,12,1), //location,velocity,acceleration
                      rotation.coeffs(),
                      covarianceCompatibleState.block(15,0,dim_state - 1 - 15, 1);

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
        std::vector<Eigen::Vector3d> rotational_differences;

    public:
        State state;

        void transitionFunction(State &state, double dt);          // state transition function
        Measurement stateToMeasurementSpaceFunction(State& state); // state to measurement transformation function
        void generateSigmaPoints();

        // covariances
        Eigen::Matrix<double,dim_state-1,dim_state-1> Q;         // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement,dim_measurement> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state-1,dim_state-1> P;         // covariance matrix of current state
};

class IMUModel: private IMUModelBase
{
public:
    IMUModel();

    void execute();
    void writeIMUData();
    void resetFilter();
    void plots();

private:

    UKF<22,6> ukf;

    typedef UKF<22,6>::Measurement Measurement;


};

#endif // IMUMODEL_H
