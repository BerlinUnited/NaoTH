#ifndef IMUMODEL_H
#define IMUMODEL_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"

#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
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
    };

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
        Eigen::Matrix<double,dim_state,dim_state> Q;             // covariance matrix of process noise
        Eigen::Matrix<double,dim_measurement,dim_measurement> R; // covariance matrix of measurement noise
        Eigen::Matrix<double,dim_state,dim_state> P;             // covariance matrix of current state
};

class IMUModel: private IMUModelBase
{
public:
    IMUModel();

    void execute();
    void writeIMUData();
    void resetFilter();

private:

    UKF<22,6> ukf;

    typedef UKF<22,6>::Measurement Measurement;


};

#endif // IMUMODEL_H
