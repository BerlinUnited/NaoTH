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


  cnnMap = createCNNMap();

  setClassifier("dortmund", "dortmund");
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
  setClassifier(params.classifier, params.classifierClose);


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

// TODO: this is not very elegant, but it is used by the BallDetectorEvaluator.cpp
std::map<string, std::shared_ptr<AbstractCNNClassifier> > BallDetector2018::createCNNMap()
{
  std::map<string, std::shared_ptr<AbstractCNNClassifier> > result;

  // register classifiers
  result.insert({"dortmund", std::make_shared<CNN_dortmund>()});
  //result.insert({ "dortmund2018", std::make_shared<CNN_dortmund2018>() });
  result.insert({ "dortmund2018_keras", std::make_shared<CNN_dortmund2018_keras>() });

  return result;
}

void BallDetector2018::setClassifier(const std::string& name, const std::string& nameClose) 
{
  auto location = cnnMap.find(name);
  if(location != cnnMap.end()){
    currentCNNClassifier = location->second;
  }

  location = cnnMap.find(nameClose);
  if(location != cnnMap.end()){
    currentCNNClassifierClose = location->second;
  }
}


void BallDetector2018::calculateCandidates()
{
  // the used patch size
  const int patch_size = 16;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for(BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).min) && getFieldPercept().getValidField().isInside((*i).max))
    {
      // limit the max amount of evaluated keys
      if(index > params.maxNumberOfKeys) {
        break;
      }

      static BallCandidates::PatchYUVClassified patch((*i).min, (*i).max, patch_size);
      patch.min = (*i).min;
      patch.max = (*i).max;
      if(!getImage().isInside(patch.min) || !getImage().isInside(patch.max)) {
        continue;
      }

      //
      // add an additional border as post-processing
      int postBorder = (int)(patch.radius()*params.postBorderFactorFar);
      double selectedCNNThreshold = params.cnn.threshold;
      if(patch.width() >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
      {
        postBorder = (int)(patch.radius()*params.postBorderFactorClose);
        selectedCNNThreshold = params.cnn.thresholdClose;
      }

      // resize the patch if possible
      if(getImage().isInside(patch.min - postBorder) && getImage().isInside(patch.max + postBorder)) {
        patch.min -= postBorder;
        patch.max += postBorder;
      }
      
      // extract the pixels
      PatchWork::subsampling(getImage(), getFieldColorPercept(), patch);

      // (5) check contrast
      if(params.checkContrast) 
      {
        //double stddev = PatchWork::calculateContrastIterative2nd(getImage(),getFieldColorPercept(),min.x,min.y,max.x,max.y,patch_size);
        double stddev = PatchWork::calculateContrastIterative2nd(patch);
        
        DEBUG_REQUEST("Vision:BallDetector2018:drawPatchContrast",
          CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
          PEN("FF0000", 1); // red
          Vector2i c = patch.center();
          CIRCLE( c.x, c.y, stddev / 5.0);
        );

        // skip this patch, if contrast doesn't fullfill minimum
        double selectedContrastMinimum = params.contrastMinimum;
        if(patch.width() >= params.postMaxCloseSize) {
          selectedContrastMinimum = params.contrastMinimumClose;
        }
        if(stddev <= selectedContrastMinimum) {
          continue;
        }
      }

      PatchWork::multiplyBrightness((cameraID == CameraInfo::Top) ? 
            params.brightnessMultiplierTop : params.brightnessMultiplierBottom, patch);

      DEBUG_REQUEST("Vision:BallDetector2018:drawPatchInImage",
        unsigned int offsetX = patch.min.x;
        unsigned int offsetY = patch.min.y;
        unsigned int pixelWidth = (unsigned int) ((double) (patch.max.x - patch.min.x) / (double) patch.size() + 0.5);

        for(unsigned int x = 0; x < patch.size(); x++) {
          for(unsigned int y = 0; y < patch.size(); y++) 
          {
            unsigned char pixelY = patch.data[x*patch_size + y].pixel.y;

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

      std::shared_ptr<AbstractCNNClassifier> classifier = currentCNNClassifier;
      if(patch.width() >= params.postMaxCloseSize) {
        classifier = currentCNNClassifierClose;
      }

      bool found = classifier->classify(patch);
      stopwatch.stop();
      stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

      if (found && classifier->getBallConfidence() >= selectedCNNThreshold) {
        addBallPercept(patch.center(), patch.radius());
      }

      DEBUG_REQUEST("Vision:BallDetector2018:drawCandidates",
        // original patch
        RECT_PX(ColorClasses::skyblue, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
        // possibly recised patch 
        RECT_PX(ColorClasses::orange, patch.min.x, patch.min.y, patch.max.x, patch.max.y);
      );

      index++;
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

