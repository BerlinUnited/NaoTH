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
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:refinePatches", "draw refined ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:lbpDetection", "draw ball percepts", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:extractPatches", "generate YUVC patches", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:keyPointsBlack", "draw black key points extracted from integral image", false);

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

  DEBUG_REQUEST("Vision:BallCandidateDetector:keyPointsBlack",  
    BestPatchList bbest;
    for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
      bbest.clear();
      BlackSpotExtractor::calculateKeyPointsBlackBetter(getBallDetectorIntegralImage(), bbest, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
      int idx = 0;
      for(BestPatchList::reverse_iterator j = bbest.rbegin(); j != bbest.rend(); ++j) {
        RECT_PX(ColorClasses::red, (*j).min.x, (*j).min.y, (*j).max.x, (*j).max.y);
        if(++idx > 3) {
          break;
        }
      }
    }
  );
}


void BallCandidateDetector::calculateCandidates()
{
  // todo needs a better place
  const int32_t FACTOR = getBallDetectorIntegralImage().FACTOR;

  //BestPatchList::iterator best_element = best.begin();
  std::vector<Vector2i> endPoints;

  // needed for calculating black key-points in the ball candidates
  BestPatchList bestBlackKey;

  // the used patch size
  const int patch_size = 16;

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
      //*/

      // (4) check body parts
      // TODO

      // (5) check contrast
      if(params.contrastUse) {
          double stddev = calculateContrast(getImage(),getFieldColorPercept(),min.x,min.y,max.x,max.y,patch_size);

          DEBUG_REQUEST("Vision:BallCandidateDetector:drawPatchStdDev",
                        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
                  CIRCLE( (min.x + max.x)/2, (min.y + max.y)/2, stddev / 5.0);
          );
          // skip this patch, if contrast doesn't fullfill minimum
          if(stddev <= params.contrastMinimum) {
              continue;
          }
      }


      //if(checkGreenBelow && checkGreenInside)
      if(params.haarDetector.execute)
      {
        // hack: if the classifier has not been loaded yet
        if(!cvHaarClassifier.modelLoaded()) {
          cvHaarClassifier.loadModel(params.haarDetector.model_file);
        }

        BallCandidates::Patch patchedBorder(0);
        //int size = ((*i).max.x - (*i).min.x)/2;
        patchedBorder.min = (*i).min;// - Vector2i(size,size);
        patchedBorder.max = (*i).max;// + Vector2i(size,size);

        // add an additional border as post-processing
        int size = patchedBorder.max.x - patchedBorder.min.x;
        double radius = (double) size / 2.0;
        int postBorder = (int)(radius*params.postBorderFactorFar);
        if(size >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
        {
          postBorder = (int)(radius*params.postBorderFactorClose);
        }

        patchedBorder.min.x = patchedBorder.min.x - postBorder;
        patchedBorder.min.y = patchedBorder.min.y - postBorder;
        patchedBorder.max.x = patchedBorder.max.x + postBorder;
        patchedBorder.max.y = patchedBorder.max.y + postBorder;

        if(getImage().isInside(patchedBorder.min.x, patchedBorder.min.y) && getImage().isInside(patchedBorder.max.x, patchedBorder.max.y))
        {

            auto r = (patchedBorder.max.x - patchedBorder.min.x)/2;
            auto x = (patchedBorder.min.x + patchedBorder.max.x)/2;
            auto y = (patchedBorder.min.y + patchedBorder.max.y)/2;
            DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidatesResizes",
              RECT_PX(ColorClasses::pink, x-r, y-r, x+r, y+r);
            );

          PatchWork::subsampling(getImage(), patchedBorder.data, patchedBorder.min.x, patchedBorder.min.y, patchedBorder.max.x, patchedBorder.max.y, patch_size);
          if(cvHaarClassifier.classify(patchedBorder, params.haarDetector.minNeighbors, params.haarDetector.windowSize) > 0) {
            
            if(!params.blackKeysCheck.enable || blackKeysOK(*i)) {
              addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);

            }

          }
        } else if(getImage().isInside(min.x, min.y) && getImage().isInside(max.x, max.y)) {
            BallCandidates::Patch p(0);
            p.min = (*i).min;
            p.max = (*i).max;

            PatchWork::subsampling(getImage(), p.data, min.x, min.y, max.x, max.y, patch_size);
            if(cvHaarClassifier.classify(p, params.haarDetector.minNeighbors, params.haarDetector.windowSize) > 0) {

              if(!params.blackKeysCheck.enable || blackKeysOK(*i)) {
                addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);

              }
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

    } // end if in field

  } // end for

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

double BallCandidateDetector::calculateContrast(const Image& image,  const FieldColorPercept& fielColorPercept, int x0, int y0, int x1, int y1, int size)
{
    x0 = std::max(0, x0);
    y0 = std::max(0, y0);
    x1 = std::min((int)image.width()-1, x1);
    y1 = std::min((int)image.height()-1, y1);

    double width_step = static_cast<double>(x1 - x0) / static_cast<double>(size);
    double height_step = static_cast<double>(y1 - y0) / static_cast<double>(size);

    double avg = 0;
    double cnt = 0;
    BallCandidates::ClassifiedPixel p;

    for(double x = x0 + width_step/2.0; x < x1; x += width_step)
    {
        for(double y = y0 + height_step/2.0; y < y1; y += height_step)
        {
            image.get((int)(x + 0.5), (int)(y + 0.5), p.pixel);
            if(!fielColorPercept.greenHSISeparator.isColor(p.pixel)) { // no green!
                avg += p.pixel.y;
                cnt++;
            }
        }
    }

    // we got only green ?!
    if(cnt == 0) { return 0; }

    // expected value -> average/mean
    avg /= cnt;

    double variance = 0;
    for(double x = x0 + width_step/2.0; x < x1; x += width_step)
    {
        for(double y = y0 + height_step/2.0; y < y1; y += height_step)
        {
            image.get((int)(x + 0.5), (int)(y + 0.5), p.pixel);
            if(!fielColorPercept.greenHSISeparator.isColor(p.pixel)) { // green!
                auto t = p.pixel.y;
                variance += (t-avg)*(t-avg);
            }
        }
    }
    variance /= cnt;
    return std::sqrt(variance);
}
