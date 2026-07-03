#include "CNNBallDetector.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/BlackSpotExtractor.h"

#include "Classifier/Fy1500_Conf.h"
#include "Classifier/FrugallyDeep.h"
#include "Classifier/mbc_36k.h"
#include "Classifier/mbc_36ksm.h"
#include "Classifier/mbc_36ksm_finetuned.h"
#include "Classifier/mbc_36ksm_finetuned_crop.h"
#include "Classifier/mbd_gopen_56k.h"
#include "Classifier/TFLiteModelNaoTH.h"
#include "Classifier/TFLiteModelNaoTH32.h"

using namespace std;

CNNBallDetector::CNNBallDetector()
{
  // TODO: maybe rename
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:extractPatches", "generate YUVC patches", false);

  // drawings in the raw image
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image_px:drawCandidates", "draw ball candidates", false);
  //DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image_px:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image_px:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image_px:drawPatchInImage", "draw the gray-scale patch like it is passed to the CNN in the image", false);

  // TODO: obsolede; check ...
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image_px:keyPointsBlack", "draw black key points extracted from integral image", false);

  // vector drawings
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:Image:drawPercepts", "draw ball percepts", false);

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

  //patches.clear();
  // Add the existing patches from the module, but do not perform any sorting.
  // Instead, use the order as it is provided by the original patch list.
  // Add in reverse order, so the entries with the highest value come first
  //for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
  //  patches.push_back(*i);
  //}

  if(!getBestPatchList().empty()) {
    calculateCandidates();
  }
  DEBUG_REQUEST("Vision:CNNBallDetector:Image_px:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );
  DEBUG_REQUEST("Vision:CNNBallDetector:Image:drawPercepts",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    PEN("FF9900", 2); // orange
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE((int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
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

  DEBUG_REQUEST("Vision:CNNBallDetector:Image_px:keyPointsBlack",  
    BestPatchList bbest;
    for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
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

  // mean: -0.5714, now works with dynamic patchwise brightness 
  // and doesnt need to be subtracted by mean of meanBrightnessOffset parameter
  // mbc_36ksm finetuned on devils + naoth data + manual verfied patches from RC24 SQPR testgame
  result.insert({ "mbc_36ksm_finetuned", std::make_shared<mbc_36ksm_finetuned>() });

  // dynamic patchwise brightness 
  // trained on devils + devils crop (based on ball_center) + devils gauss blurred
  result.insert({ "mbc_36ksm_finetuned_crop", std::make_shared<mbc_36ksm_finetuned_crop>() });
  
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
  
  result.insert({ "bc-rc26-150k", std::make_shared<TFLiteModelNaoTH32>("bc-rc26-150k.tflite", false, false, true)});

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
  //const int patch_size = 32; // is parameter now

  // NOTE: at this point. the PatchList is already sorted with the most promising patches first
  int index = 0;
  for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).min) && getFieldPercept().getValidField().isInside((*i).max))
    {
      // limit the max amount of evaluated keys
      if(index > params.maxNumberOfKeys) {
        break;
      }

      static BallCandidates::PatchYUVClassified patch((*i).min, (*i).max, params.patch_size);
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
        
        DEBUG_REQUEST("Vision:CNNBallDetector:Image:drawPatchContrast",
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

      DEBUG_REQUEST("Vision:CNNBallDetector:Image_px:drawPatchInImage",
        unsigned int offsetX = patch.min.x;
        unsigned int offsetY = patch.min.y;
        unsigned int pixelWidth = (unsigned int) ((double) (patch.max.x - patch.min.x) / (double) patch.size() + 0.5);

        for(unsigned int x = 0; x < patch.size(); x++) {
          for(unsigned int y = 0; y < patch.size(); y++) 
          {
            unsigned char pixelY = patch.data[x*params.patch_size + y].pixel.y;

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

      int redCount = 0;
      for(const BallCandidates::ClassifiedPixel& p : patch.data) {
        if (p.c == (unsigned char)ColorClasses::red) {
          redCount++;
        }
      }
      /*
      if(redCount > 3) {
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        PEN("FF00FF", 1); // red
        Vector2i c = patch.center();
        CIRCLE( c.x, c.y, patch.radius());
      }
     */

      STOPWATCH_START("CNNBallDetector:classifierPredict");
      cnn->predict(patch, params.cnn.classifierMeanBrightnessOffset);
      STOPWATCH_STOP("CNNBallDetector:classifierPredict");
      std::cout << "Ball Confidence" << cnn->getBallConfidence() << std::endl;
      // only run the detector if the classifier predicted a ball in the patch
      if (cnn->getBallConfidence() >= selectedCNNThreshold || redCount > params.redCount) 
      {
        
        // HACK: resizing the patch with postBorder helps the classifier
        // but worsens the detector, so keep a copy of the original patch
        static BallCandidates::PatchYUVClassified patchForDetector((*i).min, (*i).max, params.patch_size);
        patchForDetector.min = (*i).min;
        patchForDetector.max = (*i).max;
        PatchWork::subsampling(getImage(), getFieldColorPercept(), patchForDetector);
          

        STOPWATCH_START("CNNBallDetector:detectorPredict");
        cnn_detector->predict(patchForDetector, params.cnn.detectorMeanBrightnessOffset);
        STOPWATCH_STOP("CNNBallDetector:detectorPredict");

        double radius = cnn_detector->getRadius();
        Vector2d pos = cnn_detector->getCenter();

        // sanity check needed for fy1500_conf CNN, where ball.x and ball.y (and radius)
        // has predicted values < 0 in some cases in the past
        if (pos.x >= 0.0 && pos.y >= 0.0) {
          // adjust the center and radius of the patch
          Vector2d ballCenterInPatch(pos.x * patchForDetector.width(), pos.y*patchForDetector.width());
          //addBallPercept(ballCenterInPatch + patchForDetector.min, radius*patchForDetector.width());
          
          addBallPercept(ballCenterInPatch + patchForDetector.min, patch.radius());
        }        
      }

      stopwatch.stop();
      stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

      DEBUG_REQUEST("Vision:CNNBallDetector:Image_px:drawCandidates",
        // original patch
        RECT_PX(ColorClasses::skyblue, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
        // possibly revised patch 
        RECT_PX(ColorClasses::orange, patch.min.x, patch.min.y, patch.max.x, patch.max.y);
      );

      DEBUG_REQUEST("Vision:CNNBallDetector:Image:drawCandidates",
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        // original patch
        PEN("6666FF", 1); // skyblue
        BOX((*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
        // possibly revised patch 
        PEN("FF9900", 1); // orange
        BOX(patch.min.x, patch.min.y, patch.max.x, patch.max.y);
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
  // at this point, the patches are already sorted with the most promising patches first
  for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i)
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
  for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); i++)
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
