/**
* @file BallCandidateDetector.cpp
*
* Implementation of class BallCandidateDetector
*
*/

#include "BallCandidateDetector.h"
#include "Tools/CameraGeometry.h"

using namespace std;

BallCandidateDetector::BallCandidateDetector()
  : globalNumberOfKeysClassified(0)
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawCandidates", "draw ball candidates", false);

  getDebugParameterList().add(&params);
}

BallCandidateDetector::~BallCandidateDetector()
{
  getDebugParameterList().remove(&params);
}


void BallCandidateDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  // todo: check validity of the intergral image
  if(getGameColorIntegralImage().getWidth() == 0) {
    return;
  }

  best.clear();
  calculateKeyPoints(best);

  DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
    for(BestPatchList::iterator i = best.begin(); i != best.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);

      calculateKeyPointsBlack((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );

  executeHeuristic();
}


void BallCandidateDetector::executeHeuristic()
{
  int maxNumberOfKeys = params.maxNumberOfKeys;
  // HACK:
  if(cameraID == CameraInfo::Top) {
    maxNumberOfKeys += (params.maxNumberOfKeys-globalNumberOfKeysClassified)/2;
  }

  BestPatchList::iterator best_element = best.begin();
  std::vector<Vector2i> endPoints;

  ColorClasses::Color c = ColorClasses::gray;// debug

  int index = 0;
  int svmIndex = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      c = ColorClasses::yellow; // debug
      int radius = (int)((*i).radius + 0.5);

      // limit the max amount of evaluated keys
      if(++index > maxNumberOfKeys) {
        break;
      }

      Vector2i min((*i).center.x - radius, (*i).center.y - radius);
      Vector2i max((*i).center.x + radius, (*i).center.y + radius);

      // (1) check green below
      bool checkGreenBelow = false;
      if(getImage().isInside(max.x, max.y+radius/2) && getImage().isInside(min.x - GameColorIntegralImage::FACTOR, min.y - GameColorIntegralImage::FACTOR)) {
        double greenBelow = getGameColorIntegralImage().getDensityForRect(min.x/4, max.y/4, max.x/4, (max.y+radius/2)/4, 1);
        if(greenBelow > params.heuristic.minGreenBelowRatio) {
          c = ColorClasses::pink;
          checkGreenBelow = true;
        }
      }

      // (2) check green inside
      bool checkGreenInside = false;
      int offsetY = (max.y-min.y)/4;
      int offsetX = (max.x-min.x)/4;
      double greenInside = getGameColorIntegralImage().getDensityForRect((min.x+offsetX)/4, (min.y+offsetY)/4, (max.x-offsetX)/4, (max.y-offsetY)/4, 1);
      if(greenInside < params.heuristic.maxGreenInsideRatio) {
        c = ColorClasses::red;
        checkGreenInside = true;
      }

      // (3) check black dots
      bool checkBlackDots = false;
      if(max.y-min.y > params.heuristic.minBlackDetectionSize) 
      {
        int blackSpotCount = calculateKeyPointsBlack((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
        if( blackSpotCount > params.heuristic.blackDotsMinCount ) {
          checkBlackDots = true;
          c = ColorClasses::orange;
        }
      }

      DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
        RECT_PX(c, (*i).center.x - radius, (*i).center.y - radius,
          (*i).center.x + radius, (*i).center.y + radius);
      );
    }
  }

} // end executeHeuristic


int BallCandidateDetector::calculateKeyPointsBlack(int minX, int minY, int maxX, int maxY) const
{
  static BestPatchList bestBlack;
  bestBlack.clear();

  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y+1 < maxY/FACTOR; ++point.y)
  {
    // HACK: assume square
    int radius = (maxX - minX) / 5 / 2; // image coords
    int size   = ((maxX - minX) / 5)/FACTOR; // integral coords
    int border = size / 2;

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    
    for(point.x = minX/FACTOR + border + 1; point.x + size + border+1 < maxX/FACTOR; ++point.x)
    {
      int innerBlack = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 2);
      int innerWhite = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      int innerGreen = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);

      int innerDark = ((size+1)*(size+1) - innerWhite - innerGreen);

      // at least 50%
      if (innerDark*3 > (size+1)*(size+1) && innerBlack > 0)
      {
        //int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 2);
        int outerWhite = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);

        double value = (double)(innerDark + outerWhite)/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + radius;
        center.y = point.y*FACTOR + radius;

        bestBlack.add(center, radius, value);
      }
    }
  }

  DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
    for(BestPatchList::iterator i = bestBlack.begin(); i != bestBlack.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::red, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );

  return bestBlack.size();
}

void BallCandidateDetector::calculateKeyPoints(BestPatchList& best) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

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
  
  for(point.y = minY/FACTOR; point.y+1 < (int)getGameColorIntegralImage().getHeight(); ++point.y)
  {
    double radius = max( 6.0, estimatedBallRadius(point.x*FACTOR, point.y*FACTOR));
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*params.keyDetector.borderRadiusFactorClose/FACTOR+0.5);
    double radiusGuess = radius + radius*params.keyDetector.borderRadiusFactorClose;

    // HACK
    if(size < 40/FACTOR) {
      border = (int)(radius*params.keyDetector.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = max( 2, border);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    
    for(point.x = border + 1; point.x + size + border+1 < (int)getGameColorIntegralImage().getWidth(); ++point.x)
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

double BallCandidateDetector::estimatedBallRadius(int x, int y) const
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



