#include "PlainKalmanFilterBallLocator.h"

#include <cmath>

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator():
     epsilon(10e-6)
{
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field",     "draw the modelled ball on the field",  false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_real_ball_percept", "draw the real incomming ball percept", false);

    getDebugParameterList().add(&kfParameters);
}

PlainKalmanFilterBallLocator::~PlainKalmanFilterBallLocator()
{
    getDebugParameterList().remove(&kfParameters);
}

void PlainKalmanFilterBallLocator::execute()
{

    if(!getBallModel().valid && getBallPercept().ballWasSeen)
    {
        Eigen::Vector4d x;
        x << getBallPercept().bearingBasedOffsetOnField.x,
             0,
             getBallPercept().bearingBasedOffsetOnField.y,
             0;

        filter.setState(x);
    }

    //////////////////////////////////
    // Odometry-update (translation): move the model into the current local coordinte system
    //////////////////////////////////

//    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
//    Vector2<double> h = Vector2<double>(Sx[0], Sy[0]); // translation of the model
//    h = odometryDelta * h;
//    Sx[0] = h.x; Sy[0] = h.y;
//    How to handle speed / velocity -> should be rotate

    Eigen::Vector4d x = filter.getState();
    Eigen::Vector4d u = Eigen::Vector4d::Zero(); // control vector

// handle friction as an input control for acceleration
//    double deceleration = 4;
//    if(fabs(x(1)) > epsilon) {
//        u(1) = ((x(1) < 0) ? 1 : -1) * deceleration;
//    }

//    if(fabs(x(3)) > epsilon) {
//        u(3) = ((x(3) < 0) ? 1 : -1) * deceleration;
//    }

    // prediction
    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
    filter.prediction(u, dt);

    // measurement
    newPercept = getBallPercept();
    if(newPercept.ballWasSeen)
    {
        Eigen::Vector2d z;
        z << newPercept.bearingBasedOffsetOnField.x,
             newPercept.bearingBasedOffsetOnField.y;

        // fusion/update
        filter.update(z);
    }

    x = filter.getState();

    // set ball model representation
    getBallModel().position.x = x(0);
    getBallModel().position.y = x(2);
    getBallModel().speed.x = x(1);
    getBallModel().speed.y = x(3);

    // TODO: decide whether model is valid depending on P or time
    // e.g. if (max, min, std)

    if(newPercept.ballWasSeen)
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

    lastFrameInfo = getFrameInfo();
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
        if(newPercept.ballWasSeen)
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

      Eigen::Matrix4d P = filter.getCovariance();

      // draw the distribution
      PEN("00FFFF", 20);
      OVAL(getBallModel().position.x, getBallModel().position.y, P(0,0), P(2,2));
      PEN("FF00FF", 20);
      OVAL(getBallModel().position.x, getBallModel().position.y, P(1,1), P(3,3));
    );
}
