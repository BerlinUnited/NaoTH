/**
* @file PerceptionsVisualizer.cpp
*
* @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
* Implementation of class PerceptionsVizualization
*/

#include "PerceptionsVisualizer.h"

using namespace std;

PerceptionsVisualizer::PerceptionsVisualizer()
  : cameraID(CameraInfo::numOfCamera)
{
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image:draw_horizon", "draw the hizon line in the image", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:CamTop", "execute for the top cam", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:CamBottom", "execute for the bottom cam", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:ball_percept", "draw ball percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image:ball_percept", "draw ball percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image_px:ball_percept", "draw ball percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:goal_percept", "draw goal percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image_px:goal_percept", "draw goal percept", false);


  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:edgels_percept", "draw edgels percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image_px:edgels_percept", "draw edgels percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:line_percept_2018", "draw new line percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:line_percept", "draw line percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image:line_percept", "draw line percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image_px:line_percept", "draw line percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:ransac_circle_percept", "draw circle provided by RansacLineDetector", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:players_percept", "draw players percept", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:image_px:players_percept", "draw players percept", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:corrrect_camera_matrix","", false);

  DEBUG_REQUEST_REGISTER( "PerceptionsVisualizer:image:draw_field_polygon", "draw field boundaries in image", false);

  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:field_info", "", false);
  DEBUG_REQUEST_REGISTER("PerceptionsVisualizer:field:robot_pose", "", false);
}

void PerceptionsVisualizer::execute()
{
  DEBUG_REQUEST("PerceptionsVisualizer:CamTop",
    execute(CameraInfo::Top);
  );
  DEBUG_REQUEST("PerceptionsVisualizer:CamBottom",
    execute(CameraInfo::Bottom);
  );

  DEBUG_REQUEST("PerceptionsVisualizer:field:field_info",
    FIELD_DRAWING_CONTEXT;
    getFieldInfo().draw(getDebugDrawings());
  );

  DEBUG_REQUEST("PerceptionsVisualizer:field:robot_pose",
    FIELD_DRAWING_CONTEXT;
    getRobotPose().draw(getDebugDrawings());
  );
}

void PerceptionsVisualizer::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  DEBUG_REQUEST("PerceptionsVisualizer:image:draw_horizon",
    Vector2d a(getArtificialHorizon().begin());
    Vector2d b(getArtificialHorizon().end());
    LINE_PX( ColorClasses::red, (int)a.x, (int)a.y, (int)b.x, (int)b.y );
  );

  //draw ball percept
  DEBUG_REQUEST("PerceptionsVisualizer:field:ball_percept",
    if(getBallPercept().ballWasSeen) 
    {
      FIELD_DRAWING_CONTEXT;
      PEN("FF9900", 20);
      CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, 
        getBallPercept().bearingBasedOffsetOnField.y,
        getFieldInfo().ballRadius);
    }

    FIELD_DRAWING_CONTEXT;
    PEN("FF9900", 20);
    for (MultiBallPercept::ConstABPIterator i = getMultiBallPercept().begin(); i != getMultiBallPercept().end(); ++i) {
      if ((*i).cameraId == cameraID) {
        CIRCLE((*i).positionOnField.x,
          (*i).positionOnField.y,
          getFieldInfo().ballRadius);
      }
    }
 );

  DEBUG_REQUEST("PerceptionsVisualizer:image:ball_percept",
    if(getBallPercept().ballWasSeen) 
    {
      IMAGE_DRAWING_CONTEXT;
      PEN("FF9900", 2);
      CIRCLE(
        (int)getBallPercept().centerInImage.x,
        (int)getBallPercept().centerInImage.y,
        (int)getBallPercept().radiusInImage);
    }

	  for (MultiBallPercept::ConstABPIterator i = getMultiBallPercept().begin(); i != getMultiBallPercept().end(); ++i) {
		  CANVAS((((*i).cameraId == CameraInfo::Top) ? "ImageTop" : "ImageBottom"));
		  PEN("FF9900", 3);
		  CIRCLE(
			  (*i).centerInImage.x,
			  (*i).centerInImage.y,
			  (*i).radiusInImage);
	  }
  );

  

  DEBUG_REQUEST("PerceptionsVisualizer:image_px:ball_percept",
    if(getBallPercept().ballWasSeen) 
    {
      CIRCLE_PX(ColorClasses::red,
        (int)getBallPercept().centerInImage.x,
        (int)getBallPercept().centerInImage.y,
        (int)getBallPercept().radiusInImage);
    }
  );


  DEBUG_REQUEST("PerceptionsVisualizer:field:goal_percept",
    FIELD_DRAWING_CONTEXT;
    for(int n = 0; n < getGoalPercept().getNumberOfSeenPosts(); n++)
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

  DEBUG_REQUEST("PerceptionsVisualizer:image_px:goal_percept",

  );


  DEBUG_REQUEST("PerceptionsVisualizer:field:edgels_percept",
    FIELD_DRAWING_CONTEXT;

    CameraMatrix cameraMatrix(getCameraMatrix());
    DEBUG_REQUEST("PerceptionsVisualizer:field:corrrect_camera_matrix",
      cameraMatrix.rotateY(getCameraMatrixOffset().offset.y)
                  .rotateX(getCameraMatrixOffset().offset.x);
    );

    for(size_t i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
      // no projection is avaliable for this point
      if(!point.valid && point.posOnField.x == 0 && point.posOnField.y == 0) continue;

      if(point.posInImage.y < 10) { // close to the top
        PEN("009900", 20);
      } else {
        PEN(ColorClasses::colorClassToHex(point.color), 20);
      }

      CIRCLE(point.posOnField.x, point.posOnField.y, 10);
      if(i > 0)
      {
        const ScanLineEdgelPercept::EndPoint& last_point = getScanLineEdgelPercept().endPoints[i-1];
        LINE(last_point.posOnField.x, last_point.posOnField.y, point.posOnField.x, point.posOnField.y);
      }
    }//end for


    
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      const Edgel& edgelBegin = getScanLineEdgelPercept().edgels[pair.begin];
      const Edgel& edgelEnd = getScanLineEdgelPercept().edgels[pair.end];
    
      Vector2d edgelOnFieldDirectionBegin;
      Vector2d edgelOnFieldBegin;
      if(CameraGeometry::imagePixelToFieldCoord(
        cameraMatrix,
        getCameraInfo(),
        edgelBegin.point.x,
        edgelBegin.point.y,
        0.0,
        edgelOnFieldBegin))
      {
        Vector2d direction = Vector2d(edgelBegin.point) + edgelBegin.direction;
        
        if(CameraGeometry::imagePixelToFieldCoord(
          cameraMatrix,
          getCameraInfo(),
          direction.x,
          direction.y,
          0.0,
          edgelOnFieldDirectionBegin))
        {
          PEN("000000", 5);
          //CIRCLE(edgelOnFieldBegin.x, edgelOnFieldBegin.y, 10);
          double r = (edgelOnFieldDirectionBegin - edgelOnFieldBegin).angle();
          SIMPLE_PARTICLE(edgelOnFieldBegin.x,edgelOnFieldBegin.y,r);
        }
      }

      Vector2d edgelOnFieldDirectionEnd;
      Vector2d edgelOnFieldEnd;
      if(CameraGeometry::imagePixelToFieldCoord(
        cameraMatrix,
        getCameraInfo(),
        edgelEnd.point.x,
        edgelEnd.point.y,
        0.0,
        edgelOnFieldEnd))
      {
        Vector2d direction = Vector2d(edgelEnd.point) + edgelEnd.direction;

        if(CameraGeometry::imagePixelToFieldCoord(
          cameraMatrix,
          getCameraInfo(),
          direction.x,
          direction.y,
          0.0,
          edgelOnFieldDirectionEnd))
        {
          PEN("FF0000", 5);
          //CIRCLE(edgelOnFieldEnd.x, edgelOnFieldEnd.y, 10);
          double r = (edgelOnFieldDirectionEnd - edgelOnFieldEnd).angle();
          SIMPLE_PARTICLE(edgelOnFieldEnd.x,edgelOnFieldEnd.y,r);
        }
      }
    }//end for
  );

  DEBUG_REQUEST("PerceptionsVisualizer:image_px:edgels_percept",
    /*FIELD_DRAWING_CONTEXT;

    for(size_t i = 0; i < getScanLineEdgelPercept().scanLineEdgels.size(); i++)
    {
      const DoubleEdgel& e = getScanLineEdgelPercept().scanLineEdgels[i];

      Vector2d begin_dir(10.0,0.0);
      begin_dir.rotate(e.begin_angle);
      LINE_PX(ColorClasses::red, e.begin.x, e.begin.y, e.begin.x + (int)(begin_dir.x+0.5), e.begin.y + (int)(begin_dir.y+0.5));

      Vector2d end_dir(10.0,0.0);
      end_dir.rotate(e.end_angle);
      LINE_PX(ColorClasses::red, e.end.x, e.end.y, e.end.x + (int)(end_dir.x+0.5), e.end.y + (int)(end_dir.y+0.5));
    }*///end for
  );


  //draw line percept
  DEBUG_REQUEST("PerceptionsVisualizer:field:line_percept",
    FIELD_DRAWING_CONTEXT;
    PEN("666666AA", 50);

    for (size_t i = 0; i < getLinePercept().lines.size(); i++)
    {
      const LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

      // draw the circle lines with a different color
      if(linePercept.type == LinePercept::C) {
        PEN("AAAAAAAA", 50);
      } else {
        PEN("666666AA", 50);
      }

      LINE(linePercept.lineOnField.begin().x,
           linePercept.lineOnField.begin().y,
           linePercept.lineOnField.end().x,
           linePercept.lineOnField.end().y);
    }//end for

    for(size_t i=0; i < getLinePercept().intersections.size(); i++)
    {
      //mark intersection on the field
      LineIntersection::Type type = getLinePercept().intersections[i].getType();
      const Vector2d& intersectionPoint = getLinePercept().intersections[i].getPos();

      PEN(ColorClasses::colorClassToHex((ColorClasses::Color) type), 3); 
      CIRCLE(intersectionPoint.x, intersectionPoint.y, 50);
      TEXT_DRAWING(intersectionPoint.x + 100, intersectionPoint.y + 100, LineIntersection::getTypeName(type));
    }

    // circle
    if(getLinePercept().middleCircleWasSeen)
    {
      const Vector2d& center = getLinePercept().middleCircleCenter;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);

      if(getLinePercept().middleCircleOrientationWasSeen)
      {
        const Vector2d direction = getLinePercept().middleCircleOrientation*(getFieldInfo().centerCircleRadius+100);
        LINE(
          center.x + direction.x,
          center.y + direction.y,
          center.x - direction.x,
          center.y - direction.y
          );
      }//end if
    }//end if
  ); // end line_percept on field


  DEBUG_REQUEST("PerceptionsVisualizer:image:line_percept",
    IMAGE_DRAWING_CONTEXT;
    // mark lines
    for (size_t i = 0; i < getLinePercept().lines.size(); i++)
    {
      const LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

      PEN("009900", 2);
      LINE(linePercept.lineInImage.segment.begin().x, 
           linePercept.lineInImage.segment.begin().y,
           linePercept.lineInImage.segment.end().x, 
           linePercept.lineInImage.segment.end().y);
      
      // indicate line count
      LINE((2*i)+1,7,(2*i)+1,12);
    }//end for

    for(size_t i=0; i < getLinePercept().intersections.size(); i++)
    {
      //mark intersection on the field
      LineIntersection::Type type = getLinePercept().intersections[i].getType();
      const Vector2d& intersectionPoint = getLinePercept().intersections[i].getPos();

      PEN(ColorClasses::colorClassToHex((ColorClasses::Color) type), 3); 
      CIRCLE(intersectionPoint.x, intersectionPoint.y, 50);
      TEXT_DRAWING(intersectionPoint.x + 100, intersectionPoint.y + 100, LineIntersection::getTypeName(type));
    }
    
  ); // end line_percept in image



  DEBUG_REQUEST("PerceptionsVisualizer:image_px:line_percept",
    // mark lines
    for (size_t i = 0; i < getLinePercept().lines.size(); i++)
    {
      const LinePercept::FieldLineSegment& linePercept = getLinePercept().lines[i];

      Vector2d d(0.0, ceil(linePercept.lineInImage.thickness / 2.0));
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
    }//end for
  ); // end line_percept in image_px

  /*
  // FIXME: deprecated
  DEBUG_REQUEST("PerceptionsVisualizer:field:ransac_circle_percept",
    if(getRansacCirclePercept().middleCircleWasSeen) {
      FIELD_DRAWING_CONTEXT;
      PEN("009900", 10);
      CIRCLE(getRansacCirclePercept().middleCircleCenter.x,
             getRansacCirclePercept().middleCircleCenter.y,
             getFieldInfo().centerCircleRadius);
    }
  );
  */

  DEBUG_REQUEST("PerceptionsVisualizer:field:players_percept",
    FIELD_DRAWING_CONTEXT;
    getPlayersPercept().draw(getDebugDrawings());
  );

  DEBUG_REQUEST("PerceptionsVisualizer:image_px:players_percept",
  );


  DEBUG_REQUEST("PerceptionsVisualizer:image:draw_field_polygon",
    ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::green : ColorClasses::red;
    const FieldPercept::FieldPoly& fieldpoly = getFieldPercept().getValidField();
    for(size_t i = 0; i + 1 < fieldpoly.size(); i++) {
      LINE_PX(color, fieldpoly[i].x, fieldpoly[i].y, fieldpoly[i+1].x, fieldpoly[i+1].y);
    }
  );

  // draw new line percept
  DEBUG_REQUEST("PerceptionsVisualizer:field:line_percept_2018",
    FIELD_DRAWING_CONTEXT;
    PEN("AA666666", 50);

    // draw ransac line percept
    PEN("AA666666", 50);
    for (size_t i = 0; i < getRansacLinePercept().fieldLineSegments.size(); i++)
    {
      const Math::LineSegment line = getRansacLinePercept().fieldLineSegments[i];

      LINE(line.begin().x,
           line.begin().y,
           line.end().x,
           line.end().y);
    }

    // draw short line percept (RansacLineDetectorOnGraphs)
    PEN("66AA6666", 50);
    for (size_t i = 0; i < getShortLinePercept().fieldLineSegments.size(); i++)
    {
      const Math::LineSegment line = getShortLinePercept().fieldLineSegments[i];

      LINE(line.begin().x,
           line.begin().y,
           line.end().x,
           line.end().y);
    }

    // draw ransac circle
    if(getRansacCirclePercept2018().wasSeen)
    {
      const Vector2d& center = getRansacCirclePercept2018().center;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);
    }
  );

}//end execute
