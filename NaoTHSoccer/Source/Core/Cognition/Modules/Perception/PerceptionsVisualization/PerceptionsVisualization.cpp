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
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:field:ball_percept", "draw ball percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:image:ball_percept", "draw ball percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:field:goal_percept", "draw goal percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:image:goal_percept", "draw goal percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:field:edgels_percept", "draw edgels percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:image:edgels_percept", "draw edgels percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:field:line_percept", "draw line percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:image:line_percept", "draw line percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:field:players_percept", "draw players percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualization:image:players_percept", "draw players percept", false);
}

void PerceptionsVisualization::execute()
{
  //draw ball percept
  DEBUG_REQUEST("PerceptionsVisualization:field:ball_percept",
    FIELD_DRAWING_CONTEXT;
    PEN("FF9900", 20);
    CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, 
      getBallPercept().bearingBasedOffsetOnField.y,
      getFieldInfo().ballRadius);
  );

  DEBUG_REQUEST("PerceptionsVisualization:image:ball_percept",
    CIRCLE_PX(ColorClasses::red, 
      (int)getBallPercept().centerInImage.x, 
      (int)getBallPercept().centerInImage.y, 
      (int)getBallPercept().radiusInImage);
  );


  DEBUG_REQUEST("PerceptionsVisualization:field:goal_percept",
    FIELD_DRAWING_CONTEXT;
    for(unsigned int n = 0; n < getGoalPercept().getNumberOfSeenPosts(); n++)
    {
      const GoalPercept::GoalPost& post = getGoalPercept().getPost(n);
      
      if(post.color==ColorClasses::skyblue && post.positionReliable)
        PEN("8888FF", 20);
      else if(post.color==ColorClasses::yellow && post.positionReliable)
        PEN("FFFF00", 20);
      else
        PEN("FF0000", 20);

      CIRCLE(post.position.x,
             post.position.y,
             getFieldInfo().goalpostRadius);
    }//end for
  );

  DEBUG_REQUEST("PerceptionsVisualization:image:goal_percept",

  );


  DEBUG_REQUEST("PerceptionsVisualization:field:edgels_percept",
  );


  DEBUG_REQUEST("PerceptionsVisualization:image:edgels_percept",
  );


  //draw line percept
  DEBUG_REQUEST("PerceptionsVisualization:field:line_percept",
    FIELD_DRAWING_CONTEXT;
    PEN("666666AA", 50);

    for (unsigned int i = 0; i < getLinePercept().lines.size(); i++)
    {
      const LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

      // draw the circle lines with a different color
      if(linePercept.type == LinePercept::C)
        PEN("AAAAAAAA", 50);
      else
        PEN("666666AA", 50);

      LINE(linePercept.lineOnField.begin().x,
           linePercept.lineOnField.begin().y,
           linePercept.lineOnField.end().x,
           linePercept.lineOnField.end().y);
    }//end for

    for(unsigned int i=0; i < getLinePercept().intersections.size(); i++)
    {
      //mark intersection on the field
      PEN(ColorClasses::colorClassToHex((ColorClasses::Color) getLinePercept().intersections[i].getType()), 3); 
      const Vector2<double>& intersectionPoint = getLinePercept().intersections[i].getPosOnField();
      CIRCLE(intersectionPoint.x, intersectionPoint.y, 50);

      string type = "N";
      switch(getLinePercept().intersections[i].getType())
      {
        case Math::Intersection::C: type="C"; break;
        case Math::Intersection::T: type="T"; break;
        case Math::Intersection::L: type="L"; break;
        case Math::Intersection::E: type="E"; break;
        case Math::Intersection::X: type="X"; break;
        default: break;
      }
      TEXT_DRAWING(intersectionPoint.x + 100, intersectionPoint.y + 100, type);
    }//end for

    // circle
    const Vector2<double>& center = getLinePercept().middleCircleCenter;
    PEN("FFFFFF99", 10);
    CIRCLE(center.x, center.y, 50);
    PEN("FFFFFF99", 50);
    CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);

    if(getLinePercept().middleCircleOrientationWasSeen)
    {
      const Vector2<double> direction = getLinePercept().middleCircleOrientation*(getFieldInfo().centerCircleRadius+100);
      LINE(
        center.x + direction.x,
        center.y + direction.y,
        center.x - direction.x,
        center.y - direction.y
        );
    }//end if
  ); // end line_percept on field


  DEBUG_REQUEST("PerceptionsVisualization:image:line_percept",
    IMAGE_DRAWING_CONTEXT;
    // mark lines
    for (unsigned int i = 0; i < getLinePercept().lines.size(); i++)
    {
      const LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

      Vector2<double> d(0.0, ceil(linePercept.lineInImage.thickness / 2.0));
      //d.rotate(Math::pi_2 - line.angle);

      Vector2<int> lowerLeft(linePercept.lineInImage.segment.begin() - d);
      Vector2<int> upperLeft(linePercept.lineInImage.segment.begin() + d);
      Vector2<int> lowerRight(linePercept.lineInImage.segment.end() - d);
      Vector2<int> upperRight(linePercept.lineInImage.segment.end() + d);
      LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y);
      LINE_PX(ColorClasses::green, lowerLeft.x, lowerLeft.y, upperLeft.x, upperLeft.y);
      LINE_PX(ColorClasses::green, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y);
      LINE_PX(ColorClasses::green, lowerRight.x, lowerRight.y, upperRight.x, upperRight.y);

      LINE_PX(ColorClasses::green,(2*i)+1,7,(2*i)+1,12);

      PEN("009900", 1);
      LINE(lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y);
      LINE(lowerLeft.x, lowerLeft.y, upperLeft.x, upperLeft.y);
      LINE(upperLeft.x, upperLeft.y, upperRight.x, upperRight.y);
      LINE(lowerRight.x, lowerRight.y, upperRight.x, upperRight.y);
    }//end for

    for(unsigned int i=0; i < getLinePercept().intersections.size(); i++)
    {
      //mark intersection on the field
      ColorClasses::Color color = (ColorClasses::Color) getLinePercept().intersections[i].getType();
      const Vector2<double>& intersectionPoint = getLinePercept().intersections[i].getPos();
      CIRCLE_PX(color, (int)(intersectionPoint.x+0.5), (int)(intersectionPoint.y+0.5), 5);


      PEN(ColorClasses::colorClassToHex(color), 1); 
      CIRCLE(intersectionPoint.x, intersectionPoint.y, 5);

      string type = "N";
      switch(getLinePercept().intersections[i].getType())
      {
        case Math::Intersection::C: type="C"; break;
        case Math::Intersection::T: type="T"; break;
        case Math::Intersection::L: type="L"; break;
        case Math::Intersection::E: type="E"; break;
        case Math::Intersection::X: type="X"; break;
        default: break;
      }

      PEN(ColorClasses::colorClassToHex(color), 0.1); 
      TEXT_DRAWING(intersectionPoint.x + 10, intersectionPoint.y + 10, type);
    }//end for
  ); // end line_percept in image



  DEBUG_REQUEST("PerceptionsVisualization:field:players_percept",
    getPlayersPercept().draw();
  );

  DEBUG_REQUEST("PerceptionsVisualization:image:players_percept",
  );

}//end execute
