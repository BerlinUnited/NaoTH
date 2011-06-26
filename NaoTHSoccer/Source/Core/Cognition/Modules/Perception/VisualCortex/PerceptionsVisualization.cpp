/**
* @file PerceptionsVisualization.cpp
*
* @author Kirill Yasinovskiy <yasinovs@informatik.hu-berlin.de>
* Implementation of class PerceptionsVizualization
*/

#include "PerceptionsVisualization.h"

//include debugs
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

PerceptionsVisualization::PerceptionsVisualization()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:image_drawing_context", "draw perceptions direct in image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:field_drawing_context", "draw perceptions direct in field", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:draw_line_percept", "draw line percept", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:draw_edgels_percept", "draw edgels percept", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:draw_ball_percept", "draw ball percept", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:draw_players_percept", "draw players percept", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:PerceptionsVisualization:draw_goal_percept", "draw goal percept", false);
}

void PerceptionsVisualization::execute()
{
  //do we want to use an image or/and a field context?
  bool image = false;
  DEBUG_REQUEST("ImageProcessor:PerceptionsVisualization:image_drawing_context",
    image = true;
  );
  bool field = false;
  DEBUG_REQUEST("ImageProcessor:PerceptionsVisualization:field_drawing_context",
    field = true;
  );
  //draw ball percept
  DEBUG_REQUEST("ImageProcessor:PerceptionsVisualization:draw_ball_percept",
    if (field)
    {
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, 
        getBallPercept().bearingBasedOffsetOnField.y,
        45);
    }
    if (image)
    {
      CIRCLE_PX(ColorClasses::red, (int)getBallPercept().centerInImage.x, (int)getBallPercept().centerInImage.y, (int)getBallPercept().radiusInImage);
    }
  );
  //draw line percept
  DEBUG_REQUEST("ImageProcessor:PerceptionsVisualization:draw_line_percept",
    if (field)
    {
    }
    if (image)
    {
      //mark lines
      for (unsigned int i = 0; i < getLinePercept().lines.size(); i++)
      {
        if (!getLinePercept().lines[i].valid) continue;
        LinePercept::FieldLineSegment line = getLinePercept().lines[i];
        Vector2<double> d(0.0, ceil(line.thickness / 2.0));
        Vector2<int> lowerLeft(getLinePercept().lines[i].begin() - d);
        Vector2<int> upperLeft(getLinePercept().lines[i].begin() + d);
        Vector2<int> lowerRight(getLinePercept().lines[i].end() - d);
        Vector2<int> upperRight(getLinePercept().lines[i].end() + d);
        LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y);
        LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, upperLeft.x, upperLeft.y);
        LINE_PX(ColorClasses::green, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y);
        LINE_PX(ColorClasses::green, lowerRight.x, lowerRight.y, upperRight.x, upperRight.y);
        LINE_PX(ColorClasses::green,(2*i)+1,7,(2*i)+1,12);
      }//end for
    }
  );
}
