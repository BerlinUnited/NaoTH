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

  DEBUG_REQUEST_REGISTER("ImageProcessor:TopCam:draw_horizon", "draws the artificial horizon", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BottomCam:draw_horizon", "draws the artificial horizon", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:TopCam:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BottomCam:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:TopCam:draw_ball_in_image", "draw ball in the image if found", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BottomCam:draw_ball_in_image", "draw ball in the image if found", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:TopCam:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BottomCam:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:TopCam:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BottomCam:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);

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
  //todo: for consistency reasonst this should be done within the detectors
  getBallPercept().reset();
  getBallPerceptTop().reset();
  getGoalPercept().reset();
  getGoalPerceptTop().reset();
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
  theLineDetector->getModuleT()->execute(CameraInfo::Top);
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

    DEBUG_REQUEST("ImageProcessor:BottomCam:draw_ball_in_image",
      CIRCLE_PX(ColorClasses::red, (int)getBallPercept().centerInImage.x, (int)getBallPercept().centerInImage.y, (int)getBallPercept().radiusInImage);
    );

    DEBUG_REQUEST("ImageProcessor:BottomCam:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, 
             getBallPercept().bearingBasedOffsetOnField.y,
             getFieldInfo().ballRadius);
    );


    // estimate the position of the relative to the camera based on the 
    // size of the ball
    // HACK: +2 pixel because the ball is always recognized to small
    double ballRadiusOffset = 2.0;
    MODIFY("ImageProcessor:ballRadiusOffset", ballRadiusOffset);

    // for the bottom image
    double y = getBallPercept().centerInImage.y - getImage().cameraInfo.getOpticalCenterY();
    double f = getImage().cameraInfo.getFocalLength();
    double r = getBallPercept().radiusInImage + ballRadiusOffset;
    double alpha_y = atan2(y,f) - atan2(y-r,f);
    double q = -1;
    
    if(fabs(sin(alpha_y)) > 1e-3) {
      q = getFieldInfo().ballRadius / sin(alpha_y);
    }

    Vector3d ballCenter;
    ballCenter.x = getImage().cameraInfo.getFocalLength();
    ballCenter.y = -getBallPercept().centerInImage.x + getImage().cameraInfo.getOpticalCenterX();
    ballCenter.z = -getBallPercept().centerInImage.y + getImage().cameraInfo.getOpticalCenterY();
    ballCenter.normalize(q);

    Vector3d ballCenterGlobal = getCameraMatrix()*ballCenter;
    ballCenter = ballCenterGlobal; // in roboter coordinates
    if(q > -1) {
      getBallPercept().sizeBasedRelativePosition = ballCenter;
    }
    DEBUG_REQUEST("ImageProcessor:BottomCam:ballpos_relative_3d",
      SPHERE(ColorClasses::orange, getFieldInfo().ballRadius, ballCenterGlobal);
      LINE_3D(ColorClasses::red, getCameraMatrix().translation, ballCenterGlobal);
    );


    // ... the same for the top image
    double yTop = getBallPerceptTop().centerInImage.y - getImageTop().cameraInfo.getOpticalCenterY();
    double fTop = getImageTop().cameraInfo.getFocalLength();
    double rTop = getBallPerceptTop().radiusInImage + ballRadiusOffset;
    double alpha_yTop = atan2(yTop,fTop) - atan2(yTop-rTop,fTop);
    double qTop = -1;

    if(fabs(sin(alpha_yTop)) > 1e-3) {
      qTop = getFieldInfo().ballRadius / sin(alpha_yTop);
    }

    Vector3d ballCenterTop;
    ballCenterTop.x = getImageTop().cameraInfo.getFocalLength();
    ballCenterTop.y = -getBallPerceptTop().centerInImage.x + getImageTop().cameraInfo.getOpticalCenterX();
    ballCenterTop.z = -getBallPerceptTop().centerInImage.y + getImageTop().cameraInfo.getOpticalCenterY();
    ballCenterTop.normalize(qTop);

    Vector3d ballCenterGlobalTop = getCameraMatrixTop()*ballCenter;
    ballCenterTop = ballCenterGlobalTop; // in roboter coordinates
    if(qTop > -1) {
      getBallPerceptTop().sizeBasedRelativePosition = ballCenterTop;
    }

    DEBUG_REQUEST("ImageProcessor:TopCam:ballpos_relative_3d",
      SPHERE(ColorClasses::orange, getFieldInfo().ballRadius, ballCenterGlobalTop);
      LINE_3D(ColorClasses::red, getCameraMatrixTop().translation, ballCenterGlobalTop);
    );

  }//end if ballWasSeen
  else
  {
    // no ball in the image found

    Vector2<int> projectedBall = CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo,
        Vector3<double>(getBallPercept().bearingBasedOffsetOnField.x,
                        getBallPercept().bearingBasedOffsetOnField.y, 
                        getFieldInfo().ballRadius));
    Vector2<int> projectedBallTop = CameraGeometry::relativePointToImage(getCameraMatrixTop(), getImageTop().cameraInfo,
        Vector3<double>(getBallPerceptTop().bearingBasedOffsetOnField.x,
                        getBallPerceptTop().bearingBasedOffsetOnField.y, 
                        getFieldInfo().ballRadius));

    //project the old percept in the image
    DEBUG_REQUEST("ImageProcessor:TopCam:mark_previous_ball",
      TOP_CIRCLE_PX(ColorClasses::gray, (int)projectedBallTop.x, (int)projectedBallTop.y, (int)getBallPerceptTop().radiusInImage);
    );
    DEBUG_REQUEST("ImageProcessor:BottomCam:mark_previous_ball",
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

    DEBUG_REQUEST("ImageProcessor:TopCam:draw_ball_in_image",
      TOP_CIRCLE_PX(ColorClasses::red, (int)getBallPerceptTop().centerInImage.x, (int)getBallPerceptTop().centerInImage.y, (int)getBallPerceptTop().radiusInImage);
    );

    DEBUG_REQUEST("ImageProcessor:TopCam:draw_ball_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPerceptTop().bearingBasedOffsetOnField.x, 
             getBallPerceptTop().bearingBasedOffsetOnField.y,
             getFieldInfo().ballRadius);
    );
  }

  //draw horizon to image
  DEBUG_REQUEST("ImageProcessor:TopCam:draw_horizon",
    Vector2<double> a(getArtificialHorizonTop().begin());
    Vector2<double> b(getArtificialHorizonTop().end());
    TOP_LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("ImageProcessor:BottomCam:draw_horizon",
    Vector2<double> a(getArtificialHorizon().begin());
    Vector2<double> b(getArtificialHorizon().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("ImageProcessor:classify_ball_color",
    // color experiment
    for(unsigned char x = 0; x < getImage().cameraInfo.resolutionWidth; x++)
    {
      for(unsigned char y = 0; y < getImage().cameraInfo.resolutionHeight; y++)
      {
        Pixel pixel = getImage().get(x,y);

        //double d = (Math::sqr((255.0 - (double)pixel.u)) + Math::sqr((double)pixel.v)) / (2.0*255.0);

        double d = (Math::sqr((double)pixel.u) + Math::sqr((255.0 - (double)pixel.v))) / (2.0*255.0);
        unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);
        
        /*
        if(t > 120)
          //naoth::ImageDrawings::drawPointToImage(DebugImageDrawings::getInstance(),x,y,pixel.y,pixel.u,pixel.v);
          POINT_PX(pixel.y,pixel.u,pixel.v,x,y);
        else*/
          //naoth::ImageDrawings::drawPointToImage(DebugBottomImageDrawings::getInstance(),x,y,t,0,0);
          POINT_PX( (unsigned char) x, (unsigned char) y, t, 0, 0);

      }//end for
    }//end for
  );
}//end execute

