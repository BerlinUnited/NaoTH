
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
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:peak_scan:mark_full", "mark the scanned points in image", false);

  for(size_t x = 0; x < getImage().width()/FACTOR; ++x) {
    for(size_t y = 0; y < getImage().height()/FACTOR; ++y) {
      integralImage[x][y] = 0;
    }
  }
}

BallCandidateDetectorBW::~BallCandidateDetectorBW()
{
}

void BallCandidateDetectorBW::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  integralBild();
  
  Vector2i center;
  Vector2i point;

  best.clear();

  for(point.y = 0; point.y < (int)getImage().height()/FACTOR; ++point.y) 
  {
    double radius = estimatedBallRadius(point.x*FACTOR, point.y*FACTOR);
    int size = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*0.2/FACTOR+0.5);

    if (size < 3 || point.y < border || point.y+size+border+1 > 480/FACTOR) {
      continue;
    }


    for(point.x = border; point.x + size + border < (int)getImage().width()/FACTOR; ++point.x) 
    {
      int inner  = getIntegral(point.x, point.y, point.x+size, point.y+size);

      // at least 50%
      if (inner*2 > size*size) {

        int outer = getIntegral(point.x-border, point.y-border, point.x+size+border, point.y+size+border);
        double value = (double)(inner - (outer - inner))/((double)size*size);

        center.x = point.x*FACTOR + (int)(radius+0.5);
        center.y = point.y*FACTOR + (int)(radius+0.5);
        best.add(center, radius, value);
      }
    }
  }
  
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

  index = 0;
  //CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center)) {
      //CIRCLE_PX(ColorClasses::red, (*i).center.x, (*i).center.y, (int)((*i).radius));

      int radius = (int)((*i).radius*1.5 + 0.5);

      
      subsampling((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
      //PEN("FF0000", 1);
      //CIRCLE((*i).center.x, (*i).center.y, (*i).value*10);
      //RECT_PX(isBall()?ColorClasses::blue:ColorClasses::red, (*i).center.x - radius, (*i).center.y - radius,
      //  (*i).center.x + radius, (*i).center.y + radius);

      if(isBall()) { 
        RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius,
          (*i).center.x + radius, (*i).center.y + radius);

        getBallPercept().ballWasSeen = true;
        getBallPercept().centerInImage = (*i).center;
        getBallPercept().radiusInImage = radius;
      }
      //std::cout << (*i).value << std::endl;
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

  //std::cerr << "\n";
  index++;
  //std::cout << xi << " " << yi << std::endl;
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

void BallCandidateDetectorBW::integralBild()
{
  Pixel pixel;
  Vector2i point;
  for(point.x = 1; point.x < (int)getImage().width()/FACTOR; ++point.x) 
  {
    for(point.y = 1; point.y < (int)getImage().height()/FACTOR; ++point.y) {
      getImage().get(point.x*FACTOR, point.y*FACTOR, pixel);
      
      integralImage[point.x][point.y] = integralImage[point.x-1][point.y] + integralImage[point.x][point.y-1] - integralImage[point.x-1][point.y-1];
      if(getFieldColorPercept().greenHSISeparator.noColor(pixel) 
                      //&& !getBodyContour().isOccupied(point)
        ) {
        integralImage[point.x][point.y]++;//pixel.y;
      }
    }
  }


  /*
  // experiment: cut accoring to the histogram
  int y = 0;
  int value = 0;
  int old_value = 160;
  //MaximumScan<int,double> negativeScan(point.y, -1.2*1e+9);

  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  for(point.y = 1; point.y < (int)getImage().height()/FACTOR; ++point.y) {
    value = integralImage[(int)getImage().width()/FACTOR-1][point.y] - integralImage[(int)getImage().width()/FACTOR-1][point.y-1];
    
    if(cameraID == CameraInfo::Top) {

      if(value < old_value) {
        old_value = value;
        y = point.y;
      }

      PLOT_GENERIC("integral.y", point.y, value);
    }
  }

  PEN("0000FF66", 1);
  LINE(0, y*FACTOR -FACTOR/2 ,639, y*FACTOR -FACTOR/2);
  */


  /*
  for(size_t x = 1; x < getImage().width(); ++x) {
    for(size_t y = 1; y < getImage().height(); ++y) {
      int v = (int)(((double)integralImage[x][y])/((double)integralImage[getImage().width()-1][getImage().height()-1])*255);
      getDebugImageDrawings().drawPointToImage(v,0,0,x,y);
    }
  }
  */
}


