/**
* @file KalmanFilterBallLocator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class KalmanFilterBallLocator
*/

#include "KalmanFilterBallLocator.h"

KalmanFilterBallLocator::KalmanFilterBallLocator()
  : 
    parameters(getDebugParameterList()), modelIsValid(false), wasReactiveInLastFrame(false)
{
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:draw_ball_on_field", "draw the modelled ball on the field", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:hold_ball_prediction_in_10s", "draw the ball prediction for 10s.", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:draw_ball_percept_after_buffering", "draw the buffered ball percept", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:draw_real_ball_percept", "draw the real incomming ball percept", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:test_model", "set the initial speed to 100mm/s", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:reset_matrices", "reset the model matrices (from Parameterlist)", false);
  DEBUG_REQUEST_REGISTER("KalmanFilterBallLocator:draw_smoothPercept_buffer", "Draw the buffer for smoothing the perception", false);

  reset(getBallPercept());
  resetMatrices();
}

void KalmanFilterBallLocator::resetMatrices()
{
  double rPos   = getSituationStatus().reactiveBallModelNeeded
      ? parameters.sigmaBallPositionReactive : parameters.sigmaBallPosition;
  double rSpeed = getSituationStatus().reactiveBallModelNeeded ?
        parameters.sigmaBallSpeedReactive : parameters.sigmaBallSpeed;
  double q      = parameters.processNoise;

  //R : Measurement noise Covariance (S_Z)

  //X-Coord - kleinere Werte, weniger Glaettung
  Rx[0][0] = rPos; Rx[1][0] = 0;           
  Rx[0][1] = 0;    Rx[1][1] = rSpeed;
  
  //Y-Coord - kleinere Werte, weniger Glaettung
  Ry[0][0] = rPos; Ry[1][0] = 0;     
  Ry[0][1] = 0;    Ry[1][1] = rSpeed;
  

  // Q : Process noise Covariance (S_W)
  Qx[0][0] = q;
  Qy[0][0] = q;    


  //P : estimation error matrix
  Px[0][0] = 250000;
  Px[0][1] = 250000;
  Px[1][0] = 250000;
  Px[1][1] = 250000;

  Py[0][0] = 250000;
  Py[0][1] = 250000;
  Py[1][0] = 250000;
  Py[1][1] = 250000;
}//end resetMatrices


void KalmanFilterBallLocator::execute()
{

    if(getBallPercept().ballWasSeen) {
        //to check correctness of the prediction
        DEBUG_REQUEST("KalmanFilterBallLocator:draw_real_ball_percept",

          PEN("FF0000", 10);
          CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, getBallPercept().bearingBasedOffsetOnField.y, getFieldInfo().ballRadius-5);
        );
    }

    DEBUG_REQUEST("KalmanFilterBallLocator:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;

      PEN("FF0000", 30);
      CIRCLE( getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius-10);
    );

    double validatePerceptWithModel = 0;
    MODIFY("KalmanFilterBallLocator:validatePerceptWithModel", validatePerceptWithModel);
    if (validatePerceptWithModel == 0)  {
        executeKalman(getBallPercept());

        if(!getBallPercept().ballWasSeen){
            getBallModel().resetTimeBallIsSeen();
        }

        return;
    }

    double deltaModelpercept = 2000;
    MODIFY("KalmanFilterBallLocator:deltaModelpercept", deltaModelpercept);

    //updateByTime
    if( badPerceptBuffer.size() > 0 &&
        getFrameInfo().getFrameNumber() - badPerceptBuffer.first().frameInfoWhenBallWasSeen.getFrameNumber() > 200)
    {
      badPerceptBuffer.removeFirst();
    }

    //updateByOdometry
    Pose2D odometryDelta = lastRobotOdometryAll - getOdometryData();

    //update the badPerceptBuffer
    for (int i = 0; i < badPerceptBuffer.size(); i++)
    {
      BallPercept& ball = badPerceptBuffer.getEntry(i);
      ball.bearingBasedOffsetOnField = odometryDelta * ball.bearingBasedOffsetOnField;
    }

    lastRobotOdometryAll = getOdometryData();


    if(getBallPercept().ballWasSeen
        && getBodyState().fall_down_state == BodyState::upright)
    {


        double perceptX = getBallPercept().bearingBasedOffsetOnField.x;
        double perceptY = getBallPercept().bearingBasedOffsetOnField.y;

        double modelX = getBallModel().position.x;
        double modelY = getBallModel().position.y;

        if(fabs( perceptX - modelX) < deltaModelpercept
                   && fabs(perceptY - modelY) < deltaModelpercept)
        {

            executeKalman(getBallPercept());
            return;

        } else {

            badPerceptBuffer.add(getBallPercept());
            //std::cout << " " << std::endl;
            //std::cout <<badPerceptBuffer.getNumberOfEntries() << std::endl;
        }


    }

    if (badPerceptBuffer.size() > 10) {

      reset(badPerceptBuffer.first());
      badPerceptBuffer.clear();

    } //reset




    /* Mitteln der percepte zur selektion auf perceptebene (mit vergleichen von percepten)
     *
     *
     *double useBuffer = 0;
    MODIFY("KalmanFilterBallLocator:useBuffer", useBuffer);
    if (useBuffer == 0)  {
        executeKalman(getBallPercept());
        return;
    }

    //odometry Update
    Pose2D odometryDelta = lastRobotOdometryAll - getOdometryData();

    //update the ballBuffer
    for (int i = 0; i < smoothingPerceptBuffer.getNumberOfEntries(); i++)
    {
      BallPercept& ball = smoothingPerceptBuffer.getEntry(i);
      ball.bearingBasedOffsetOnField = odometryDelta * ball.bearingBasedOffsetOnField;
    }

    lastRobotOdometryAll = getOdometryData();


    if(getBallPercept().ballWasSeen
      && getBodyState().fall_down_state == BodyState::upright)
    {


        const BallPercept currentPercept = getBallPercept();

        unsigned int idxMax = 0;
        if (smoothingPerceptBuffer.getNumberOfEntries() > 4)
            idxMax = smoothingPerceptBuffer.getNumberOfEntries()-1;
        else {
            smoothingPerceptBuffer.add(currentPercept);
            return;
        }

        const double currentX = currentPercept.bearingBasedOffsetOnField.x;
        const double currentY = currentPercept.bearingBasedOffsetOnField.y;

        BallPercept meanPercept = getBallPercept();

        //add percept
        int similarPercepts = 0;

        if (fabs(currentX - smoothingPerceptBuffer[idxMax].bearingBasedOffsetOnField.x) < 1000
         && fabs(currentY - smoothingPerceptBuffer[idxMax].bearingBasedOffsetOnField.y) < 1000) {

              similarPercepts++;
              //meanPercept.bearingBasedOffsetOnField += smoothingPerceptBuffer[idxMax].bearingBasedOffsetOnField/2;
        }

        for (int i = idxMax-1; idxMax > 0 && i > 0 && similarPercepts < 6; i--) {

            if (fabs(currentX - smoothingPerceptBuffer[i-1].bearingBasedOffsetOnField.x) < 1000
             && fabs(currentY - smoothingPerceptBuffer[i-1].bearingBasedOffsetOnField.y) < 1000) {
              similarPercepts++;
              //meanPercept.bearingBasedOffsetOnField += smoothingPerceptBuffer[i].bearingBasedOffsetOnField/2;
            }
        }


        if (similarPercepts > 4) {
            executeKalman(meanPercept);
            //smoothingPerceptBuffer.clear(); better not clearing -> not reactive anymore
        } else {
            smoothingPerceptBuffer.add(currentPercept);
        }
    } //end if perceptSeen

    DEBUG_REQUEST("KalmanFilterBallLocator:draw_smoothPercept_buffer",
      FIELD_DRAWING_CONTEXT;
      PEN("000000", 50);
      for (int i = 0; i < smoothingPerceptBuffer.getNumberOfEntries(); i++)
      {
        CIRCLE(smoothingPerceptBuffer[i].bearingBasedOffsetOnField.x, smoothingPerceptBuffer[i].bearingBasedOffsetOnField.y, 20);
      }//end for
    );*/

} //end execute

void KalmanFilterBallLocator::executeKalman(const BallPercept& newPercept)
{

  DEBUG_REQUEST("KalmanFilterBallLocator:reset_matrices",
    resetMatrices();
  );

  if(getSituationStatus().reactiveBallModelNeeded != wasReactiveInLastFrame)
  {
    resetMatrices();
  }
  
  if(newPercept.ballWasSeen)
  {
    getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());
  }

  //////////////////////////////////
  // Odometry-update (translation)
  //////////////////////////////////
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  Vector2<double> h = Vector2<double>(Sx[0], Sy[0]); // translation of the model
  h = odometryDelta * h;
  Sx[0] = h.x; Sy[0] = h.y;

  lastSeenBall = odometryDelta * lastSeenBall;

  //update the ballBuffer
  for (int i = 0; i < ballBuffer.size(); i++) 
  {
    LocalBallPercept& ball = ballBuffer.getEntry(i);
    ball.lastSeenBall = odometryDelta * ball.lastSeenBall;
  }

  //////////////////////////////////
  // motion-update
  //////////////////////////////////
  
  Vector2<double> Sx_, Sy_;

  if(modelIsValid)
  {
    updateMotion(Sx_, Sy_);
  }

  //////////////////////////////////
  // sensor-update
  //////////////////////////////////

  if(newPercept.ballWasSeen
    && getBodyState().fall_down_state == BodyState::upright)
  {

    //TODO should be also reseted when ball wasnt seen!?
    if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) > 10000.0) // 10s
    {
      reset(newPercept);
      modelIsValid = false; // model is not valid enymore after 10s
    }
    else if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) < 500.0)
    {
      //////////////////////////////////
      // observation
      //////////////////////////////////
      Vector2<double> Zx, Zy;
      // observated position
      Zx.x = newPercept.bearingBasedOffsetOnField.x;
      Zy.x = newPercept.bearingBasedOffsetOnField.y;

      // observated speed
      // select actual and one random observation, or the one and the last!
      double timeDelta;
      Vector2<double>  pickedSeenBall;
      if (parameters.randomizeFrameSelection && ballBuffer.size() > 1) 
      {
        //pick random Element
        const int idx = Math::random(ballBuffer.size());
        timeDelta       = getFrameInfo().getTimeInSeconds() - ballBuffer.getEntry(idx).lastFrameInfoWhenBallSeen.getTimeInSeconds();
        pickedSeenBall  = ballBuffer.getEntry(idx).lastSeenBall;
      } 
      else 
      {
        timeDelta       = getFrameInfo().getTimeInSeconds() - lastFrameInfoWhenBallSeen.getTimeInSeconds();
        pickedSeenBall  = lastSeenBall;
      }

      ASSERT(timeDelta > 0);
      Vector2<double> speed = (newPercept.bearingBasedOffsetOnField-pickedSeenBall) / timeDelta;

      // clip speed to max 4m/s
      if(speed.abs() > 2000.0) speed.normalize(2000.0); // ~7kmh

      Zx.y = speed.x;
      Zy.y = speed.y;

      DEBUG_REQUEST("KalmanFilterBallLocator:test_model",
        //Zx.y = speed.x;
        Zy.y = 1000;
      );
      
      PLOT("ball.speed.x", speed.x);
      PLOT("ball.speed.y", speed.y);

      //////////////////////////////////
      // prediction
      //////////////////////////////////

      Matrix2x2<double> Px_ = Px + Qx;
      Matrix2x2<double> Py_ = Py + Qy;

      // Kalman-Update x
      Kx = Px_ * (Px_ + Rx).invert();
      Sx = Sx_ + Kx * (Zx - Sx_);
      Px = (I - Kx) * Px_;

      // Kalman-Update y
      Ky = Py_ * (Py_ + Ry).invert();
      Sy = Sy_ + Ky * (Zy - Sy_);
      Py = (I - Ky) * Py_;

      modelIsValid = true;
    }//end else

    lastSeenBall = newPercept.bearingBasedOffsetOnField;

    //insert in buffer
    LocalBallPercept ball;
    ball.lastFrameInfoWhenBallSeen = getFrameInfo();
    ball.lastSeenBall = newPercept.bearingBasedOffsetOnField;
    ballBuffer.add(ball);

    lastFrameInfoWhenBallSeen = getFrameInfo();

  }
  else
  {
    Sx = Sx_;
    Sy = Sy_;

    Px += Qx;
    Py += Qy;
  }


  //////////////////////////////////
  // update the model
  //////////////////////////////////

  // DEBUG
  ASSERT(!Math::isNan(getBallModel().speed.x));
  ASSERT(!Math::isNan(getBallModel().speed.y));
  ASSERT(!Math::isNan(getBallModel().position.x));
  ASSERT(!Math::isNan(getBallModel().position.y));

  if(modelIsValid)
  {
    getBallModel().position.x = Sx[0];
    getBallModel().position.y = Sy[0];
    getBallModel().speed.x = Sx[1];      
    getBallModel().speed.y = Sy[1];

    // estimate the line the ball will move along
    getBallModel().futurePosition[0] = getBallModel().position;
    for(int i=1; i <= BALLMODEL_MAX_FUTURE_SECONDS; i++)
    {
      Vector2<double> futurePosition(getBallModel().position);
      Vector2<double> futureSpeed(getBallModel().speed);
      predictByMotionModelContineously(futurePosition, futureSpeed, (double) i);
      getBallModel().futurePosition[i] = futurePosition;
    }
    //Math::LineSegment(getBallModel().position, getBallModel().position+getBallModel().speed);
  }//end if

  getBallModel().valid = modelIsValid;

  lastRobotOdometry = getOdometryData();

  updatePreviewModel();

  PLOT("ballModel.speeed.x", getBallModel().speed.x);
  PLOT("ballModel.speeed.y", getBallModel().speed.y);
  
  DEBUG_REQUEST("KalmanFilterBallLocator:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    
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

    // draw the distribution
    PEN("00FFFF", 20);
    OVAL(getBallModel().position.x, getBallModel().position.y, Px[0][0], Py[0][0]);
    PEN("FF00FF", 20);
    OVAL(getBallModel().position.x, getBallModel().position.y, Px[1][1], Py[1][1]);

    PEN("999999", 10);
    for(int i=1; i <= BALLMODEL_MAX_FUTURE_SECONDS; i++)
    {
      CIRCLE(getBallModel().futurePosition[i].x, getBallModel().futurePosition[i].y, getFieldInfo().ballRadius-5);
    }
  );

  //KalmanFilterBallLocator:draw_ball_prediction_in_10s
  DEBUG_REQUEST("KalmanFilterBallLocator:hold_ball_prediction_in_10s",

    //For easier experiments: just switch on if ball was seen.
    if (newPercept.ballWasSeen) {

      static unsigned int oldFrameNumber = getFrameInfo().getFrameNumber();
      unsigned int newFrameNumber = getFrameInfo().getFrameNumber();

      static Vector2<double> future10position = getBallModel().futurePosition[10];
      static Vector2<double> currentposition = getBallModel().position;

      if (newFrameNumber - oldFrameNumber > 1) {
        future10position = getBallModel().futurePosition[10];
        currentposition  = getBallModel().position;
      }

      PEN("999999", 30);
      CIRCLE(future10position.x, future10position.y, getFieldInfo().ballRadius+8);
      PEN("FF00FF", 30);
      LINE(currentposition.x, currentposition.y, future10position.x, future10position.y);

      oldFrameNumber = newFrameNumber;
      
    }
  );

  //to check correctness of the prediction
  DEBUG_REQUEST("KalmanFilterBallLocator:draw_ball_percept_after_buffering",

    PEN("FF0000", 10);
    CIRCLE(newPercept.bearingBasedOffsetOnField.x, newPercept.bearingBasedOffsetOnField.y, getFieldInfo().ballRadius-5);
  );


  lastFrameInfo = getFrameInfo();
  wasReactiveInLastFrame = getSituationStatus().reactiveBallModelNeeded;
}//end executeKalman

void KalmanFilterBallLocator::updateMotion(Vector2<double>& Sx_, Vector2<double>& Sy_)
{
  // time elapsed sinse last execution
  double timeDelta = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
  GT_TRACE("time " << getFrameInfo().getTimeInSeconds());
  ASSERT(timeDelta > 0);

  if(timeDelta > 10.0) return;

  // motion update
  Vector2<double> position(Sx.x, Sy.x);
  Vector2<double> speed(Sx.y, Sy.y);

  predictByMotionModelContineously(position, speed, timeDelta);

  Sx_.x = position.x;
  Sy_.x = position.y;
  Sx_.y = speed.x;
  Sy_.y = speed.y;
}//end updateMotion


void KalmanFilterBallLocator::predictByMotionModelContineously(Vector2<double>& position, Vector2<double>& speed, double time)
{
  static double time_step = 0.2; // 200 ms in s 

  ASSERT(time > 0);

  ASSERT(time <= 10.0); // maximal 10s steps ~ 50 steps

  // integrate
  while(time > time_step)
  {
    predictByMotionModel(position, speed, time_step);
    time -= time_step;
  }//end while

  predictByMotionModel(position, speed, time);
}//end predictByMotionModelContineously

void KalmanFilterBallLocator::predictByMotionModel(Vector2<double>& position, Vector2<double>& speed, double time)
{
  double ball_mass = parameters.ballMass;
  double alpha = parameters.frictionCoefficiant;

  double k = std::exp(-alpha/ball_mass*time);

  speed *= k;
  position += speed*time;
}//end predictByMotionModel


void KalmanFilterBallLocator::reset(BallPercept newPercept)
{
  if(!newPercept.ballWasSeen)
  {
    // Kalman-Position
    Sx.x = 0;
    Sy.x = 0;

    // Kalman-Speed
    Sx.y = 0;
    Sy.y = 0;

    ballBuffer.clear();

  }else
  {
    // Kalman-Position
    Sx.x = newPercept.bearingBasedOffsetOnField.x;
    Sy.x = newPercept.bearingBasedOffsetOnField.y;

    // Kalman-Speed
    Sx.y = 0;
    Sy.y = 0;
  }

  modelIsValid = false;
}//end reset

void KalmanFilterBallLocator::updatePreviewModel()
{
  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;
  
  Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
  Pose2D rFootPose(rFoot.rotation.getZAngle(), rFoot.translation.x, rFoot.translation.y);

  Vector2<double> ballLeftFoot  = lFootPose/getBallModel().position;
  Vector2<double> ballRightFoot = rFootPose/getBallModel().position;

  getBallModel().positionPreview = getMotionStatus().plannedMotion.hip / getBallModel().position;
  getBallModel().positionPreviewInLFoot = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;
  getBallModel().positionPreviewInRFoot = getMotionStatus().plannedMotion.rFoot / ballRightFoot;
}
