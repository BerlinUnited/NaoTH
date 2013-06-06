/**
* @file ImageProcessor.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageProcessor
*/

#include "ImageProcessor.h"

// Debug

#include "Tools/Debug/DebugDrawings3D.h"

// Tools

ImageProcessor::ImageProcessor()
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_horizon", "draws the artificial horizon", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_horizon_top", "draws the artificial horizon", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_in_image", "draw ball in the image if found", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);


  DEBUG_REQUEST_REGISTER("ImageProcessor:classify_ball_color", "", false);

  theHistogramFieldDetector = registerModule<HistogramFieldDetector>("HistogramFieldDetector");
  theHistogramFieldDetector->setEnabled(true);

  theBodyContourProvider = registerModule<BodyContourProvider>("BodyContourProvider");
  theBodyContourProvider->setEnabled(true);

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

  theLineClusterProvider = registerModule<LineClusterProvider>("LineClusterProvider");
  theLineClusterProvider->setEnabled(true);

  theGoalDetector = registerModule<GoalDetector>("GoalDetector");
  theGoalDetector->setEnabled(true);

}//end constructor


void ImageProcessor::execute()
{
  //reset the Representations:
  
  getBallPercept().reset();
  getBallPerceptTop().reset();
  getGoalPercept().reset();
  getScanLineEdgelPercept().reset();
  getScanLineEdgelPerceptTop().reset();
  getLinePercept().reset();
  getPlayersPercept().reset();

  GT_TRACE("executing HistogramFieldDetector");
  STOPWATCH_START("HistogramFieldDetector");
  theHistogramFieldDetector->execute();
  theHistogramFieldDetector->getModuleT()->execute(CameraInfo::Top);
  STOPWATCH_STOP("HistogramFieldDetector");

  GT_TRACE("executing BodyContourProvider");
  STOPWATCH_START("BodyContourProvider");
  theBodyContourProvider->execute();
  theBodyContourProvider->getModuleT()->execute(CameraInfo::Top);
  STOPWATCH_STOP("BodyContourProvider");

  GT_TRACE("executing RobotDetector");
  STOPWATCH_START("RobotDetector");
  theRobotDetector->execute();
  STOPWATCH_STOP("RobotDetector");

  GT_TRACE("executing GoalDetector");
  STOPWATCH_START("GoalDetector");
  theGoalDetector->execute();
  STOPWATCH_STOP("GoalDetector");

  GT_TRACE("executing ScanLineEdgelDetector");
  STOPWATCH_START("ScanLineEdgelDetector");
  theScanLineEdgelDetector->execute();
  theScanLineEdgelDetector->getModuleT()->execute(CameraInfo::Top);
  STOPWATCH_STOP("ScanLineEdgelDetector");

  GT_TRACE("executing FieldDetector");
  STOPWATCH_START("FieldDetector");
  theFieldDetector->execute();
  theFieldDetector->getModuleT()->execute(CameraInfo::Top);
  STOPWATCH_STOP("FieldDetector");

  GT_TRACE("executing BallDetector");
  STOPWATCH_START("BallDetector");
  theBallDetector->execute();
  theBallDetector->getModuleT()->execute(CameraInfo::Top);
  STOPWATCH_STOP("BallDetector");

  GT_TRACE("executing LineDetector");
  STOPWATCH_START("LineDetector");
  theLineDetector->execute();
  STOPWATCH_STOP("LineDetector");

  GT_TRACE("executing rest of ImageProcessor::execute()");

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
             getFieldInfo().ballRadius);
    );


    // estimate the position of the relative to the camera based on the 
    // size of the ball
    double y = getBallPercept().centerInImage.y - getImage().cameraInfo.getOpticalCenterY();
    double f = getImage().cameraInfo.getFocalLength();
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
    ballCenter.x = getImage().cameraInfo.getFocalLength();
    ballCenter.y = -getBallPercept().centerInImage.x + getImage().cameraInfo.getOpticalCenterX();
    ballCenter.z = -getBallPercept().centerInImage.y + getImage().cameraInfo.getOpticalCenterY();
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



  // estimate the relative position of the ball
  if(getBallPerceptTop().ballWasSeen)
  {
    // estimate the projection of the ball on the ground
    getBallPerceptTop().ballWasSeen = CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrixTop(), 
      getImageTop().cameraInfo,
      getBallPerceptTop().centerInImage.x, 
      getBallPerceptTop().centerInImage.y, 
      getFieldInfo().ballRadius,
      getBallPerceptTop().bearingBasedOffsetOnField);

    getBallPerceptTop().frameInfoWhenBallWasSeen = getFrameInfo();

    DEBUG_REQUEST("ImageProcessor:draw_ball_in_image",
      CIRCLE_PX(ColorClasses::red, (int)getBallPerceptTop().centerInImage.x, (int)getBallPerceptTop().centerInImage.y, (int)getBallPercept().radiusInImage);
    );

    DEBUG_REQUEST("ImageProcessor:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPerceptTop().bearingBasedOffsetOnField.x, 
             getBallPerceptTop().bearingBasedOffsetOnField.y,
             getFieldInfo().ballRadius);
    );
  }



  //draw horizon to image
  DEBUG_REQUEST("ImageProcessor:draw_horizon",
    Vector2d a(getArtificialHorizon().begin());
    Vector2d b(getArtificialHorizon().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );
  DEBUG_REQUEST("ImageProcessor:draw_horizon_top",
    Vector2d a(getArtificialHorizonTop().begin());
    Vector2d b(getArtificialHorizonTop().end());
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

