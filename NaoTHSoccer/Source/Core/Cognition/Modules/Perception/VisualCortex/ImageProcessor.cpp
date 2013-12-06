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

  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, HistogramFieldDetector, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, BodyContourProvider, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, ScanLineEdgelDetector, " ", false);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, FieldDetector, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, BallDetector, " ", false);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, RobotDetector, " ", false);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, LineDetector, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, LineClusterProvider, " ", false);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, GoalDetector, " ", false);

  // TODO: NeoVision stuff
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, SimpleFieldColorClassifier, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, ScanLineEdgelDetectorDifferential, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, GradientGoalDetector, " ", true);
  DEBUG_REQUEST_REGISTER_SUBMODULE(ImageProcessor, MaximumRedBallDetector, " ", true);

}//end constructor


void ImageProcessor::execute()
{
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, SimpleFieldColorClassifier);
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, HistogramFieldDetector);
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, FieldDetector);
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, BodyContourProvider);

  //robot detector using waist band color
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, RobotDetector);

  //color based goal detector
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, GoalDetector);
  //Gradient based gola detector (default)
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, GradientGoalDetector);

  //Edgel detector using threshold and color
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, ScanLineEdgelDetector);
  //Edgel detector using differences
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, ScanLineEdgelDetectorDifferential);

  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, LineDetector);
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, LineClusterProvider);

  //ball detector based on color
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, BallDetector);
  //ball detector based on gradients and red color peaks search inside field region (needs known field)
  DEBUG_EXECUTE_SUBMODULE(ImageProcessor, MaximumRedBallDetector);

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

