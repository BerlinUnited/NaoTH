#include "PlainKalmanFilterBallLocator.h"

#include <cmath>

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator():
     dt(0.03),
     epsilon(10e-6)
{
    // state transition matrix
    F << 1, dt, 0, 0,
         0,  1, 0, 0,
         0,  0, 1, dt,
         0,  0, 0, 1;

    // control matrix
    B << 0,  0, 0,  0,
         1, dt, 0,  0,
         0,  0, 0,  0,
         0,  0, 1, dt;

    // covariance matrix of process noise (values taken from old kalman filter)
    double q = 3;
    Q << q, 0, 0, 0,
         0, 0, 0, 0,
         0, 0, q, 0,
         0, 0, 0, 0;

    // inital covariance matrix of current state (values taken from old kalman filter)
    double p = 250000;
    P << p, p, 0, 0,
         p, p, 0, 0,
         0, 0, p, p,
         0, 0, p, p;

    // state to measurement transformation matrix
    H << 1, 0, 0, 0,
         0, 0, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 0;

    // covariance matrix of measurement noise (values taken from old kalman filter)
    R << 100, 0,   0, 0,
           0, 0,   0, 0,
           0, 0, 100, 0,
           0, 0,   0, 0;
}

PlainKalmanFilterBallLocator::~PlainKalmanFilterBallLocator()
{

}

void PlainKalmanFilterBallLocator::execute()
{
    //////////////////////////////////
    // Odometry-update (translation): move the model into the current local coordinte system
    //////////////////////////////////

//    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
//    Vector2<double> h = Vector2<double>(Sx[0], Sy[0]); // translation of the model
//    h = odometryDelta * h;
//    Sx[0] = h.x; Sy[0] = h.y;
//    How to handle speed / velocity -> should be rotate

    // handle friction as an input control for acceleration
    double deceleration = 4;
    if(fabs(x(1)) <= epsilon) {
        u(1) = 0;
    } else {
        u(1) = ((x(1) < 0) ? 1 : -1) * deceleration;
    }

    if(fabs(x(3)) <= epsilon) {
        u(3) = 0;
    } else {
        u(3) = ((x(3) < 0) ? 1 : -1) * deceleration;
    }

    // prediction
    x_pre = F * x + B * u; // + w;

    P_pre = F * P * F.transpose() + Q;

    // measurement
    newPercept = getBallPercept();
    if(newPercept.ballWasSeen)
    {
        z << newPercept.bearingBasedOffsetOnField.x,
             0,
             newPercept.bearingBasedOffsetOnField.y,
             0;

        // fusion/update
        K = P_pre * H.transpose() * (H * P_pre * H.transpose() + R).inverse();

        x_corr = x_pre + K * (z - H * x_pre );

        P_corr = P_pre - K*H*P_pre;

        x = x_corr;
        P = P_corr;
    }
    else
    {
        x = x_pre;
        P = P_pre;
    }

    // set ball model representation
    getBallModel().position.x = x(0);
    getBallModel().position.y = x(2);
    getBallModel().speed.x = x(1);
    getBallModel().speed.y = x(3);

    // TODO: decide whether model is valid depending on P or time
    // e.g. if (max, min, std)
    if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) > 10000.0) // 10s
    {
      //reset(newPercept);
      getBallModel().valid = false; // model is not valid enymore after 10s
    }
    else
    {
     getBallModel().valid = true;
    }
}
