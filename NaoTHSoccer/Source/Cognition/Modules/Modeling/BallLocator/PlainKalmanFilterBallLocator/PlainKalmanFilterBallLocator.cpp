#include "PlainKalmanFilterBallLocator.h"

#include <cmath>

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator():
     epsilon(10e-6)
{
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field",     "draw the modelled ball on the field",                              false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_real_ball_percept", "draw the real incomming ball percept",                             false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:reloadParameters"      , "reloads the kalman filter parameters from the kfParameter object", false);

    getDebugParameterList().add(&kfParameters);

    reloadKFParameters();
}

PlainKalmanFilterBallLocator::~PlainKalmanFilterBallLocator()
{
    getDebugParameterList().remove(&kfParameters);
}

void PlainKalmanFilterBallLocator::execute()
{
    // TODO: remove initialization
    if(!getBallModel().valid && getBallPercept().ballWasSeen)
    {
        Eigen::Vector4d x1;
        x1 << getBallPercept().bearingBasedOffsetOnField.x,
             0,
             getBallPercept().bearingBasedOffsetOnField.y,
             0;

        filter.setState(x1);
    }

    // apply odometry on the filter state, to keep it in the robot's local coordinate system
    applyOdometryOnFilterState(filter);

    const Eigen::Vector4d& x = filter.getState();
    Eigen::Vector2d u; // control vector

    // prediction
    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();

//----------- handle friction as an input control for acceleration

    /*
    rolling resistance = rolling resitance coefficient * normal force
    F_R = d / R * F_N

    F_N = g * weight
    g = 9.81
    weight = ballMass

    deceleration = F_R/ballMass
    */
    double deceleration = c_RR*9810;//[mm/s^2]

    // deceleration has to be in opposite direction of velocity
    u <<  -x(1), -x(3);

    // deceleration vector with absoult deceleration of deceleration
    if(u.norm() > 0){
        u.normalize();
    }
    u *= deceleration;

    double time_until_vel_x_zero = 0;
    double time_until_vel_y_zero = 0;

    if(fabs(u(0)) > epsilon){
        time_until_vel_x_zero = -x(1)/u(0);
    }
    if(fabs(u(1)) > epsilon){
        time_until_vel_y_zero = -x(3)/u(1);
    }

    // 1. predict until velocity is zero
    Eigen::Matrix<double,1,1> dummy;
    dummy(0,0) = 5;

    // predict dt seconds and handle events
    std::vector<Event> events;
    events.push_back(Event(dummy(0,0),                     dt, Event::finalEvent));
    events.push_back(Event(      u(0),  time_until_vel_x_zero, Event::normalEvent));
    events.push_back(Event(      u(1),  time_until_vel_y_zero, Event::normalEvent));

    std::sort(events.begin(),events.end());
    int index = 0;
    double past = 0;
    do {
        double simTime = events[index].time - past;
        if(simTime > epsilon){
            filter.prediction(u,simTime);
            past += simTime;
        }
        *(events[index].u_entry) = 0; // beware! u_entry is a pointer to a entry in the vector u
    } while(events[index++].type == Event::normalEvent);

//----------- handel friction end

    // measurement
    if(getBallPercept().ballWasSeen)
    {
        Eigen::Vector2d z;
        z << getBallPercept().bearingBasedOffsetOnField.x,
             getBallPercept().bearingBasedOffsetOnField.y;

        // check plausibility
        double evalue = evaluatePredictionWithMeasurement(filter,z);

        if(evalue < epsilon)
        {
             // do something
             // fusion/update
            filter.update(z);
        }
        else
        {
            // fusion/update
            filter.update(z);
        }
    }

    // set ball model representation
    getBallModel().position.x = x(0);
    getBallModel().position.y = x(2);
    getBallModel().speed.x = x(1);
    getBallModel().speed.y = x(3);

    // TODO: decide whether model is valid depending on P or time
    // e.g. if (max, min, std)

    if(getBallPercept().ballWasSeen)
    {
      getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());
    }


    if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) > 10000.0) // 10s
    {
      //reset(newPercept);
      getBallModel().valid = false; // model is not valid enymore after 10s
    }
    else
    {
      getBallModel().valid = true;
    }

    doDebugRequest();

    lastFrameInfo     = getFrameInfo();
    lastRobotOdometry = getOdometryData();
}


double PlainKalmanFilterBallLocator::mahalanobisDistanceToState(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const
{
    // reduce state covariances to location
    Eigen::Matrix2d location_cov;
    location_cov << filter.getProcessCovariance()(0,0), 0, filter.getProcessCovariance()(2,2), 0;

    return std::sqrt(((z-filter.getStateInMeasurementSpace()).transpose() * location_cov.inverse() * (z-filter.getStateInMeasurementSpace()))(0,0));
}

double PlainKalmanFilterBallLocator::evaluatePredictionWithMeasurement(const KalmanFilter4d& filter, const Eigen::Vector2d& z) const
{
    return std::exp(((filter.getStateInMeasurementSpace()-z).transpose() * filter.getMeasurementCovariance().inverse() * (filter.getStateInMeasurementSpace()-z))(0,0) * (-0.5));
}

void PlainKalmanFilterBallLocator::applyOdometryOnFilterState(KalmanFilter4d& filter)
{
    const Eigen::Vector4d& x = filter.getState();
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();

    //rotate and translate location part of the filter's state
    Vector2<double> location = Vector2<double>(x(0), x(2)); // translation of the model
    location = odometryDelta * location;

    //just rotate the velocity part of the filter's state
    Vector2<double> velocity = Vector2<double>(x(1), x(3)); // translation of the model
    velocity.rotate(odometryDelta.getAngle());

    Eigen::Vector4d newStateX;
    newStateX << location.x, velocity.x, location.y, velocity.y;

    filter.setState(newStateX);
}

void PlainKalmanFilterBallLocator::doDebugRequest()
{
    if(getBallPercept().ballWasSeen) {
        //to check correctness of the prediction
        DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_real_ball_percept",

          PEN("FF0000", 10);
          CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, getBallPercept().bearingBasedOffsetOnField.y, getFieldInfo().ballRadius-5);
        );
    }

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;

      PEN("FF0000", 30);
      CIRCLE( getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius-10);

      if(getBallModel().valid)
      {
        if(getBallPercept().ballWasSeen)
          PEN("FF9900", 20);
        else
          PEN("0099FF", 20);
      }
      else
        PEN("999999", 20);

      CIRCLE( getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius-10);
      ARROW( getBallModel().position.x, getBallModel().position.y,
             getBallModel().position.x+getBallModel().speed.x,
             getBallModel().position.y+getBallModel().speed.y);

      Eigen::Matrix4d P = filter.getProcessCovariance();

      // draw the distribution
      PEN("00FFFF", 20);
      OVAL(getBallModel().position.x, getBallModel().position.y, P(0,0), P(2,2));
      PEN("FF00FF", 20);
      OVAL(getBallModel().position.x, getBallModel().position.y, P(1,1), P(3,3));
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:reloadParameters",

        reloadKFParameters();

    );
}

void PlainKalmanFilterBallLocator::reloadKFParameters()
{
    Eigen::Matrix2d processNoiseCovariancesSingleDimension;

    processNoiseCovariancesSingleDimension(0,0) = kfParameters.processNoiseQ00;
    processNoiseCovariancesSingleDimension(0,1) = kfParameters.processNoiseQ01;
    processNoiseCovariancesSingleDimension(1,0) = kfParameters.processNoiseQ10;
    processNoiseCovariancesSingleDimension(1,1) = kfParameters.processNoiseQ11;

    filter.setCovarianceOfProcessNoise(processNoiseCovariancesSingleDimension);

    Eigen::Matrix2d measurementNoiseCovariances = Eigen::Matrix2d::Zero();
    measurementNoiseCovariances(0,0) = kfParameters.measurementNoiseR00;
    measurementNoiseCovariances(1,1) = kfParameters.measurementNoiseR11;

    filter.setCovarianceOfMeasurementNoise(measurementNoiseCovariances);

    // wrong function name?
    //ballMass = kfParameters.ballMass;
    c_RR = kfParameters.c_RR;
}
