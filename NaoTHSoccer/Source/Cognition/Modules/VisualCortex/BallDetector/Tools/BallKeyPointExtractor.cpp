/**
* @file BallKeyPointExtractor.cpp
*
* Sorted list evaluated non-overlaping patches
*/

#include "BallKeyPointExtractor.h"
#include "Tools/CameraGeometry.h"

using namespace naoth;
using namespace std;

void BallKeyPointExtractor::calculateKeyPoints(BestPatchList& best) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  // we search for key points only inside the field polygon
  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getImage().height();
  for(int i = 0; i < fieldPolygon.length ; i++)
  {
    if(fieldPolygon.points[i].y < minY && fieldPolygon.points[i].y >= 0) {
      minY = fieldPolygon.points[i].y;
    }
  }

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y < (int)getGameColorIntegralImage().getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
      point.x*FACTOR, point.y*FACTOR);
    
    double radius = max( 6.0, estimatedRadius);
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*params.borderRadiusFactorClose/FACTOR+0.5);
    double radiusGuess = radius + radius*params.borderRadiusFactorClose;

    // HACK: different parameters depending on size
    if(size < 40/FACTOR) {
      border = (int)(radius*params.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = max( 2, border);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    
    for(point.x = border; point.x+size+border < (int)getGameColorIntegralImage().getWidth(); ++point.x)
    {
      int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

      if (inner*2 > size*size && greenBelow > 0.3)
      {
        int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
        double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + (int)(radius+0.5);
        center.y = point.y*FACTOR + (int)(radius+0.5);

        best.add(center, radiusGuess, value);
      }
    }
  }
}
