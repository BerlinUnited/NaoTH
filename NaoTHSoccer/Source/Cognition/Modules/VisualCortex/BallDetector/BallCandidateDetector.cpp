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
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:refinePatches", "draw refined ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawPercepts", "draw ball percepts", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:lbpDetection", "draw ball percepts", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:extractPatches", "generate YUVC patches", false);

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

  best.clear();
  // update parameter
  theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
  theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
  //theBallKeyPointExtractor->getModuleT()->calculateKeyPoints(best);
  theBallKeyPointExtractor->getModuleT()->calculateKeyPointsBetter(best);

  if(best.size() > 0) {
    calculateCandidates();
  }

  DEBUG_REQUEST("Vision:BallCandidateDetector:refinePatches",
    for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
      BestPatchList::Patch p = theBallKeyPointExtractor->getModuleT()->refineKeyPoint(*i);
      RECT_PX(ColorClasses::red, p.min.x, p.min.y, p.max.x, p.max.y);
    }
  );

  DEBUG_REQUEST("Vision:BallCandidateDetector:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  DEBUG_REQUEST("Vision:BallCandidateDetector:extractPatches",
    extractPatches();
  );

  if(params.numberOfExportBestPatches > 0) {
    extractPatches();
  }
}


void BallCandidateDetector::calculateCandidates()
{
  // todo needs a better place
  const int32_t FACTOR = getBallDetectorIntegralImage().FACTOR;

  //BestPatchList::iterator best_element = best.begin();
  std::vector<Vector2i> endPoints;

  // needed for calculating black key-points in the ball candidates
  BestPatchList bestBlackKey;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).min) && getFieldPercept().getValidField().isInside((*i).max))
    {
      //int radius = (int)((*i).radius + 0.5);

      // limit the max amount of evaluated keys
      if(index > params.maxNumberOfKeys) {
        break;
      }

      const Vector2i& min((*i).min);
      const Vector2i& max((*i).max);

      // (1) check green below
      bool checkGreenBelow = false;
      if(getImage().isInside(max.x, max.y+(max.y-min.y)/2) && getImage().isInside(min.x - FACTOR, min.y - FACTOR)) {
        double greenBelow = getBallDetectorIntegralImage().getDensityForRect(min.x/FACTOR, max.y/FACTOR, max.x/FACTOR, (max.y+(max.y-min.y)/2)/FACTOR, 1);
        if(greenBelow > params.heuristic.minGreenBelowRatio) {
          checkGreenBelow = true;
        }
      }

      // (2) check green inside
      bool checkGreenInside = false;
      int offsetY = (max.y-min.y)/FACTOR;
      int offsetX = (max.x-min.x)/FACTOR;
      double greenInside = getBallDetectorIntegralImage().getDensityForRect((min.x+offsetX)/FACTOR, (min.y+offsetY)/FACTOR, (max.x-offsetX)/FACTOR, (max.y-offsetY)/FACTOR, 1);
      if(greenInside < params.heuristic.maxGreenInsideRatio) {
        checkGreenInside = true;
      }

      // (3) check black dots
      bool checkBlackDots = false;
      /*
      if(max.y-min.y > params.heuristic.minBlackDetectionSize) 
      {
        BlackSpotExtractor::calculateKeyPoints(getBallDetectorIntegralImage(), bestBlackKey, min.x, min.y, max.x, max.y);

        DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
          for(BestPatchList::iterator i = bestBlackKey.begin(); i != bestBlackKey.end(); ++i) {
            RECT_PX(ColorClasses::red, min.x, min.y, max.x, max.y);
          }
        );

        if( (int)bestBlackKey.size() >= params.heuristic.blackDotsMinCount ) {
          checkBlackDots = true;
        }
      } else {
        //TODO: what to do with small balls?
      }
      */

      //if(checkGreenBelow && checkGreenInside)
      if(params.haarDetector.execute)
      {
        // hack: if the classifier has not been loaded yet
        if(!cvHaarClassifier.modelLoaded()) {
          cvHaarClassifier.loadModel(params.haarDetector.model_file);
        }

        const int patch_size = 12;
        
        BallCandidates::Patch p(0);
        //int size = ((*i).max.x - (*i).min.x)/2;
        p.min = (*i).min;// - Vector2i(size,size);
        p.max = (*i).max;// + Vector2i(size,size);

        if(getImage().isInside(p.min.x, p.min.y) && getImage().isInside(p.max.x, p.max.y)) 
        {
          PatchWork::subsampling(getImage(), p.data, p.min.x, p.min.y, p.max.x, p.max.y, patch_size);
          if(cvHaarClassifier.classify(p, params.haarDetector.minNeighbors, params.haarDetector.windowSize) > 0) {
            addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);
          }
        }

        index++;
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
        //index++;
      }

    }
  }

} // end calculateCandidates


void BallCandidateDetector::extractPatches()
{
  int idx = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(idx >= params.numberOfExportBestPatches) {
      break;
    }
    Vector2i min = (*i).min;
    Vector2i max = (*i).max;
    int offset = (max.x - min.x)/4;
    min.x -= offset;
    min.y -= offset;
    max.x += offset;
    max.y += offset;

    if(getFieldPercept().getValidField().isInside(min) && getFieldPercept().getValidField().isInside(max))
    {
      BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
      q.min = min;
      q.max = max;
      PatchWork::subsampling(getImage(), getFieldColorPercept(), q.data, q.min.x, q.min.y, q.max.x, q.max.y, 24);
      
      idx++;
    }
  }
}

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


