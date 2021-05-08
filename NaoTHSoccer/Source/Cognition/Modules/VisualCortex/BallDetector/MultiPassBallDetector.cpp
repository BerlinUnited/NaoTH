#include "MultiPassBallDetector.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/BlackSpotExtractor.h"

#include "Classifier/Model1.h"
#include "Classifier/Fy1500_Conf.h"
#include "Classifier/FrugallyDeep.h"

using namespace std;

MultiPassBallDetector::MultiPassBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);
  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawProjectedBall","", false);

  DEBUG_REQUEST_REGISTER("Vision:MultiPassBallDetector:drawPatchInImage", "draw the gray-scale patch like it is passed to the CNN in the image", false);

  theBallKeyPointExtractor = registerModule<BallKeyPointExtractor>("BallKeyPointExtractor", true);
  getDebugParameterList().add(&params);

  cnnMap = createCNNMap();

  // initialize classifier selection
  setClassifier(params.classifier, params.classifierClose);
}

MultiPassBallDetector::~MultiPassBallDetector()
{
  getDebugParameterList().remove(&params);
}


void MultiPassBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  std::vector<MultiBallPercept::BallPercept> percepts;
  std::vector<double> scores;

  std::vector<BestPatchList::Patch> allPatches;

  // 1. pass: projection of the previous ball
  BestPatchList lastBallPatches = getPatchesByLastBall();
  allPatches = lastBallPatches.asVector();
  executeCNNOnPatches(allPatches, static_cast<int>(lastBallPatches.size()), false, percepts, scores);
  addBallPercepts(percepts, scores);

  // 2. pass: keypoints
  if(!getMultiBallPercept().wasSeen()) {
    percepts.clear();
    scores.clear();

    // update parameter
    std::vector<BestPatchList::Patch> keypointPatches;

    theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
    theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
    BestPatchList keypointList;
    theBallKeyPointExtractor->getModuleT()->calculateKeyPointsBetter(keypointList);
    keypointPatches = keypointList.asVector();
    allPatches.insert(allPatches.end(), keypointPatches.begin(), keypointPatches.end());

    executeCNNOnPatches(keypointPatches, params.maxNumberOfKeys, params.checkContrast, percepts, scores);
    addBallPercepts(percepts, scores);

  }

  // 3. pass: patches around the most promising patch of the second pass
  if(!getMultiBallPercept().wasSeen() && !scores.empty()) {
   
    // Find the maximum score
    double maxScore = 0.0;
    size_t idxMaxScore = 0;
    for(size_t i = 0; i < scores.size(); i++) {
      if(scores[i] > maxScore) {
        idxMaxScore = i;
        maxScore = scores[i];
      }
    }
    // Add keypoints around the original patch
    MultiBallPercept::BallPercept centerPercept = percepts[idxMaxScore];
    std::vector<BestPatchList::Patch> aroundPromisingKeyPoint;
    int radius = 30; //static_cast<int>(centerPercept.radiusInImage+0.5);
    int centerX = static_cast<int>(centerPercept.centerInImage.x+0.5);
    int centerY = static_cast<int>(centerPercept.centerInImage.y+0.5);

    BestPatchList::Patch p1(centerX - radius, centerY-radius, centerX, centerY, 0.0);
    aroundPromisingKeyPoint.push_back(p1);

    BestPatchList::Patch p2(centerX, centerY-radius, centerX+radius, centerY, 0.0);
    aroundPromisingKeyPoint.push_back(p2);

    BestPatchList::Patch p3(centerX, centerY, centerX + radius, centerY+radius, 0.0);
    aroundPromisingKeyPoint.push_back(p3);


    BestPatchList::Patch p4(centerX-radius, centerY, centerX, centerY+radius, 0.0);
    aroundPromisingKeyPoint.push_back(p4);

    DEBUG_REQUEST("Vision:MultiPassBallDetector:drawCandidates",
      for(size_t i =0; i < aroundPromisingKeyPoint.size(); i++) {
        RECT_PX(ColorClasses::pink, aroundPromisingKeyPoint[i].min.x, aroundPromisingKeyPoint[i].min.y, aroundPromisingKeyPoint[i].max.x,aroundPromisingKeyPoint[i].max.y);
      }
     );

    percepts.clear();
    scores.clear();

    executeCNNOnPatches(aroundPromisingKeyPoint, static_cast<int>(aroundPromisingKeyPoint.size()), false, percepts, scores);
    allPatches.insert(allPatches.end(), aroundPromisingKeyPoint.begin(), aroundPromisingKeyPoint.end());

    addBallPercepts(percepts, scores);

  }

  DEBUG_REQUEST("Vision:MultiPassBallDetector:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  if(params.providePatches) 
    {
      providePatches(allPatches);
    } 
    else if(params.numberOfExportBestPatches > 0) 
    {
      extractPatches(allPatches);
    }

}

void MultiPassBallDetector::executeCNNOnPatches(const std::vector<BestPatchList::Patch>& best, 
    int maxNumberOfKeys, bool checkContrast,
    std::vector<MultiBallPercept::BallPercept>& ballPercepts,
    std::vector<double>& scores) {
  // the used patch size
  const int patch_size = 16;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for(size_t i = 0; i < best.size(); i++)
  {
    if(getFieldPercept().getValidField().isInside(best[i].min) && getFieldPercept().getValidField().isInside(best[i].max))
    {
      // limit the max amount of evaluated keys
      if(index > maxNumberOfKeys) {
        break;
      }

      static BallCandidates::PatchYUVClassified patch(best[i].min, best[i].max, patch_size);
      patch.min = best[i].min;
      patch.max = best[i].max;
      if(!getImage().isInside(patch.min) || !getImage().isInside(patch.max)) {
        continue;
      }

      //
      // add an additional border as post-processing
      int postBorder = (int)(patch.radius()*params.postBorderFactorFar);
      if(patch.width() >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
      {
        postBorder = (int)(patch.radius()*params.postBorderFactorClose);
      }

      // resize the patch if possible
      if(getImage().isInside(patch.min - postBorder) && getImage().isInside(patch.max + postBorder)) {
        patch.min -= postBorder;
        patch.max += postBorder;
      }
      
      // extract the pixels
      PatchWork::subsampling(getImage(), getFieldColorPercept(), patch);

      // (5) check contrast
      if(checkContrast) 
      {
        //double stddev = PatchWork::calculateContrastIterative2nd(getImage(),getFieldColorPercept(),min.x,min.y,max.x,max.y,patch_size);
        double stddev = PatchWork::calculateContrastIterative2nd(patch);
        
        DEBUG_REQUEST("Vision:MultiPassBallDetector:drawPatchContrast",
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

      DEBUG_REQUEST("Vision:MultiPassBallDetector:drawPatchInImage",
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

      std::shared_ptr<AbstractCNNFinder> cnn = currentCNN;
      if(patch.width() >= params.postMaxCloseSize) {
        cnn = currentCNNClose;
      }

      STOPWATCH_START("MultiPassBallDetector:predict");
      cnn->predict(patch, params.cnn.meanBrightnessOffset);
      STOPWATCH_STOP("MultiPassBallDetector:predict");

      bool found = false;
      double radius = cnn->getRadius();
      Vector2d pos = cnn->getCenter();
      stopwatch.stop();
      stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

      scores.push_back(cnn->getBallConfidence());
      // adjust the center and radius of the patch
      Vector2d ballCenterInPatch(pos.x * patch.width(), pos.y*patch.width());
      ballPercepts.push_back(createBallPercept(ballCenterInPatch + patch.min, radius*patch.width()));

      DEBUG_REQUEST("Vision:MultiPassBallDetector:drawCandidates",
        // original patch
        RECT_PX(ColorClasses::skyblue, best[i].min.x, best[i].min.y, best[i].max.x, best[i].max.y);
        // possibly revised patch 
        RECT_PX(ColorClasses::orange, patch.min.x, patch.min.y, patch.max.x, patch.max.y);
      );

      index++;
    } // end if in field
  } // end for
}

BestPatchList MultiPassBallDetector::getPatchesByLastBall() {
  BestPatchList best;
  if (getBallModel().valid)
  {
    Vector3d ballInField;
    ballInField.x = getBallModel().position.x;
    ballInField.y = getBallModel().position.y;
    ballInField.z = getFieldInfo().ballRadius;

    Vector2i ballInImage;
    if (CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, ballInField, ballInImage))
    {

      double estimatedRadius = CameraGeometry::estimatedBallRadius(
          getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
          ballInImage.x, ballInImage.y);

      int border = static_cast<int>((estimatedRadius * 1.1) + 0.5);

      Vector2i start = ballInImage - border;
      Vector2i end = ballInImage + border;

      if (start.y >= 0 && end.y < static_cast<int>(getImage().height()) && start.x >= 0 && end.x < static_cast<int>(getImage().width()))
      {
        DEBUG_REQUEST("Vision:MultiPassBallDetector:drawProjectedBall",
                      RECT_PX(ColorClasses::pink, start.x, start.y, end.x, end.y);
                      CIRCLE_PX(ColorClasses::pink, ballInImage.x, ballInImage.y, static_cast<int>(estimatedRadius));
                      );
        best.add(
            start.x,
            start.y,
            end.x,
            end.y,
            -1.0);
      }
    }
  }
  return best;
}

std::map<string, std::shared_ptr<AbstractCNNFinder> > MultiPassBallDetector::createCNNMap()
{
  std::map<string, std::shared_ptr<AbstractCNNFinder> > result;

  // register classifiers
  result.insert({ "fy1500_conf", std::make_shared<Fy1500_Conf>() });
  result.insert({ "model1", std::make_shared<Model1>() });

#ifndef WIN32
  result.insert({ "fdeep_fy1300", std::make_shared<FrugallyDeep>("fy1300.json")});
  result.insert({ "fdeep_fy1500", std::make_shared<FrugallyDeep>("fy1500.json")});
#endif

  return result;
}


 void MultiPassBallDetector::setClassifier(const std::string& name, const std::string& nameClose) 
 {
   auto location = cnnMap.find(name);
   if(location != cnnMap.end()){
     currentCNN = location->second;
   }

   location = cnnMap.find(nameClose);
   if(location != cnnMap.end()){
     currentCNNClose = location->second;
   }
 }


/** 
 * Extract at most numberOfExportBestPatches for the logfile (and add it to the blackboard). 
 *
 * WARNING: This will include the border around the patch. The resulting patch is 24x24 pixel in size
 *          (currently internal patches size is 16x16). 
 *          To reconstruct the original patch, remove the border again.
 */
void MultiPassBallDetector::extractPatches(const std::vector<BestPatchList::Patch>& best)
{
  int idx = 0;
  for(size_t i=0; i < best.size(); i++)
  {
    if(idx >= params.numberOfExportBestPatches) {
      break;
    }
    int offset = (best[i].max.x - best[i].min.x)/4;
    Vector2i min = best[i].min - offset;
    Vector2i max = best[i].max + offset;

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

/** Provides all the internally generated patches in the representation */
void MultiPassBallDetector::providePatches(const std::vector<BestPatchList::Patch>& best)
{
  for(size_t i=0; i < best.size(); i++)
  {
    BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
    q.min = best[i].min;
    q.max = best[i].max;
    q.setSize(16);
  }
}

MultiBallPercept::BallPercept MultiPassBallDetector::createBallPercept(const Vector2d& center, double radius) {
  const double ballRadius = 50.0;
  MultiBallPercept::BallPercept ballPercept;
  ballPercept.cameraId = cameraID;
  ballPercept.centerInImage = center;
  ballPercept.radiusInImage = radius;
  return ballPercept;
}
  

void MultiPassBallDetector::addBallPercepts(std::vector<MultiBallPercept::BallPercept>& percepts,
    std::vector<double>& scores) {
  const double ballRadius = 50.0;

  for(size_t i=0; i < percepts.size(); i++) {
    if(scores[i] >= params.cnn.threshold && percepts[i].centerInImage.x >= 0.0 && percepts[i].centerInImage.y >= 0.0) {
        if(CameraGeometry::imagePixelToFieldCoord(
            getCameraMatrix(), 
            getImage().cameraInfo,
            percepts[i].centerInImage.x, 
            percepts[i].centerInImage.y, 
            ballRadius,
            percepts[i].positionOnField))
        {
            getMultiBallPercept().add(percepts[i]);
            getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
        }
    }
  }  

}