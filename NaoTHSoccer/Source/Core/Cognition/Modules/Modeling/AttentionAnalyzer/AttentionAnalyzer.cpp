/**
* @file AttentionAnalyzer.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class AttentionAnalyzer
*/

#include "AttentionAnalyzer.h"

// Debug
#include "Tools/Debug/DebugDrawings.h"
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugBufferedOutput.h>

// Tools
#include "Tools/CameraGeometry.h"


AttentionAnalyzer::AttentionAnalyzer()
{
  // init the map
  createMapOfInterest();

  //createRadialMapOfInterest(mapOfInterest,250,1250,500);
  //createRadialMapOfInterest(closeMapOfInterest,250,600,100);


  // remember the time for the current interesting point
  currentPointOfInterestTimeStamp = getFrameInfo().getTime();
  // initialize the first element
  currentPointOfInterest = mapOfInterest.begin();


  DEBUG_REQUEST_REGISTER("AttentionAnalyzer:drawMapOfInterest", "Plot the map of interest on the field.", false);
  DEBUG_REQUEST_REGISTER("AttentionAnalyzer:plot_focus_trace", "Plots the trace of the point of the attention.", false);
}


void AttentionAnalyzer::execute()
{
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  odometryDelta.translation = Vector2<double>(); // only rotattion is intersting

  // TODO: make it parameter
  double agingFactor = 0.99; // factor per second


  // update the points
  for(ListOfInterest::iterator iter = mapOfInterest.begin(); iter != mapOfInterest.end(); ++iter)
  {
    // opdate by odometry
    iter->position = odometryDelta*iter->position;

    // update the weights
    // TODO: make it parameter
    if( distanceToImageCenter(iter->position) < 80 )
    {
      iter->weight = 1.0;
    }
    else
    {
      iter->weight = iter->weight*agingFactor;
    }
  }//end for


  //HACK: if the point was seen or is too old
  if(getFrameInfo().getTimeSince(currentPointOfInterestTimeStamp) > 1000)
  {
    (*currentPointOfInterest).weight = 1.0;
  }

  
  // get the next most interesting point
  for(ListOfInterest::iterator iter = mapOfInterest.begin(); iter != mapOfInterest.end(); ++iter)
  {
    if(( compare(*currentPointOfInterest, *iter) < 0 && inView(*iter))
       || !inView(*currentPointOfInterest))
    {
      currentPointOfInterest = iter;
      currentPointOfInterestTimeStamp = getFrameInfo().getTime();
    }
  }//end for


  // update the model
  getAttentionModel().mostInterestingPoint = (*currentPointOfInterest).position;
  

  DEBUG_REQUEST("AttentionAnalyzer:drawMapOfInterest",
    drawMapOfInterest(mapOfInterest);
    drawImageProjection();
  );

  DEBUG_REQUEST("AttentionAnalyzer:plot_focus_trace",
    PEN("000000", 10);
    OVAL((*currentPointOfInterest).position.x, (*currentPointOfInterest).position.y, 30, 30);

    Vector2<double> centerOnField;
    if(CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), 
      getCameraInfo(),
      (double)getCameraInfo().getOpticalCenterX(),
      (double)getCameraInfo().getOpticalCenterY(),
      0.0,
      centerOnField))
    {
      FILLOVAL(centerOnField.x, centerOnField.y, 30, 30);

      ORIGIN2D("AttentionAnalyzer look_at_trace",0,0,getOdometryData().rotation);
      PLOT2D("AttentionAnalyzer look_at_trace",centerOnField.x,centerOnField.y);
    }
  );

  lastFrameInfo = getFrameInfo();
  lastRobotOdometry = getOdometryData();
}//end execute

bool AttentionAnalyzer::inView(const PointOfInterest& point)
{
  return fabs(point.position.angle()) < Math::pi*0.5;
}//end inView

// return 0 if one==two, or < 0 if one < two
int AttentionAnalyzer::compare(const PointOfInterest& one, const PointOfInterest& two)
{
  // update by distance to field of view
  Vector2<double> centerOnField;
  if(!CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), 
      getCameraInfo(),
      (double)getCameraInfo().getOpticalCenterX(),
      (double)getCameraInfo().getOpticalCenterY(),
      0.0,
      centerOnField))
      return 0;

  double oneDistanceToCenterOfView = (centerOnField-one.position).abs();
  double twoDistanceToCenterOfView = (centerOnField-two.position).abs();

  // point in the field of view
  if(fabs(one.position.angle()) < Math::pi)
  {
    //...
  }

  return (int)(twoDistanceToCenterOfView*(1.0-one.weight) - oneDistanceToCenterOfView*(1.0-two.weight));
}//end compare

double AttentionAnalyzer::distanceToImageCenter(const Vector2<double>& point)
{
  // distance from center to a corner of the image
  // "+1" is for practiacl reasons (see below)
  static const double maxDistanceToCenter = 
    Vector2<double>(getCameraInfo().getOpticalCenterX()+1, 
                    getCameraInfo().getOpticalCenterY()+1).abs();


  // Note: projectionInImage = (0,0) if no projection is possible.
  // We don't need to treat it extra, cince in this case the resulting distance is maximal.
  Vector2<int> projectionInImage;
  CameraGeometry::relativePointToImage(
    getCameraMatrix(),
    getCameraInfo(),
    Vector3<double>(point.x, point.y, 0.0),
    projectionInImage
  );

  Vector2<double> pointToCenter(
    projectionInImage.x-getCameraInfo().getOpticalCenterX(),
    projectionInImage.y-getCameraInfo().getOpticalCenterY());

  return std::min(pointToCenter.abs(), maxDistanceToCenter);
}//end distanceToImageCenter


void AttentionAnalyzer::createRadialMapOfInterest(
  std::list<PointOfInterest>& moi,
  double minDistance,
  double maxDistance,
  double distance_step
  )
{
  
  //double minDistance = 250;
  //double maxDistance = 600;
  //double distance_step = 100;

  int shift = 0;
  double alpha_step = Math::pi2/12;

  for(double d = minDistance; d <= maxDistance; d += distance_step)
  {
    shift = 1 - shift;

    for(double alpha = -Math::pi; alpha < Math::pi; alpha += alpha_step)
    {
      PointOfInterest tmp;
      tmp.position.x = d;
      tmp.position.rotate(alpha + 0.5*alpha_step*shift);

      moi.push_back(tmp);
    }//end for
  }//end for
}//end createMapOfInterest


void AttentionAnalyzer::createMapOfInterest()
{
  double distanceSteps = 3.0;
  double maxDistance = 1200.0;

  for(double x = -distanceSteps; x <= distanceSteps; x++)
  {
    for(double y = -distanceSteps; y <= distanceSteps; y++)
    {
      if(y == 0 && x == 0) continue;

      PointOfInterest tmp;
      tmp.position.x = x/distanceSteps*maxDistance;
      tmp.position.y = y/distanceSteps*maxDistance;

      //if(tmp.position.abs()-1.0 > maxDistance) continue;

      mapOfInterest.push_back(tmp);
    }//end for
  }//end for

  mapOfInterest.push_back(PointOfInterest(Vector2<double>(0.0, -200.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(0.0, 200.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(200.0, 0.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(-200.0, 0.0)));

  mapOfInterest.push_back(PointOfInterest(Vector2<double>(200.0, 200.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(200.0, -200.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(-200.0, 200.0)));
  mapOfInterest.push_back(PointOfInterest(Vector2<double>(-200.0, -200.0)));
}//end createMapOfInterest


void AttentionAnalyzer::drawMapOfInterest(const std::list<PointOfInterest>& moi) const
{
  FIELD_DRAWING_CONTEXT;

  std::list<PointOfInterest>::const_iterator iter;
  for(iter = moi.begin(); iter != moi.end(); ++iter)
  {
    PEN(DebugDrawings::Color(1.0,0.0,0.0,iter->weight), 10);

    //double step = 6050.0/12.0/2-5;
    double radius = iter->position.abs()*0.1;
    FILLOVAL(iter->position.x, iter->position.y, radius, radius);


    PEN("000000", 5);
    OVAL(iter->position.x, iter->position.y, radius, radius);

    /*
    FILLBOX(iter->position.x-step, iter->position.y-step,
            iter->position.x+step, iter->position.y+step);
    */
  }//end for
}//end drawMapOfInterest

//TODO: handle the case if the projection is not possible
void AttentionAnalyzer::drawImageProjection()
{
  PEN("000000", 10);
  Vector2<double> lu;
  CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), 
        getCameraInfo(),
        0.0, 
        0.0, 
        0.0,
        lu);

  Vector2<double> ru;
  CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), 
        getCameraInfo(),
        (double)getCameraInfo().resolutionWidth, 
        0.0, 
        0.0,
        ru);

  Vector2<double> rb;
  CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), 
        getCameraInfo(),
        (double)getCameraInfo().resolutionWidth, 
        (double)getCameraInfo().resolutionHeight, 
        0.0,
        rb);

  Vector2<double> lb;
  CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(), 
        getCameraInfo(),
        0.0, 
        (double)getCameraInfo().resolutionHeight, 
        0.0,
        lb);

  LINE(lu.x, lu.y, ru.x, ru.y);
  LINE(ru.x, ru.y, rb.x, rb.y);
  LINE(rb.x, rb.y, lb.x, lb.y);
  LINE(lb.x, lb.y, lu.x, lu.y);
}//end drawImageProjection

