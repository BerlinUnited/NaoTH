
/**
* @file BDRBallDetector.cpp
*
* Implementation of class BDRBallDetector
*
*/

#include "BDRBallDetector.h"


BDRBallDetector::BDRBallDetector()
  : bdrCarpetRectOffset(params.bdrCarpetRectOffset)
{
  DEBUG_REQUEST_REGISTER("Vision:BDRBallDetector:draw_ball_candidates", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:BDRBallDetector:draw_ball_percepts", "draw ball percepts", false);

  theBallKeyPointExtractor = registerModule<BallKeyPointExtractor>("BallKeyPointExtractor", true);

  bdrCarpetRect = Geometry::Rect2d(
    Vector2d(-getFieldInfo().xFieldLength*0.5 + bdrCarpetRectOffset, -getFieldInfo().yFieldLength*0.5 + bdrCarpetRectOffset), 
    Vector2d(getFieldInfo().xFieldLength*0.5 - bdrCarpetRectOffset, getFieldInfo().yFieldLength*0.5 + 60 - bdrCarpetRectOffset)
  );

  getDebugParameterList().add(&params);
}

BDRBallDetector::~BDRBallDetector()
{
  getDebugParameterList().remove(&params);
}

void BDRBallDetector::execute(CameraInfo::CameraID id)
{
  // update params
  if(params.bdrCarpetRectOffset != bdrCarpetRectOffset) {
    bdrCarpetRectOffset = params.bdrCarpetRectOffset;
    
    bdrCarpetRect = Geometry::Rect2d(
      Vector2d(-getFieldInfo().xFieldLength*0.5 + bdrCarpetRectOffset, -getFieldInfo().yFieldLength*0.5 + bdrCarpetRectOffset), 
      Vector2d(getFieldInfo().xFieldLength*0.5 - bdrCarpetRectOffset, getFieldInfo().yFieldLength*0.5 + 60 - bdrCarpetRectOffset)
    );
  }

  cameraID = id;

  best.clear();

  // update parameter
  theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
  theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
  theBallKeyPointExtractor->getModuleT()->calculateKeyPointsBetter(best);

  DEBUG_REQUEST("Vision:BDRBallDetector:draw_ball_candidates",
    for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
      RECT_PX(ColorClasses::red, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );

  // calculate percepts
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {

    //const int patch_size = 16;
    //double contrast = calculateRedContrastIterative((*i).min.x,(*i).min.y,(*i).max.x,(*i).max.y,patch_size);
    
    const int32_t FACTOR = getBallDetectorIntegralImage().FACTOR;
    bool checkRedInside = false;
    int offsetY = 0;//((*i).max.y-(*i).min.y)/FACTOR;
    int offsetX = 0;//((*i).max.x-(*i).min.x)/FACTOR;
    double greenInside = getBallDetectorIntegralImage().getDensityForRect(((*i).min.x+offsetX)/FACTOR, ((*i).min.y+offsetY)/FACTOR, ((*i).max.x-offsetX)/FACTOR, ((*i).max.y-offsetY)/FACTOR, 2);
    if(greenInside > params.minRedInsideRatio) {
      checkRedInside = true;
    }

    if(checkRedInside) {
      addBallPercept(Vector2i(((*i).min.x + (*i).max.x)/2, ((*i).min.y + (*i).max.y)/2), ((*i).max.x - (*i).min.x)/2);
    }
  }

  DEBUG_REQUEST("Vision:BDRBallDetector:draw_ball_percepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );
}

void BDRBallDetector::addBallPercept(const Vector2i& center, double radius) 
{
  MultiBallPercept::BallPercept ballPercept;
  
  if(CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  center.x, 
		  center.y, 
		  getFieldInfo().ballRadius,
		  ballPercept.positionOnField))
  {
    ballPercept.cameraId = cameraID;
    ballPercept.centerInImage = center;
    ballPercept.radiusInImage = radius;

    // reject the ball
    /*
    if (params.verifyByGlobalPosition && getRobotPose().isValid &&
        !getFieldInfo().insideCarpet(getRobotPose()*ballPercept.positionOnField)) {
      return;
    }
    */

    if (params.verifyByGlobalPosition && getRobotPose().isValid &&
        !bdrCarpetRect.inside(getRobotPose().getGlobalPose()*ballPercept.positionOnField)) {
      return;
    }

    getMultiBallPercept().add(ballPercept);
    getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
  }
}


double BDRBallDetector::calculateRedContrastIterative(int x0, int y0, int x1, int y1, int size)
{
    x0 = std::max(0, x0);
    y0 = std::max(0, y0);
    x1 = std::min((int)getImage().width()-1, x1);
    y1 = std::min((int)getImage().height()-1, y1);

    double width_step = static_cast<double>(x1 - x0) / static_cast<double>(size);
    double height_step = static_cast<double>(y1 - y0) / static_cast<double>(size);

    double k = -1; // inidicating first pixel
    double n = 0;
    double sum_ = 0;
    double sum_sqr = 0;
    //Pixel pixel;

    for(double x = x0 + width_step/2.0; x < x1; x += width_step)
    {
        for(double y = y0 + height_step/2.0; y < y1; y += height_step)
        {
            
            int v = getImage().getV((int)(x + 0.5), (int)(y + 0.5));
            if(k == -1) { 
              k = v; 
            }
            n++;
            sum_ += v - k;
            sum_sqr += (v-k)*(v-k);

            /*
            getImage().get((int)(x + 0.5), (int)(y + 0.5), pixel);
            if(!getFieldColorPercept().greenHSISeparator.isColor(pixel)) { // no green!
                if(k == -1) { k = pixel.y; }
                n++;
                sum_ += pixel.y - k;
                sum_sqr += (pixel.y-k)*(pixel.y-k);
            }
            */
        }
    }

    // make sure we got some none-green pixels!
    if(n == 0) {
        return 0;
    }

    return std::sqrt((sum_sqr - (sum_ * sum_)/n)/(n));
}
