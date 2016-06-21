/**
* @file BallCandidateDetector.cpp
*
* Implementation of class BallCandidateDetector
*
*/

#include "BallCandidateDetector.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/CVClassifier.h"
#include "Tools/BlackSpotExtractor.h"

using namespace std;

BallCandidateDetector::BallCandidateDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawPercepts", "draw ball percepts", false);

  theBallKeyPointExtractor = registerModule<BallKeyPointExtractor>("BallKeyPointExtractor", true);
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
  // update parameter
  theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
  theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
  theBallKeyPointExtractor->getModuleT()->calculateKeyPoints(best);

  calculateCandidates();

  DEBUG_REQUEST("Vision:BallCandidateDetector:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );
}


void BallCandidateDetector::calculateCandidates()
{
  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  //BestPatchList::iterator best_element = best.begin();
  std::vector<Vector2i> endPoints;

  // needed for calculating black key-points in the ball candidates
  BestPatchList bestBlackKey;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      int radius = (int)((*i).radius + 0.5);

      // limit the max amount of evaluated keys
      if(index > params.maxNumberOfKeys) {
        break;
      }

      Vector2i min((*i).center.x - radius, (*i).center.y - radius);
      Vector2i max((*i).center.x + radius, (*i).center.y + radius);

      // (1) check green below
      bool checkGreenBelow = false;
      if(getImage().isInside(max.x, max.y+radius/2) && getImage().isInside(min.x - FACTOR, min.y - FACTOR)) {
        double greenBelow = getGameColorIntegralImage().getDensityForRect(min.x/FACTOR, max.y/FACTOR, max.x/FACTOR, (max.y+radius/2)/FACTOR, 1);
        if(greenBelow > params.heuristic.minGreenBelowRatio) {
          checkGreenBelow = true;
        }
      }

      // (2) check green inside
      bool checkGreenInside = false;
      int offsetY = (max.y-min.y)/FACTOR;
      int offsetX = (max.x-min.x)/FACTOR;
      double greenInside = getGameColorIntegralImage().getDensityForRect((min.x+offsetX)/FACTOR, (min.y+offsetY)/FACTOR, (max.x-offsetX)/FACTOR, (max.y-offsetY)/FACTOR, 1);
      if(greenInside < params.heuristic.maxGreenInsideRatio) {
        checkGreenInside = true;
      }

      // (3) check black dots
      bool checkBlackDots = false;
      if(max.y-min.y > params.heuristic.minBlackDetectionSize) 
      {
        BlackSpotExtractor::calculateKeyPoints(getGameColorIntegralImage(), bestBlackKey, min.x, min.y, max.x, max.y);

        DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
          for(BestPatchList::iterator i = bestBlackKey.begin(); i != bestBlackKey.end(); ++i) {
            int radius = (int)((*i).radius + 0.5);
            RECT_PX(ColorClasses::red, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
          }
        );

        if( (int)bestBlackKey.size() >= params.heuristic.blackDotsMinCount ) {
          checkBlackDots = true;
        }
      } else {
        //TODO: what to do with small balls?
      }

      if(checkGreenBelow && checkGreenInside)
      {
        const int patch_size = 12*2; 
        
        BallCandidates::Patch p(0);
        p.min = Vector2i((*i).center.x - radius*2, (*i).center.y - radius*2);
        p.max = Vector2i((*i).center.x + radius*2, (*i).center.y + radius*2);

        if(getImage().isInside(p.min.x, p.min.y) && getImage().isInside(p.max.x, p.max.y)) 
        {
          PatchWork::subsampling(getImage(), p.data, p.min.x, p.min.y, p.max.x, p.max.y, patch_size);
          if(cvClassifier.classify(p) > params.haarDetector.minNeighbors) {
            addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);
          }
        }
      }


      DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
        ColorClasses::Color c = ColorClasses::gray;
        if(checkGreenBelow && checkGreenInside) {
          c = ColorClasses::orange;
        } else if(checkGreenBelow || checkGreenInside) {
          c = ColorClasses::skyblue;
        }
        RECT_PX(c, min.x, min.y, max.x, max.y);

        if(checkBlackDots) {
          CIRCLE_PX(ColorClasses::red, (min.x + max.x)/2, (min.y + max.y)/2, (max.x - min.x)/2);
        }
      );


      // TODO: provide ball candidates based on above criteria
      //getBallCandidates();

      // an acceptable candidate found...
      if(checkGreenBelow && checkGreenInside) {
        index++;
      }

    }
  }

} // end executeHeuristic

void BallCandidateDetector::addBallPercept(const Vector2i& center, double radius) 
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


