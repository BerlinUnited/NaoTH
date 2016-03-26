
/**
* @file BallCandidateDetectorBW.cpp
*
* Implementation of class BallCandidateDetectorBW
*
*/

#include "BallCandidateDetectorBW.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>

#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

#include <list>

BallCandidateDetectorBW::BallCandidateDetectorBW()
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawPercepts", "draw ball percepts", false);
}

BallCandidateDetectorBW::~BallCandidateDetectorBW()
{
}

void BallCandidateDetectorBW::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  // todo: check validity of the intergral image
  if(getGameColorIntegralImage().getWidth() == 0) {
    return;
  }


  best.clear();
  calculateCandidates(best);

  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
    for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );
  

  /*
  for(point.y = 0; point.y < (int)getImage().height()/4; point.y+=1) 
  {
    int r = radiusEstimation[point.y*4]*2/4;
    int so = (int)(r*2.0*0.3/4.0+0.5);

    if (point.y - so < 0 || point.y+r+so+1 > 480/4) {
      continue;
    }

    for(point.x = so; point.x < (int)getImage().width()/4 - r - so; point.x+=1) 
    {

      int v  = integralImage[point.x+r][point.y+r]      +integralImage[point.x][point.y]      -integralImage[point.x][point.y+r]      -integralImage[point.x+r][point.y];
      int vo = integralImage[point.x+r+so][point.y+r+so]+integralImage[point.x-so][point.y-so]-integralImage[point.x-so][point.y+r+so]-integralImage[point.x+r+so][point.y-so];
      int vx = v - (vo - v);

      int vi = (int)(((double)vx)/((double)valueMax)*255);
      getDebugImageDrawings().drawPointToImage(v,0,0,point.x*4,point.y*4);
    }
  }
  */
  /*
  if(radius > 0) {
    CIRCLE_PX(ColorClasses::orange, center.x, center.y, radius);
  }
  */


  std::list<Best::BallCandidate>::iterator best_element = best.candidates.begin();
  int best_radius = -1;
  double maxV = 0;
  
  for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center)) 
    {
      int radius = (int)((*i).radius*1.5 + 0.5);
      
      subsampling((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);

      double v = isBall();
      if(best_radius < 0 || maxV < v) {
        maxV = v;
        best_element = i;
        best_radius = radius;
      }

      if(v >= 0.5) {
        addBallPercept((*i).center, radius);
      }

      DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
        //CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        if(v >= 0.5) {
          RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius,
            (*i).center.x + radius, (*i).center.y + radius);
        } else {
          RECT_PX(ColorClasses::gray, (*i).center.x - radius, (*i).center.y - radius,
            (*i).center.x + radius, (*i).center.y + radius);
        }
      );
    }
  }


  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
    if(best_element != best.candidates.end()) { 
      RECT_PX(ColorClasses::red, (*best_element).center.x - best_radius, (*best_element).center.y - best_radius,
        (*best_element).center.x + best_radius, (*best_element).center.y + best_radius);
    } 
  );

  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );
}

void BallCandidateDetectorBW::calculateCandidates(Best& best) const
{
  // todo needs a better place
  const int FACTOR = 4;

  double borderRadiusFactor = 0.2;
  MODIFY("BallCandidateDetectorBW:borderRadiusFactor", borderRadiusFactor);

  Vector2i center;
  Vector2i point;
  
  for(point.y = 0; point.y+1 < (int)getGameColorIntegralImage().getHeight(); ++point.y) 
  {
    double radius = estimatedBallRadius(point.x*FACTOR, point.y*FACTOR);
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*borderRadiusFactor/FACTOR+0.5);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (size < 3 || point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }

    for(point.x = border + 1; point.x + size + border+1 < (int)getGameColorIntegralImage().getWidth(); ++point.x)
    {
      int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size);

      // at least 50%
      if (inner*2 > size*size) {

        int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y-border, point.x+size+border, point.y+size+border);
        double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + (int)(radius+0.5);
        center.y = point.y*FACTOR + (int)(radius+0.5);
        best.add(center, radius, value);
      }
    }
  }
}

void BallCandidateDetectorBW::subsampling(int x0, int y0, int x1, int y1) 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)getImage().width()-1, x1);
  y1 = std::min((int)getImage().height()-1, y1);

  const double size = 12.0;
  double width_step = static_cast<double>(x1 - x0) / size;
  double height_step = static_cast<double>(y1 - y0) / size;

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) {
      unsigned char yy = getImage().getY((int)(x + 0.5), (int)(y + 0.5));

      //PEN(Color(yy, yy, yy), 1);
      //FILLBOX(index*25 + xi*2, 10 + yi*2, index*25 + (xi+1)*2, 10 + (yi+1)*2);

      //std::cerr << ((int)yy) << ";";

      sub_img[xi*12 + yi] = yy;

      //POINT_PX(ColorClasses::red, (int)(x + 0.5), (int)(y + 0.5));
      yi++;
    }
    xi++;
  }
}


double BallCandidateDetectorBW::estimatedBallRadius(int x, int y) const
{
  const double ballRadius = 50.0;
  Vector2d pointOnField;
  if(!CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  x, 
		  y, 
		  ballRadius,
		  pointOnField))
  {
    return -1;
  }

  Vector3d d = getCameraMatrix().invert()*Vector3d(pointOnField.x, pointOnField.y, ballRadius);
  double cameraBallDistance = d.abs();
  if(cameraBallDistance > ballRadius) {
    double a = atan2(ballRadius, cameraBallDistance);
    return a / getImage().cameraInfo.getOpeningAngleHeight() * getImage().cameraInfo.resolutionHeight;
  }
  
  return -1;
}

void BallCandidateDetectorBW::addBallPercept(const Vector2i& center, double radius) 
{
  const double ballRadius = 50.0;
  MultiBallPercept::BallPercept ballPercept;
  
  if(CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  center.x, 
		  center.y, 
		  ballRadius,
		  ballPercept.positionOnField))
  {
    
    ballPercept.cameraId = cameraID;
    ballPercept.centerInImage = center;
    ballPercept.radiusInImage = radius;

    getMultiBallPercept().add(ballPercept);
    getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
  }
}

