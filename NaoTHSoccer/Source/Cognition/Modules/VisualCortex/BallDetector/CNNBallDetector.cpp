#include "CNNBallDetector.h"
#include "Tools/CameraGeometry.h"

#include "Tools/PatchWork.h"
#include "Tools/BlackSpotExtractor.h"

#include "Classifier/Fy1500_Conf.h"
#include "Classifier/FrugallyDeep.h"

using namespace std;

CNNBallDetector::CNNBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawCandidates", "draw ball candidates", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawCandidatesResizes", "draw ball candidates (resized)", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:refinePatches", "draw refined ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPercepts", "draw ball percepts", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPatchContrast", "draw patch contrast (only when contrast-check is in use!", false);
  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:draw_projected_ball","", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:extractPatches", "generate YUVC patches", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:keyPointsBlack", "draw black key points extracted from integral image", false);

  DEBUG_REQUEST_REGISTER("Vision:CNNBallDetector:drawPatchInImage", "draw the gray-scale patch like it is passed to the CNN in the image", false);

  theBallKeyPointExtractor = registerModule<BallKeyPointExtractor>("BallKeyPointExtractor", true);
  getDebugParameterList().add(&params);

  cnnMap = createCNNMap();

  setClassifier("fy1500_conf", "fy1500_conf");
}

CNNBallDetector::~CNNBallDetector()
{
  getDebugParameterList().remove(&params);
}

void CNNBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  best.clear();
  // update parameter
  theBallKeyPointExtractor->getModuleT()->setParameter(params.keyDetector);
  theBallKeyPointExtractor->getModuleT()->setCameraId(cameraID);
  theBallKeyPointExtractor->getModuleT()->calculateKeyPoints(best);

  addPatchByLastBall();
  
  // update selected classifier from parameters
  // TODO: make it more efficient
  setClassifier(params.classifier, params.classifierClose);

  if (best.size() > 0)
  {
    calculateCandidates();
  }

  DEBUG_REQUEST("Vision:CNNBallDetector:refinePatches",
                for (BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
                  //BestPatchList::Patch p = theBallKeyPointExtractor->getModuleT()->refineKeyPoint(*i);
                  RECT_PX(ColorClasses::red, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
                });

  DEBUG_REQUEST("Vision:CNNBallDetector:drawPercepts",
                for (MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
                  if ((*iter).cameraId == cameraID)
                  {
                    CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x + 0.5), (int)((*iter).centerInImage.y + 0.5), (int)((*iter).radiusInImage + 0.5));
                  }
                });

  DEBUG_REQUEST("Vision:CNNBallDetector:extractPatches",
                extractPatches(););

  if (params.numberOfExportBestPatches > 0)
  {
    extractPatches();
  }

  DEBUG_REQUEST("Vision:CNNBallDetector:keyPointsBlack",
                BestPatchList bbest;
                for (BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i) {
                  bbest.clear();
                  BlackSpotExtractor::calculateKeyPointsBlackBetter(getBallDetectorIntegralImage(), bbest, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
                  int idx = 0;
                  for (BestPatchList::reverse_iterator j = bbest.rbegin(); j != bbest.rend(); ++j)
                  {
                    RECT_PX(ColorClasses::red, (*j).min.x, (*j).min.y, (*j).max.x, (*j).max.y);
                    if (++idx > 3)
                    {
                      break;
                    }
                  }
                });
}

std::map<string, std::shared_ptr<AbstractCNNFinder>> CNNBallDetector::createCNNMap()
{
  std::map<string, std::shared_ptr<AbstractCNNFinder>> result;

  // register classifiers
  result.insert({"fy1500_conf", std::make_shared<Fy1500_Conf>()});

#ifndef WIN32
  result.insert({"fdeep_fy1300", std::make_shared<FrugallyDeep>("fy1300.json")});
  result.insert({"fdeep_fy1500", std::make_shared<FrugallyDeep>("fy1500.json")});
#endif

  return result;
}

void CNNBallDetector::setClassifier(const std::string &name, const std::string &nameClose)
{
  auto location = cnnMap.find(name);
  if (location != cnnMap.end())
  {
    currentCNN = location->second;
  }

  location = cnnMap.find(nameClose);
  if (location != cnnMap.end())
  {
    currentCNNClose = location->second;
  }
}

void CNNBallDetector::calculateCandidates()
{
  // the used patch size
  const int patch_size = 16;

  // NOTE: patches are sorted in the ascending order, so start from the end to get the best patches
  int index = 0;
  for (BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  { 
    if (getFieldPercept().getValidField().isInside((*i).min) && getFieldPercept().getValidField().isInside((*i).max))
    {
      // limit the max amount of evaluated keys
      if (index > params.maxNumberOfKeys)
      {
        break;
      }

      static BallCandidates::PatchYUVClassified patch((*i).min, (*i).max, patch_size);
      patch.min = (*i).min;
      patch.max = (*i).max;
      if (!getImage().isInside(patch.min) || !getImage().isInside(patch.max))
      {
        continue;
      }

      //
      // add an additional border as post-processing
      int postBorder = (int)(patch.radius() * params.postBorderFactorFar);
      double selectedCNNThreshold = params.cnn.threshold;
      if (patch.width() >= params.postMaxCloseSize) // HACK: use patch size as estimate if close or far away
      {
        postBorder = (int)(patch.radius() * params.postBorderFactorClose);
        selectedCNNThreshold = params.cnn.thresholdClose;
      }

      // resize the patch if possible
      if (getImage().isInside(patch.min - postBorder) && getImage().isInside(patch.max + postBorder))
      {
        patch.min -= postBorder;
        patch.max += postBorder;
      }

      // extract the pixels
      PatchWork::subsampling(getImage(), getFieldColorPercept(), patch);

      // (5) check contrast
      if (params.checkContrast)
      {
        //double stddev = PatchWork::calculateContrastIterative2nd(getImage(),getFieldColorPercept(),min.x,min.y,max.x,max.y,patch_size);
        double stddev = PatchWork::calculateContrastIterative2nd(patch);

        DEBUG_REQUEST("Vision:CNNBallDetector:drawPatchContrast",
                      CANVAS(((cameraID == CameraInfo::Top) ? "ImageTop" : "ImageBottom"));
                      PEN("FF0000", 1); // red
                      Vector2i c = patch.center();
                      CIRCLE(c.x, c.y, stddev / 5.0););

        // skip this patch, if contrast doesn't fullfill minimum
        double selectedContrastMinimum = params.contrastMinimum;
        if (patch.width() >= params.postMaxCloseSize)
        {
          selectedContrastMinimum = params.contrastMinimumClose;
        }
        if (stddev <= selectedContrastMinimum)
        {
          continue;
        }
      }

      PatchWork::multiplyBrightness((cameraID == CameraInfo::Top) ? params.brightnessMultiplierTop : params.brightnessMultiplierBottom, patch);

      DEBUG_REQUEST("Vision:CNNBallDetector:drawPatchInImage",
                    unsigned int offsetX = patch.min.x;
                    unsigned int offsetY = patch.min.y;
                    unsigned int pixelWidth = (unsigned int)((double)(patch.max.x - patch.min.x) / (double)patch.size() + 0.5);

                    for (unsigned int x = 0; x < patch.size(); x++) {
                      for (unsigned int y = 0; y < patch.size(); y++)
                      {
                        unsigned char pixelY = patch.data[x * patch_size + y].pixel.y;

                        // draw each image pixel this patch pixel occupies
                        for (unsigned int px = 0; px < pixelWidth; px++)
                        {
                          for (unsigned int py = 0; py < pixelWidth; py++)
                          {
                            getDebugImageDrawings().drawPointToImage(pixelY, 128, 128,
                                                                     static_cast<int>(offsetX + (x * pixelWidth) + px), offsetY + (y * pixelWidth) + py);
                          }
                        }
                      }
                    });

      // run CNN
      stopwatch.start();

      std::shared_ptr<AbstractCNNFinder> cnn = currentCNN;
      if (patch.width() >= params.postMaxCloseSize)
      {
        cnn = currentCNNClose;
      }

      STOPWATCH_START("CNNBallDetector:predict");
      cnn->find(patch, params.cnn.meanBrightness);
      STOPWATCH_STOP("CNNBallDetector:predict");

      bool found = false;
      double radius = cnn->getRadius();
      Vector2d pos = cnn->getCenter();
      if (cnn->getBallConfidence() >= selectedCNNThreshold && pos.x >= 0.0 && pos.y >= 0.0)
      {
        found = true;
      }

      stopwatch.stop();
      stopwatch_values.push_back(static_cast<double>(stopwatch.lastValue) * 0.001);

      if (found)
      {
        // adjust the center and radius of the patch
        Vector2i ballCenterInPatch(static_cast<int>(pos.x * patch.width()), static_cast<int>(pos.y * patch.width()));

        addBallPercept(patch.min + ballCenterInPatch, radius * patch.width());
      }

      DEBUG_REQUEST("Vision:CNNBallDetector:drawCandidates",
                    // original patch
                    RECT_PX(ColorClasses::skyblue, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
                    // possibly recised patch
                    RECT_PX(ColorClasses::orange, patch.min.x, patch.min.y, patch.max.x, patch.max.y););

      index++;
    } // end if in field
  }   // end for

} // end calculateCandidates

void CNNBallDetector::extractPatches()
{
  int idx = 0;
  for (BestPatchList::reverse_iterator i = best.rbegin(); i != best.rend(); ++i)
  {
    if (idx >= params.numberOfExportBestPatches)
    {
      break;
    }
    int offset = ((*i).max.x - (*i).min.x) / 4;
    Vector2i min = (*i).min - offset;
    Vector2i max = (*i).max + offset;

    if (getFieldPercept().getValidField().isInside(min) && getFieldPercept().getValidField().isInside(max))
    {
      BallCandidates::PatchYUVClassified &q = getBallCandidates().nextFreePatchYUVClassified();
      q.min = min;
      q.max = max;
      q.setSize(24);
      q.value = (*i).value;
      PatchWork::subsampling(getImage(), getFieldColorPercept(), q);

      idx++;
    }
  }
}

void CNNBallDetector::addBallPercept(const Vector2i &center, double radius)
{
  const double ballRadius = 50.0;
  MultiBallPercept::BallPercept ballPercept;

  if (CameraGeometry::imagePixelToFieldCoord(
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

void CNNBallDetector::addPatchByLastBall()
{
  if (getBallModel().valid)
  {
    Vector3d ballInField;
    ballInField.x = getBallModel().positionPreview.x;
    ballInField.y = getBallModel().positionPreview.y;
    ballInField.z = getFieldInfo().ballRadius;

    Vector2i ballInImage;
    if (CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, ballInField, ballInImage))
    {

      double estimatedRadius = CameraGeometry::estimatedBallRadius(
          getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
          ballInImage.x, ballInImage.y);

      int border = static_cast<int>((estimatedRadius * 1.2) + 0.5);

      Vector2i start = ballInImage - border;
      Vector2i end = ballInImage + border;

      if (start.y >= 0 && end.y < static_cast<int>(getImage().height()) && start.x >= 0 && end.x < static_cast<int>(getImage().width()))
      {
        DEBUG_REQUEST("Vision:CNNBallDetector:draw_projected_ball",
                      RECT_PX(ColorClasses::red, start.x, start.y, end.x, end.y););
        best.add(
            start.x,
            start.y,
            end.x,
            end.y,
            256.0);
      }
    }
  }
}
