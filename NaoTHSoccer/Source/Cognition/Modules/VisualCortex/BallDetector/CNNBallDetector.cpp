#include "CNNBallDetector.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/BlackSpotExtractor.h"

#include "Classifier/Fy1500_Conf.h"
#include "Classifier/FrugallyDeep.h"
#include "Classifier/mbc_36k.h"
#include "Classifier/mbc_36ksm.h"
#include "Classifier/mbc_36ksm_finetuned.h"
#include "Classifier/mbd_gopen_56k.h"
#include "Classifier/TFLiteModelNaoTH.h"

using namespace std;

CNNBallDetector::CNNBallDetector():
  last_percept_valid(false)
{
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:draw_projected_ball","", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:extractPatches", "generate YUVC patches", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:keyPointsBlack", "draw black key points extracted from integral image", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPatchInImage", "draw the gray-scale patch like it is passed to the CNN in the image", false);

  getDebugParameterList().add(&params);

  cnnMap = createCNNMap();

  // initialize classifier selection
  setClassifier(params.classifier, params.classifierClose);
  setDetector(params.detector, params.detectorClose);
}

CNNBallDetector::~CNNBallDetector()
{
  getDebugParameterList().remove(&params);
}


void CNNBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  patches.clear();
  // Add the existing patches from the module, but do not perform any sorting.
  // Instead, use the order as it is provided by the original patch list.
  // Add in reverse order, so the entries with the highest value come first
  for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
    patches.push_back(*i);
  }
  addPatchByLastBall();
  addPatchByLastPercept();

  last_percept_valid = false;
  if(!patches.empty()) {
    calculateCandidates();
  }
  DEBUG_REQUEST("Vision:CNNBallDetector:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  DEBUG_REQUEST("Vision:CNNBallDetector:extractPatches",
    extractPatches();
  );

  if(params.providePatches) 
  {
    providePatches();
  } 
  else if(params.numberOfExportBestPatches > 0) 
  {
    extractPatches();
  }

  DEBUG_REQUEST("Vision:CNNBallDetector:keyPointsBlack",  
    BestPatchList bbest;
    for(BestPatchList::PatchList::iterator i = patches.begin(); i != patches.end(); ++i) {
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

std::map<string, std::shared_ptr<AbstractCNNFinder> > CNNBallDetector::createCNNMap()
{
  std::map<string, std::shared_ptr<AbstractCNNFinder> > result;
  // register classifiers

  // devils compiled models

  // Do not use a brightness offset for fy1500_conf, its baked into the first layer of the model cpp 
  result.insert({ "fy1500_conf", std::make_shared<Fy1500_Conf>() });

  // Ball Classifier from German Open 2024, we used a brightness offset of -0.59 at GO (Train Dataset mean brightness is -0.5130)
  result.insert({ "mbc_36k", std::make_shared<mbc_36k>() }); // mbc: max ball classifier

  // threshold 0.98 works good on the RC24 Monday Outdoor Test Log
  // same as mbc_36k, but with softmax activation function implemented in the model
  result.insert({ "mbc_36ksm", std::make_shared<mbc_36ksm>() }); 

  // mean: -0.5714, 
  // mbc_36ksm finetuned on devils + naoth data + manual verfied patches from RC24 SQPR testgame
  result.insert({ "mbc_36ksm_finetuned", std::make_shared<mbc_36ksm_finetuned>() });
  result.insert({ "mbc_36ksm_finetuned_tflite", std::make_shared<TFLiteModelNaoTH>("mbc_36ksm_finetuned.tflite", false, false, true)});


  
  // Ball Detector from German Open 2024, we did not use a brightness offset at GO, was trained on dataset without brightness normalization
  result.insert({ "mbd_gopen_56k", std::make_shared<mbd_gopen_56k>() }); // mbd: max ball detector

  // frugally deep models
  result.insert({ "fdeep_fy1300", std::make_shared<FrugallyDeep>("fy1300.json", true, true, true)});
  result.insert({ "fdeep_fy1500", std::make_shared<FrugallyDeep>("fy1500.json", true, true, true)});

  // tflite models 

  // trained on naodevils data + GO24
  // dataset path: naoth/datasets/classification_gopen24_nao_devils_labelstudio_validated_ball_no_ball_X_y.h5
  // dataset mean brightness offset: -0.5130
  result.insert({ "bc_36k_go24", std::make_shared<TFLiteModelNaoTH>("ball_classifier_36k_2024-04-21_GO24.tflite", false, false, true)});
  result.insert({ "bc_36k_go24_f32", std::make_shared<TFLiteModelNaoTH>("ball_classifier_36k_2024-04-21_GO24_float32.tflite", false, false, true)});

  // trained on naodevils data + GO24 + labor tests 2024 up to 2024-05-10
  // dataset path: naoth/datasets/classification_naodevils_gopen_validated_sampled_labor_testgame_may_X_y.h5
  // dataset mean brightness offset: -0.5101
  result.insert({ "bc_36k_labor", std::make_shared<TFLiteModelNaoTH>("ball_classifier_36k_2024-05-17v2.tflite", false, false, true)});
  result.insert({ "bc_36k_labor_f32", std::make_shared<TFLiteModelNaoTH>("ball_classifier_36k_2024-05-17v2_float32.tflite", false, false, true)});
  


  return result;
}


 void CNNBallDetector::setClassifier(const std::string& name, const std::string& nameClose) 
 {
   auto location = cnnMap.find(name);
   if(location != cnnMap.end()){
     currentCNN = location->second;
   }

   location = cnnMap.find(nameClose);
   if(location != cnnMap.end()){
     currentCNNClose = location->second;
   }

   ASSERT(currentCNNClose != nullptr);
 }

 void CNNBallDetector::setDetector(const std::string& name, const std::string& nameClose) 
 {
   auto location = cnnMap.find(name);
   if(location != cnnMap.end()) {
     currentCNN_detector = location->second;
   }

   location = cnnMap.find(nameClose);
   if(location != cnnMap.end()) {
     currentCNNClose_detector = location->second;
   }

   ASSERT(currentCNNClose_detector != nullptr);
 }

void CNNBallDetector::calculateCandidates()
{
  // the used patch size
  const int patch_size = 16;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for(BestPatchList::PatchList::iterator i = patches.begin(); i != patches.end(); ++i)
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

      // add an additional border as post-processing
      int postBorder = (int)(patch.radius()*params.postBorderFactorFar);
      double selectedCNNThreshold = params.cnn.threshold;

      if (params.closeMeansUseBottomCamera && cameraID == CameraInfo::Bottom)
      {
        postBorder = (int)(patch.radius()*params.postBorderFactorClose);
        selectedCNNThreshold = params.cnn.thresholdClose;
      }
      
      else if(!params.closeMeansUseBottomCamera && patch.width() >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
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
        
        DEBUG_REQUEST("Vision:CNNBallDetector:drawPatchContrast",
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

      if (cameraID == CameraInfo::Top)
      {
        double average = 0;
        double number = 0;
        for(unsigned int i = 0; i < patch.data.size(); i++) 
        {
          average += patch.data[i].pixel.y;
          number++;
        }

        if(average / number < 50) {
          PatchWork::multiplyBrightness(params.brightnessMultiplierTopDark, patch);
        } else {
          PatchWork::multiplyBrightness(params.brightnessMultiplierTop, patch);
        }
      } else {
        PatchWork::multiplyBrightness(params.brightnessMultiplierBottom, patch);
      }

      //PatchWork::multiplyBrightness((cameraID == CameraInfo::Top) ? 
      //      params.brightnessMultiplierTop : params.brightnessMultiplierBottom, patch);

      DEBUG_REQUEST("Vision:CNNBallDetector:drawPatchInImage",
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
      std::shared_ptr<AbstractCNNFinder> cnn_detector = currentCNN_detector;

      if (params.closeMeansUseBottomCamera && cameraID == CameraInfo::Bottom)
      { 
        cnn = currentCNNClose;
        cnn_detector = currentCNNClose_detector;
      }
      else if(!params.closeMeansUseBottomCamera && patch.width() >= params.postMaxCloseSize) {
        cnn = currentCNNClose;
        cnn_detector = currentCNNClose_detector;
      }

      STOPWATCH_START("CNNBallDetector:classifierPredict");
      cnn->predict(patch, params.cnn.classifierMeanBrightnessOffset);
      STOPWATCH_STOP("CNNBallDetector:classifierPredict");

      Vector2d pos = Vector2d(0.0, 0.0);
      double radius = 0.0;
      bool found = false;

      // only run the detector if the classifier predicted a ball in the patch
      if (cnn->getBallConfidence() >= selectedCNNThreshold) 
      {
        
        // HACK: resizing the patch with postBorder helps the classifier
        // but worsens the detector, so keep a copy of the original patch
        static BallCandidates::PatchYUVClassified patchForDetector((*i).min, (*i).max, patch_size);
        patchForDetector.min = (*i).min;
        patchForDetector.max = (*i).max;
        PatchWork::subsampling(getImage(), getFieldColorPercept(), patchForDetector);
        
        STOPWATCH_START("CNNBallDetector:detectorPredict");
        cnn_detector->predict(patchForDetector, params.cnn.detectorMeanBrightnessOffset);
        STOPWATCH_STOP("CNNBallDetector:detectorPredict");

        radius = cnn_detector->getRadius();
        pos = cnn_detector->getCenter();

        // sanity check needed for fy1500_conf CNN, where ball.x and ball.y (and radius)
        // has predicted values < 0 in some cases in the past
        if (pos.x >= 0.0 && pos.y >= 0.0) {
          found = true;
        }        
      }

      stopwatch.stop();
      stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

      if (found) {
        // adjust the center and radius of the patch
        Vector2d ballCenterInPatch(pos.x * patch.width(), pos.y*patch.width());
        addBallPercept(ballCenterInPatch + patch.min, radius*patch.width());
        if (last_percept_valid == false){
          last_percept_min = patch.min;
          last_percept_max = patch.max;
          last_percept_valid = true;
        }
      }

      DEBUG_REQUEST("Vision:CNNBallDetector:drawCandidates",
        // original patch
        RECT_PX(ColorClasses::skyblue, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
        // possibly revised patch 
        RECT_PX(ColorClasses::orange, patch.min.x, patch.min.y, patch.max.x, patch.max.y);
      );

      index++;
    } // end if in field
  } // end for

} // end calculateCandidates


/** 
 * Extract at most numberOfExportBestPatches for the logfile (and add it to the blackboard). 
 *
 * WARNING: This will include the border around the patch. The resulting patch is 24x24 pixel in size
 *          (currently internal patches size is 16x16). 
 *          To reconstruct the original patch, remove the border again.
 */
void CNNBallDetector::extractPatches()
{
  int idx = 0;
  for(BestPatchList::PatchList::iterator i = patches.begin(); i != patches.end(); ++i)
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

/** Provides all the internally generated patches in the representation */
void CNNBallDetector::providePatches()
{
  for(BestPatchList::PatchList::iterator i = patches.begin(); i != patches.end(); i++)
  {
    BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
    q.min = (*i).min;
    q.max = (*i).max;
    q.setSize(16);
  }
}

void CNNBallDetector::addBallPercept(const Vector2d& center, double radius) 
{
  const double ballRadius = 50.0;
  MultiBallPercept::BallPercept ballPercept;
  
  if(CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getCameraInfo(),
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

void CNNBallDetector::addPatchByLastBall()
{
  // last effort if we detect nothing we check the position of the current ball model if it is valid
  if (getBallModel().valid)
  {
    Vector3d ballInField;
    ballInField.x = getBallModel().position.x;
    ballInField.y = getBallModel().position.y;
    ballInField.z = getFieldInfo().ballRadius;

    Vector2i ballInImage;
    if (CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), ballInField, ballInImage))
    {

      double estimatedRadius = CameraGeometry::estimatedBallRadius(
          getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
          ballInImage.x, ballInImage.y);

      int border = static_cast<int>((estimatedRadius * 1.1) + 0.5);

      Vector2i start = ballInImage - border;
      Vector2i end = ballInImage + border;

      if (start.y >= 0 && end.y < static_cast<int>(getImage().height()) && start.x >= 0 && end.x < static_cast<int>(getImage().width()))
      {
        DEBUG_REQUEST("Vision:CNNBallDetector:draw_projected_ball",
                      RECT_PX(ColorClasses::pink, start.x, start.y, end.x, end.y);
                      CIRCLE_PX(ColorClasses::pink, ballInImage.x, ballInImage.y, static_cast<int>(estimatedRadius));
                      );
        // Insert ball patch if there is not already another Patch that overlaps it
        BestPatchList::Patch ballPatch  = BestPatchList::Patch(start.x,
            start.y,
            end.x,
            end.y,
            -1.0);
        bool overlaps = false;
        for(BestPatchList::PatchList::iterator i = patches.begin(); i != patches.end(); i++) {
          
          if(ballPatch.min.x < (*i).max.x && 
              ballPatch.max.x > (*i).min.x &&
              ballPatch.min.y < (*i).max.y && 
              ballPatch.max.y > (*i).min.y) {
            overlaps = true;
            break;
          }
        }
        if(overlaps == false) {
          patches.insert(patches.begin(), ballPatch);
        }
      }
    }
  }
}

void CNNBallDetector::addPatchByLastPercept(){
  if (last_percept_valid){
    BestPatchList::Patch ballPatch  = BestPatchList::Patch(last_percept_min.x,
              last_percept_min.y,
              last_percept_max.x,
              last_percept_max.y,
              99);
    patches.insert(patches.end(), ballPatch);
  }
}