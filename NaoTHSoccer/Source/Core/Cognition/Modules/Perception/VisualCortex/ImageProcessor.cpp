/**
* @file ImageProcessor.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageProcessor
*/

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_horizon", "draws the artificial horizon", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:draw_ball_in_image", "draw ball in the image if found", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:classify_ball_color", "", false);



  DEBUG_REQUEST_REGISTER("ImageProcessor:HistogramFieldDetector:execute", " ", true);
  theHistogramFieldDetector = registerModule<HistogramFieldDetector>("HistogramFieldDetector");
  theHistogramFieldDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BodyContourProvider:execute", " ", true);
  theBodyContourProvider = registerModule<BodyContourProvider>("BodyContourProvider");
  theBodyContourProvider->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:execute", " ", false);
  theScanLineEdgelDetector = registerModule<ScanLineEdgelDetector>("ScanLineEdgelDetector");
  theScanLineEdgelDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldDetector:execute", " ", true);
  theFieldDetector = registerModule<FieldDetector>("FieldDetector");
  theFieldDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:execute", " ", false);
  theBallDetector = registerModule<BallDetector>("BallDetector");
  theBallDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:execute", " ", false);
  theRobotDetector = registerModule<RobotDetector>("RobotDetector");
  theRobotDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:LineDetector:execute", " ", true);
  theLineDetector = registerModule<LineDetector>("LineDetector");
  theLineDetector->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:LineClusterProvider:execute", " ", false);
  theLineClusterProvider = registerModule<LineClusterProvider>("LineClusterProvider");
  theLineClusterProvider->setEnabled(true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:execute", " ", false);
  theGoalDetector = registerModule<GoalDetector>("GoalDetector");
  theGoalDetector->setEnabled(true);


  // TODO: NeoVision stuff
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:execute", " ", true);
  theSimpleFieldColorClassifier = registerModule<SimpleFieldColorClassifier>("SimpleFieldColorClassifier");
  theSimpleFieldColorClassifier->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:ScanLineEdgelDetectorDifferential:execute", " ", true);
  theScanLineEdgelDetectorDifferential = registerModule<ScanLineEdgelDetectorDifferential>("ScanLineEdgelDetectorDifferential");
  theScanLineEdgelDetectorDifferential->setEnabled(true);

  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:execute", " ", true);
  theMaximumRedBallDetector = registerModule<MaximumRedBallDetector>("MaximumRedBallDetector");
  theMaximumRedBallDetector->setEnabled(true);
  
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:execute", " ", true);
  theGradientGoalDetector = registerModule<GradientGoalDetector>("GradientGoalDetector");
  theGradientGoalDetector->setEnabled(true);

}//end constructor


void ImageProcessor::execute()
{
  // TODO: NeoVision stuff
  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:execute",
    GT_TRACE("executing SimpleFieldColorClassifier");
    STOPWATCH_START("SimpleFieldColorClassifier");
    theSimpleFieldColorClassifier->execute();
    STOPWATCH_STOP("SimpleFieldColorClassifier");
  );

  DEBUG_REQUEST("ImageProcessor:HistogramFieldDetector:execute",
    GT_TRACE("executing HistogramFieldDetector");
    STOPWATCH_START("HistogramFieldDetector");
    theHistogramFieldDetector->execute();
    STOPWATCH_STOP("HistogramFieldDetector");
  );

  DEBUG_REQUEST("ImageProcessor:BodyContourProvider:execute",
    GT_TRACE("executing BodyContourProvider");
    STOPWATCH_START("BodyContourProvider");
    theBodyContourProvider->execute();
    STOPWATCH_STOP("BodyContourProvider");
  );

  DEBUG_REQUEST("ImageProcessor:RobotDetector:execute",
    GT_TRACE("executing RobotDetector");
    STOPWATCH_START("RobotDetector");
    theRobotDetector->execute();
    STOPWATCH_STOP("RobotDetector");
  );

  DEBUG_REQUEST("ImageProcessor:GoalDetector:execute",
    GT_TRACE("executing GoalDetector");
    STOPWATCH_START("GoalDetector");
    theGoalDetector->execute();
    STOPWATCH_STOP("GoalDetector");
  );

  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:execute",
    GT_TRACE("executing ScanLineEdgelDetector");
    STOPWATCH_START("ScanLineEdgelDetector");
    theScanLineEdgelDetector->execute();
    STOPWATCH_STOP("ScanLineEdgelDetector");
  );

  // TODO: NeoVision stuff
  DEBUG_REQUEST("NeoVision:ScanLineEdgelDetectorDifferential:execute",
    GT_TRACE("executing ScanLineEdgelDetectorDifferential");
    STOPWATCH_START("ScanLineEdgelDetectorDifferential");
    theScanLineEdgelDetectorDifferential->execute();
    STOPWATCH_STOP("ScanLineEdgelDetectorDifferential");
  );

  DEBUG_REQUEST("ImageProcessor:FieldDetector:execute",
    GT_TRACE("executing FieldDetector");
    STOPWATCH_START("FieldDetector");
    theFieldDetector->execute();
    STOPWATCH_STOP("FieldDetector");
  );

  DEBUG_REQUEST("ImageProcessor:BallDetector:execute",
    GT_TRACE("executing BallDetector");
    STOPWATCH_START("BallDetector");
    theBallDetector->execute();
    STOPWATCH_STOP("BallDetector");
  );

  DEBUG_REQUEST("ImageProcessor:LineDetector:execute",
    GT_TRACE("executing LineDetector");
    STOPWATCH_START("LineDetector");
    theLineDetector->execute();
    STOPWATCH_STOP("LineDetector");
  );

  DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:execute",
    GT_TRACE("executing MaximumRedBallDetector");
    STOPWATCH_START("MaximumRedBallDetector");
    theMaximumRedBallDetector->execute();
    STOPWATCH_STOP("MaximumRedBallDetector");
  );

  DEBUG_REQUEST("NeoVision:GradientGoalDetector:execute",
    GT_TRACE("executing GradientGoalDetector");
    STOPWATCH_START("GradientGoalDetector");
    theGradientGoalDetector->execute();
    STOPWATCH_STOP("GradientGoalDetector");
  );

  GT_TRACE("executing rest of ImageProcessor::execute()");




  //draw horizon to image
  DEBUG_REQUEST("ImageProcessor:draw_horizon",
    CANVAS_PX_TOP;
    Vector2d a(getArtificialHorizonTop().begin());
    Vector2d b(getArtificialHorizonTop().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("ImageProcessor:draw_horizon",

    CANVAS_PX_BOTTOM;
    Vector2d a(getArtificialHorizon().begin());
    Vector2d b(getArtificialHorizon().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("ImageProcessor:classify_ball_color",
    // color experiment
    CANVAS_PX_BOTTOM;
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
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
          POINT_PX(x, y, t, 0, 0);

      }//end for
    }//end for
  );
}//end execute

