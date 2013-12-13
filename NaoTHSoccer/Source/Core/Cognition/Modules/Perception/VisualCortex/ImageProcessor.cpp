/**
* @file ImageProcessor.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageProcessor
*/

#include "ImageProcessor.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"

ImageProcessor::ImageProcessor()
{
  DEBUG_REQUEST_REGISTER("Vision:draw_horizon", "draws the artificial horizon", false);

  DEBUG_REQUEST_REGISTER("Vision:draw_ball_on_field", "draw the projection of the ball on the field", false);
  DEBUG_REQUEST_REGISTER("Vision:draw_ball_in_image", "draw ball in the image if found", false);

  DEBUG_REQUEST_REGISTER("Vision:ballpos_relative_3d", "draw the estimated ball position relative to the camera in 3d viewer", false);
  DEBUG_REQUEST_REGISTER("Vision:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:classify_ball_color", "", false);
}//end constructor


void ImageProcessor::execute()
{
  GT_TRACE("executing rest of Vision::execute()");

  //draw horizon to image
  DEBUG_REQUEST("Vision:draw_horizon",
    CANVAS_PX_TOP;
    Vector2d a(getArtificialHorizonTop().begin());
    Vector2d b(getArtificialHorizonTop().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("Vision:draw_horizon",
    CANVAS_PX_BOTTOM;
    Vector2d a(getArtificialHorizon().begin());
    Vector2d b(getArtificialHorizon().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  DEBUG_REQUEST("Vision:classify_ball_color",
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

