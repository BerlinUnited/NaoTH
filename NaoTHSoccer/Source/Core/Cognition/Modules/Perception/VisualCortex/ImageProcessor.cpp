/**
* @file ImageProcessor.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageProcessor
*/

#include "ImageProcessor.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/Stopwatch.h"

//#include "Tools/Debug/DebugBufferedOutput.h"

// Tools
#include "Tools/Math/Line.h"
#include "Tools/CameraGeometry.h"

ImageProcessor::ImageProcessor()
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_horizon", "draws the artificial horizon", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_in_image", "draw ball in the image if found", true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);


  DEBUG_REQUEST_REGISTER("ImageProcessor:classify_ball_color", "", false);

  theScanLineEdgelDetector = registerModule<ScanLineEdgelDetector>("ScanLineEdgelDetector");
  theScanLineEdgelDetector->setEnabled(true);

  theFieldDetector = registerModule<FieldDetector>("FieldDetector");
  theFieldDetector->setEnabled(true);

  theBallDetector = registerModule<BallDetector>("BallDetector");
  theBallDetector->setEnabled(true);

  theRobotDetector = registerModule<RobotDetector>("RobotDetector");
  theRobotDetector->setEnabled(true);

  theLineDetector = registerModule<LineDetector>("LineDetector");
  theLineDetector->setEnabled(true);

  theGoalDetector = registerModule<GoalDetector>("GoalDetector");
  theGoalDetector->setEnabled(true);

  thePerceprionsVusalization = registerModule<PerceptionsVisualization>("PerceptionsVisualization");
  thePerceprionsVusalization->setEnabled(true);

}//end constructor


void ImageProcessor::execute()
{
  //reset the Representations:
  
  getBallPercept().reset();
  getGoalPercept().reset();
  getScanLineEdgelPercept().reset();
  getLinePercept().reset();
  getPlayersPercept().reset();


  STOPWATCH_START("ScanLineEdgelDetector");
  theScanLineEdgelDetector->execute();
  STOPWATCH_STOP("ScanLineEdgelDetector");

  STOPWATCH_START("FieldDetector");
  theFieldDetector->execute();
  STOPWATCH_STOP("FieldDetector");

  STOPWATCH_START("BallDetector");
  theBallDetector->execute();
  STOPWATCH_STOP("BallDetector");

  STOPWATCH_START("RobotDetector");
  theRobotDetector->execute();
  STOPWATCH_STOP("RobotDetector");

  STOPWATCH_START("LineDetector");
  theLineDetector->execute();
  STOPWATCH_STOP("LineDetector");

  STOPWATCH_START("GoalDetector");
  theGoalDetector->execute();
  STOPWATCH_STOP("GoalDetector");

  STOPWATCH_START("PerceptionsVisualization");
  thePerceprionsVusalization->execute();
  STOPWATCH_STOP("PerceptionsVisualization");



  // estimate the relative position of the ball
  if(getBallPercept().ballWasSeen)
  {
    // estimate the projection of the ball on the ground
    getBallPercept().ballWasSeen = CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), 
      getImage().cameraInfo,
      getBallPercept().centerInImage.x, 
      getBallPercept().centerInImage.y, 
      getFieldInfo().ballRadius,
      getBallPercept().bearingBasedOffsetOnField);

    getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();

    DEBUG_REQUEST("ImageProcessor:draw_ball_in_image",
      CIRCLE_PX(ColorClasses::red, (int)getBallPercept().centerInImage.x, (int)getBallPercept().centerInImage.y, (int)getBallPercept().radiusInImage);
    );

    DEBUG_REQUEST("ImageProcessor:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, 
             getBallPercept().bearingBasedOffsetOnField.y,
             45);

//      PLOT("ball.field.y", getBallPercept().bearingBasedOffsetOnField.y);
//      PLOT("ball.field.x", getBallPercept().bearingBasedOffsetOnField.x);
    );


    // estimate the position of the relative to the camera based on the 
    // size of the ball
    double y = getBallPercept().centerInImage.y - getImage().cameraInfo.opticalCenterY;
    double f = getImage().cameraInfo.focalLength;
    // HACK: +2 pixel because the ball is always recognized to small
    double ballRadiusOffset = 2.0;
    MODIFY("ImageProcessor:ballRadiusOffset", ballRadiusOffset);
    double r = getBallPercept().radiusInImage + ballRadiusOffset;

    double alpha_y = atan2(y,f) - atan2(y-r,f);
    double q = -1;
    //double pixesSize = 3.6 * 1e-3; // in mm

    if(fabs(sin(alpha_y)) > 1e-3)
    {
      q = getFieldInfo().ballRadius / sin(alpha_y);
    }

    Vector3<double> ballCenter;
    ballCenter.x = getImage().cameraInfo.focalLength;
    ballCenter.y = -getBallPercept().centerInImage.x + getImage().cameraInfo.opticalCenterX;
    ballCenter.z = -getBallPercept().centerInImage.y + getImage().cameraInfo.opticalCenterY;
    ballCenter.normalize(q);

    Vector3<double> ballCenterGlobal = getCameraMatrix()*ballCenter;
    ballCenter = ballCenterGlobal; // in roboter coordinates
    if(q > -1)
    {
      getBallPercept().sizeBasedRelativePosition = ballCenter;
    }

    DEBUG_REQUEST("ImageProcessor:ballpos_relative_3d",
      SPHERE(ColorClasses::orange, getFieldInfo().ballRadius, ballCenterGlobal);
      LINE_3D(ColorClasses::red, getCameraMatrix().translation, ballCenterGlobal);
    );

  }//end if ballWasSeen
  else
  {
    // no ball in the image found

    Vector2<int> projectedBall = CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo,
        Vector3<double>(getBallPercept().bearingBasedOffsetOnField.x,
                        getBallPercept().bearingBasedOffsetOnField.y, 
                        getFieldInfo().ballRadius));

    //project the old percept in the image
    DEBUG_REQUEST("ImageProcessor:mark_previous_ball",
      CIRCLE_PX(ColorClasses::gray, (int)projectedBall.x, (int)projectedBall.y, (int)getBallPercept().radiusInImage);
    );
  }




  //draw horizon to image
  DEBUG_REQUEST("ImageProcessor:draw_horizon",
    Vector2<double> a(getCameraMatrix().horizon.begin());
    Vector2<double> b(getCameraMatrix().horizon.end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );




  DEBUG_REQUEST("ImageProcessor:classify_ball_color",
    // color experiment
    for(unsigned int x = 0; x < getImage().cameraInfo.resolutionWidth; x++)
    {
      for(unsigned int y = 0; y < getImage().cameraInfo.resolutionHeight; y++)
      {
        Pixel pixel = getImage().get(x,y);

        //double d = (Math::sqr((255.0 - (double)pixel.u)) + Math::sqr((double)pixel.v)) / (2.0*255.0);

        double d = (Math::sqr((double)pixel.u) + Math::sqr((255.0 - (double)pixel.v))) / (2.0*255.0);
        unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);
        
        /*
        if(t > 120)
          naoth::ImageDrawings::drawPointToImage(DebugImageDrawings::getInstance(),x,y,pixel.y,pixel.u,pixel.v);
        else*/
          naoth::ImageDrawings::drawPointToImage(DebugImageDrawings::getInstance(),x,y,t,0,0);
      }//end for
    }//end for
  );
}//end execute

