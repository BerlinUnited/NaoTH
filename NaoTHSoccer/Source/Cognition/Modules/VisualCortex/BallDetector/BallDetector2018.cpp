/**
* @file BallDetector2018.cpp
*
* Implementation of class BallDetector2018
*
*/

#include "BallDetector2018.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/BlackSpotExtractor.h"

#include "Classifier/DortmundCNN/CNN_dortmund.h"
#include "Classifier/DortmundCNN/CNN_dortmund2018.h"
#include "Classifier/DortmundCNN/CNN_dortmund2018_keras.h"

using namespace std;

BallDetector2018::BallDetector2018()
{
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:refinePatches", "draw refined ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);

  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:extractPatches", "generate YUVC patches", false);

  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:keyPointsBlack", "draw black key points extracted from integral image", false);

  DEBUG_REQUEST_REGISTER("Vision:BallDetector2018:drawPatchInImage", "draw the gray-scale patch like it is passed to the CNN in the image", false);

  theBallKeyPointExtractor = registerModule<BallKeyPointExtractor>("BallKeyPointExtractor", true);
  getDebugParameterList().add(&params);

  //register classifier
  cnnMap = createCNNMap();

  currentCNNClassifier = cnnMap["dortmund"];
}

BallDetector2018::~BallDetector2018()
{
  getDebugParameterList().remove(&params);
}


void BallDetector2018::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  best.clear();
  // update parameter
  theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
  theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
  //theBallKeyPointExtractor->getModuleT()->calculateKeyPoints(best);
  theBallKeyPointExtractor->getModuleT()->calculateKeyPointsBetter(best);


  // update selected classifier from parameters
  // TODO: make it more efficient
  auto location = cnnMap.find(params.classifier);
  if(location != cnnMap.end()){
    currentCNNClassifier = location->second;
  }


  if(best.size() > 0) {
    calculateCandidates();
  }

  DEBUG_REQUEST("Vision:BallDetector2018:refinePatches",
    for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
      //BestPatchList::Patch p = theBallKeyPointExtractor->getModuleT()->refineKeyPoint(*i);
      RECT_PX(ColorClasses::red, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );

  DEBUG_REQUEST("Vision:BallDetector2018:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  DEBUG_REQUEST("Vision:BallDetector2018:extractPatches",
    extractPatches();
  );

  if(params.numberOfExportBestPatches > 0) {
    extractPatches();
  }

  DEBUG_REQUEST("Vision:BallDetector2018:keyPointsBlack",  
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

std::map<string, std::shared_ptr<AbstractCNNClassifier> > BallDetector2018::createCNNMap()
{
  std::map<string, std::shared_ptr<AbstractCNNClassifier> > result;

  result.insert({"dortmund", std::make_shared<CNN_dortmund>()});
  //result.insert({ "dortmund2018", std::make_shared<CNN_dortmund2018>() });
  result.insert({ "dortmund2018_keras", std::make_shared<CNN_dortmund2018_keras>() });

  return std::move(result);
}


void BallDetector2018::calculateCandidates()
{
  // todo needs a better place
  const int32_t FACTOR = getBallDetectorIntegralImage().FACTOR;

  //BestPatchList::iterator best_element = best.begin();
  //std::vector<Vector2i> endPoints;

  // needed for calculating black key-points in the ball candidates
  //BestPatchList bestBlackKey;

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

      Vector2i min((*i).min);
      Vector2i max((*i).max);
      int size = max.x - min.x;

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

      
      
      // (5) check contrast
      if(params.contrastUse) {
          double stddev = PatchWork::calculateContrastIterative2nd(getImage(),getFieldColorPercept(),min.x,min.y,max.x,max.y,patch_size);
          
          DEBUG_REQUEST("Vision:BallDetector2018:drawPatchContrast",
            CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
            PEN("FF0000", 1); // red
            CIRCLE( (min.x + max.x)/2, (min.y + max.y)/2, stddev / 5.0);
          );

          // skip this patch, if contrast doesn't fullfill minimum
          double selectedContrastMinimum = params.contrastMinimum;
          if(size >= params.postMaxCloseSize) {
            selectedContrastMinimum = params.contrastMinimumClose;
          }
          if(stddev <= selectedContrastMinimum) {
            continue;
          }
      }

      /*
      Vector2i maxMin(min);
      Vector2i maxMax(max);
      double maxDiv = 0;
      int size = maxMax.x - maxMin.x;
      
      for (int dx = -size/4; dx < size/4; dx+=4) {
        for (int dy = -size/4; dy < size/4; dy+=4) {
          if (getImage().isInside(min.x + dx, min.y + dy) && getImage().isInside(max.x + dx, max.y+dy)) {
            
            int inner = getBallDetectorIntegralImage().getSumForRect((min.x + dx)/FACTOR, (min.y + dy)/FACTOR, (max.x + dx)/FACTOR, (max.y + dy)/FACTOR, 0);
            //double stddev = calculateContrastIterative(getImage(), getFieldColorPercept(), min.x + dx, min.y + dy, max.x + dx, max.y + dy, patch_size);
            double stddev = ((double)inner)/((double)(size)*(size));
            if (stddev > maxDiv) {
              maxDiv = stddev;
              maxMin.x = min.x + dx;
              maxMin.y = min.y + dy;
              maxMax.x = max.x + dx;
              maxMax.y = max.y + dy;
            }
          }
        }
      }
      min = maxMin;
      max = maxMax;
      
      DEBUG_REQUEST("Vision:BallDetector2018:drawCandidates",
        RECT_PX(ColorClasses::yellow, maxMin.x, maxMin.y, maxMax.x, maxMax.y);
      );
      */
      

      //if(checkGreenBelow && checkGreenInside)

      // add an additional border as post-processing
      double radius = (double) size / 2.0;
      int postBorder = (int)(radius*params.postBorderFactorFar);
      double selectedCNNThreshold = params.cnn.threshold;
      if(size >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
      {
        postBorder = (int)(radius*params.postBorderFactorClose);
        selectedCNNThreshold = params.cnn.thresholdClose;
      }

      BallCandidates::Patch patchedBorder(min-postBorder, max+postBorder, patch_size);
  
      if(getImage().isInside(patchedBorder.min) && getImage().isInside(patchedBorder.max))
      {

        auto r = (patchedBorder.max.x - patchedBorder.min.x)/2;
        auto x = (patchedBorder.min.x + patchedBorder.max.x)/2;
        auto y = (patchedBorder.min.y + patchedBorder.max.y)/2;

        DEBUG_REQUEST("Vision:BallDetector2018:drawCandidatesResizes",
          RECT_PX(ColorClasses::pink, x-r, y-r, x+r, y+r);
        );

        PatchWork::subsampling(getImage(), patchedBorder);

        PatchWork::multiplyBrightness((cameraID == CameraInfo::Top) ? 
            params.brightnessMultiplierTop : params.brightnessMultiplierBottom, patchedBorder.data);

        DEBUG_REQUEST("Vision:BallDetector2018:drawPatchInImage",
          unsigned int offsetX = patchedBorder.min.x;
          unsigned int offsetY = patchedBorder.min.y;
          unsigned int pixelWidth = (unsigned int) ((double) (patchedBorder.max.x - patchedBorder.min.x) / (double) patchedBorder.size());
          for(unsigned int x = 0; x < patchedBorder.size(); x++) {
            for(unsigned int y = 0; y < patchedBorder.size(); y++) {
              unsigned char pixelY = patchedBorder.data[(x*patch_size)+y];
              // draw each image pixel this patch pixel occupies
              for(unsigned int px=0; px < pixelWidth; px++) {
                for(unsigned int py=0; py < pixelWidth; py++) {
                  getDebugImageDrawings().drawPointToImage(pixelY, 128, 128, 
                    static_cast<int>(offsetX + (x*pixelWidth) + px), offsetY + (y*pixelWidth) + py);
                }
              }
            }
          }
        );

        // run CNN
        stopwatch.start();
        bool found = currentCNNClassifier->classify(patchedBorder);
        stopwatch.stop();
        stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

        if (found && currentCNNClassifier->getBallConfidence() >= selectedCNNThreshold) {
          if(!params.blackKeysCheck.enable || blackKeysOK(*i)) {
            addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);
          }
        }
      } 
      else if(getImage().isInside(min) && getImage().isInside(max)) 
      {
        BallCandidates::Patch p(min,max,patch_size);
        PatchWork::subsampling(getImage(), p);

        PatchWork::multiplyBrightness((cameraID == CameraInfo::Top) ? 
          params.brightnessMultiplierTop : params.brightnessMultiplierBottom, p.data);

        DEBUG_REQUEST("Vision:BallDetector2018:drawPatchInImage",
          unsigned int offsetX = p.min.x;
          unsigned int offsetY = p.min.y;
          unsigned int pixelWidth = (unsigned int) ((double) (p.max.x - p.min.x) / (double) p.size());
          for(unsigned int x = 0; x < p.size(); x++) {
            for(unsigned int y = 0; y < p.size(); y++) {
              unsigned char pixelY = p.data[(x*patch_size)+y];
              // draw each image pixel this patch pixel occupies
              for(unsigned int px=0; px < pixelWidth; px++) {
                for(unsigned int py=0; py < pixelWidth; py++) {
                  getDebugImageDrawings().drawPointToImage(pixelY, 128, 128, 
                    static_cast<int>(offsetX + (x*pixelWidth) + px), offsetY + (y*pixelWidth) + py);
                }
              }
            }
          }
        );

        stopwatch.start();
        bool found = currentCNNClassifier->classify(p);
        stopwatch.stop();
        stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

        if (found && currentCNNClassifier->getBallConfidence() >= selectedCNNThreshold) {
          if(!params.blackKeysCheck.enable || blackKeysOK(*i)) {
            addBallPercept(Vector2i((min.x + max.x)/2, (min.y + max.y)/2), (max.x - min.x)/2);
          }
        }
      }

      index++;


      DEBUG_REQUEST("Vision:BallDetector2018:drawCandidates",
        ColorClasses::Color c = ColorClasses::gray;
        if(checkGreenBelow && checkGreenInside) {
          c = ColorClasses::orange;
        } else if(checkGreenBelow || checkGreenInside) {
          c = ColorClasses::skyblue;
        }
        RECT_PX(c, min.x, min.y, max.x, max.y);
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


void BallDetector2018::extractPatches()
{
  int idx = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(idx >= params.numberOfExportBestPatches) {
      break;
    }
    int offset = ((*i).max.x - (*i).min.x)/4;
    Vector2i min = (*i).min - offset;
    Vector2i max = (*i).max + offset;

    if(getFieldPercept().getValidField().isInside(min) && getFieldPercept().getValidField().isInside(max))
    {
      BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
      q.min = min;
      q.max = max;
      q.setSize(24);
      PatchWork::subsampling(getImage(), getFieldColorPercept(), q);
      
      idx++;
    }
  }
}

void BallDetector2018::addBallPercept(const Vector2i& center, double radius) 
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

