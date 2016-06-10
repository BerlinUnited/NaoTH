
/**
* @file BallCandidateDetector.cpp
*
* Implementation of class BallCandidateDetector
*
*/

#include "BallCandidateDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>

#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

#include "Tools/Math/Moments2.h"



#include <list>

BallCandidateDetector::BallCandidateDetector()
  : globalNumberOfKeysClassified(0)
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:keyPoints", "draw key points extracted from integral image", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:extractPatches", "generate YUVC patches", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawCandidates", "draw ball candidates", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawPercepts", "draw ball percepts", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:forceBothCameras", "always record both cameras", true);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawScanlines", "", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawScanEndPoints", "", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetector:drawSamples", "", false);

  getDebugParameterList().add(&params);
}

BallCandidateDetector::~BallCandidateDetector()
{
  getDebugParameterList().remove(&params);
}


bool BallCandidateDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  // todo: check validity of the intergral image
  if(getGameColorIntegralImage().getWidth() == 0) {
    return false;
  }

  best.clear();
  calculateKeyPoints(best);

  DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
    for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);

      calculateKeyPointsBlack((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );

  DEBUG_REQUEST("Vision:BallCandidateDetector:extractPatches",
    extractPatches();
  );


  if(params.classifier.cv_svm_histogram)
  {
    STOPWATCH_START("BallCandidateDetector:classifcation");
    executeOpenCVModel(id);
    STOPWATCH_STOP("BallCandidateDetector:classifcation");
  }
  else if(params.classifier.basic_svm)
  {
    executeSVM();
  }
  else if (params.classifier.heuristic) {
    executeHeuristic();
  }
  else
  {
    std::cerr << "no ball detector classifier selected in parameters!" << std::endl;
  }

  DEBUG_REQUEST("Vision:BallCandidateDetector:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  return getMultiBallPercept().wasSeen();
}

void BallCandidateDetector::executeOpenCVModel(CameraInfo::CameraID id)
{
  cv::Ptr<cv::ml::SVM>& histModel = id == CameraInfo::Top ? histModelTop : histModelBottom;
  if(!histModel || histModel->empty())
  {
    // load model from config folder
    std::string path = id == CameraInfo::Top ? "Config/model_histo_top.dat" : "Config/model_histo_bottom.dat";
    try
    {
      histModel = cv::Algorithm::load<cv::ml::SVM>(path);
      assert(histModel->getSupportVectors().rows > 0);
      assert(histModel->isTrained());
      assert(histModel->isClassifier());
    }
    catch(cv::Exception ex)
    {
      // ignore
      std::cerr << "Could not load " << path << std::endl;
    }
  }

  if(histModel && !histModel->empty())
  {
    int maxNumberOfKeys = params.classifier.maxNumberOfKeys;
    // HACK:
    if(cameraID == CameraInfo::Top) {
      maxNumberOfKeys += (params.classifier.maxNumberOfKeys-globalNumberOfKeysClassified)/2;
    }

    BallCandidates::Patch p(0);
    //BallCandidates::PatchYUVClassified pq(0);

    // the key with the highest score are at the back
    int index = 0;
    for(std::list<Best::BallCandidate>::reverse_iterator i = best.candidates.rbegin(); i != best.candidates.rend(); ++i)
    {
      if(getFieldPercept().getValidField().isInside((*i).center))
      {
        
        int radius = (int)((*i).radius*1.5 + 0.5);

        //BallCandidates::Patch& p = getBallCandidates().nextFreePatch();
        p.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
        p.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
        subsampling(p.data, p.min.x, p.min.y, p.max.x, p.max.y);

        Moments2<2> moments;
        BallCandidates::PatchYUVClassified& pq = getBallCandidates().nextFreePatchYUVClassified();
        pq.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
        pq.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);

        subsampling(pq.data, moments, p.min.x, p.min.y, p.max.x, p.max.y);
        Vector2d major, minor;
        moments.getAxes(major, minor);
        if(minor.abs() < 1 || (major.abs() / minor.abs() > params.classifier.maxMomentAxesRatio)) {
          continue;
        }

        DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
          Vector2i center( (pq.min+pq.max)/2 );
          LINE_PX(ColorClasses::red,
                  (int)(center.x - major.x), 
                  (int)(center.y - major.y),
                  (int)(center.x + major.x), 
                  (int)(center.y + major.y));

          LINE_PX(ColorClasses::yellow,
                  (int)(center.x - minor.x), 
                  (int)(center.y - minor.y),
                  (int)(center.x + minor.x), 
                  (int)(center.y + minor.y));
        );


        // limit the max amount of evaluated keys
        if(++index > maxNumberOfKeys) {
          break;
        }

        cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());
        cv::Mat in = createHistoFeat(wrappedImg);

        cv::Mat out;
        histModel->predict(in, out, 0);
        globalNumberOfKeysClassified++;
        if(out.at<float>(0,0) == 1.0f) { 
          addBallPercept((*i).center, (*i).radius);
        }

        DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
          RECT_PX(ColorClasses::yellow, (*i).center.x - radius, (*i).center.y - radius,
            (*i).center.x + radius, (*i).center.y + radius);
        );
      }
    }

  } // end if model valid
}

bool BallCandidateDetector::cvClassifyPatch(BallCandidates::Patch& p)
{
  cv::Ptr<cv::ml::SVM>& histModel = cameraID == CameraInfo::Top ? histModelTop : histModelBottom;
  if(!histModel || histModel->empty())
  {
    // load model from config folder
    std::string path = cameraID == CameraInfo::Top ? "Config/model_histo_top.dat" : "Config/model_histo_bottom.dat";
    try
    {

      histModel = cv::Algorithm::load<cv::ml::SVM>(path);
      assert(histModel->getSupportVectors().rows > 0);
      assert(histModel->isTrained());
      assert(histModel->isClassifier());
    }
    catch(cv::Exception ex)
    {
      // ignore
      std::cerr << "Could not load " << path << std::endl;
    }
  }

  if(histModel && !histModel->empty())
  {
    cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());
    cv::Mat in = createHistoFeat(wrappedImg);

    cv::Mat out;
    histModel->predict(in, out, 0);
    if(out.at<float>(0,0) == 1.0f) {
      return true;
    }
  } // end if model valid

  return false;
}


void BallCandidateDetector::executeSVM()
{
  std::list<Best::BallCandidate>::iterator best_element = best.candidates.begin();
  int best_radius = -1;
  double maxV = 0;
  std::vector<Vector2i> endPoints;

  for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      int radius = (int)((*i).radius*1.5 + 0.5);

      BallCandidates::Patch& p = getBallCandidates().nextFreePatch();
      p.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
      p.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
      subsampling(p.data, p.min.x, p.min.y, p.max.x, p.max.y);

      bool ballFound = false;
      double v = isBall(p.data);

      if(best_radius < 0 || maxV < v) {
        maxV = v;
        best_element = i;
        best_radius = radius;
      }

      endPoints.clear();
      Vector2d result = spiderScan((*i).center, endPoints, radius);

      if(v >= 0.5 && result.x > params.minNumberOfJumps) {
        ballFound = true;
        addBallPercept((*i).center, radius);
      }

      DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
        //CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        if(ballFound) {
          RECT_PX(ColorClasses::orange, (*i).center.x - radius, (*i).center.y - radius,
            (*i).center.x + radius, (*i).center.y + radius);
        } else {
          RECT_PX(ColorClasses::gray, (*i).center.x - radius, (*i).center.y - radius,
            (*i).center.x + radius, (*i).center.y + radius);
        }
      );

    }
  }
  DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
    if(best_element != best.candidates.end()) {
      RECT_PX(ColorClasses::red, (*best_element).center.x - best_radius, (*best_element).center.y - best_radius,
        (*best_element).center.x + best_radius, (*best_element).center.y + best_radius);
    }
  );
}


void BallCandidateDetector::executeHeuristic()
{
  int maxNumberOfKeys = params.classifier.maxNumberOfKeys;
  // HACK:
  if(cameraID == CameraInfo::Top) {
    maxNumberOfKeys += (params.classifier.maxNumberOfKeys-globalNumberOfKeysClassified)/2;
  }

  std::list<Best::BallCandidate>::iterator best_element = best.candidates.begin();
  std::vector<Vector2i> endPoints;

  ColorClasses::Color c = ColorClasses::gray;// debug

  int index = 0;
  int svmIndex = 0;
  for(std::list<Best::BallCandidate>::reverse_iterator i = best.candidates.rbegin(); i != best.candidates.rend(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      c = ColorClasses::yellow; // debug
      int radius = (int)((*i).radius + 0.5);

      // limit the max amount of evaluated keys
      if(++index > maxNumberOfKeys) {
        break;
      }

      Moments2<2> moments;
      BallCandidates::PatchYUVClassified& p = getBallCandidates().nextFreePatchYUVClassified();
      p.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
      p.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
      subsampling(p.data, moments, p.min.x, p.min.y, p.max.x, p.max.y);
      
      // (1) check green below
      bool checkGreenBelow = false;
      if(getImage().isInside(p.max.x, p.max.y+radius/2) && getImage().isInside(p.min.x - GameColorIntegralImage::FACTOR, p.min.y - GameColorIntegralImage::FACTOR)) {
        double greenBelow = getGameColorIntegralImage().getDensityForRect(p.min.x/4, p.max.y/4, p.max.x/4, (p.max.y+radius/2)/4, 1);
        if(greenBelow > params.heuristic.minGreenBelowRatio) {
          c = ColorClasses::pink;
          checkGreenBelow = true;
        }
      }

      // (2) check green inside
      bool checkGreenInside = false;
      int offsetY = (p.max.y-p.min.y)/4;
      int offsetX = (p.max.x-p.min.x)/4;
      double greenInside = getGameColorIntegralImage().getDensityForRect((p.min.x+offsetX)/4, (p.min.y+offsetY)/4, (p.max.x-offsetX)/4, (p.max.y-offsetY)/4, 1);
      if(greenInside < params.heuristic.maxGreenInsideRatio) {
        c = ColorClasses::red;
        checkGreenInside = true;
      }

      // (3) check black dots
      bool checkBlackDots = false;
      if(p.max.y-p.min.y > params.heuristic.minBlackDetectionSize) 
      {

        int blackSpotCount = calculateKeyPointsBlack((*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
        if( blackSpotCount > params.heuristic.blackDotsMinCount ) {
          checkBlackDots = true;
          c = ColorClasses::orange;
        }

        /*
        double blackCount = blackPointsCount(p, params.heuristic.blackDotsWhiteOffset);

        Vector2i ci = (p.max+p.min) / 2;
        int offset = (p.max.y-p.min.y) / 4;
        Vector2i a(p.min.x+offset, p.min.y+offset);
        Vector2i b(p.max.x-offset, p.max.y-offset);

        // TODO: fix this dependency
        if(getImage().isInside(p.min.x - GameColorIntegralImage::FACTOR, p.min.y - GameColorIntegralImage::FACTOR)) {
          
          a = a/GameColorIntegralImage::FACTOR;
          b = b/GameColorIntegralImage::FACTOR;
          ci = ci/GameColorIntegralImage::FACTOR;

          double s1 = getGameColorIntegralImage().getDensityForRect(a.x, a.y, ci.x, ci.y, 2);
          double s2 = getGameColorIntegralImage().getDensityForRect(ci.x, a.y, b.x, ci.y, 2);
          double s3 = getGameColorIntegralImage().getDensityForRect(a.x, ci.y, ci.x, b.y, 2);
          double s4 = getGameColorIntegralImage().getDensityForRect(ci.x, ci.y, b.x, b.y, 2);

          //double blackInside = getGameColorIntegralImage().getDensityForRect((p.min.x)/4, (p.min.y)/4, (p.max.x)/4, (p.max.y)/4, 2);

          //if(blackInside > params.heuristic.blackDotsMinRatio || blackCount > params.heuristic.blackDotsMinCount) {
          if(s1+s2+s3+s4 > params.heuristic.blackDotsMinRatio || blackCount > params.heuristic.blackDotsMinCount
          ) {
            checkBlackDots = true;
            c = ColorClasses::orange;
          }
        }
        */
      } 
      else if(p.max.y-p.min.y > params.heuristic.minBallSizeForSVM && svmIndex < max(maxNumberOfKeys/2, 1)) {
          BallCandidates::Patch p(0);
          radius = (int)((*i).radius*1.2 + 0.5);
          p.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
          p.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
          subsampling(p.data, p.min.x, p.min.y, p.max.x, p.max.y);
          ++svmIndex;
          
          if(cvClassifyPatch(p)) {
            checkBlackDots = true;
          }
      }

      // (4) check distribution
      bool checkAxes = false;
      Vector2d major, minor;
      moments.getAxes(major, minor);
      if(minor.abs() > 1 && (major.abs() / minor.abs() < params.heuristic.maxMomentAxesRatio)) {
        c = ColorClasses::skyblue;
        checkAxes = true;
      }

      if(checkGreenBelow && checkGreenInside  && checkAxes && checkBlackDots) 
      {
        addBallPercept((*i).center, radius);
      }

      DEBUG_REQUEST("Vision:BallCandidateDetector:drawCandidates",
        RECT_PX(c, (*i).center.x - radius, (*i).center.y - radius,
          (*i).center.x + radius, (*i).center.y + radius);
      );
    }
  }

} // end executeHeuristic



void BallCandidateDetector::extractPatches()
{
  for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      int radius = (int)((*i).radius*1.5 + 0.5);

      BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
      q.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
      q.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
      Moments2<2> moments;
      subsampling(q.data, moments, q.min.x, q.min.y, q.max.x, q.max.y);
    }
  }
}

int BallCandidateDetector::calculateKeyPointsBlack(int minX, int minY, int maxX, int maxY) const
{
  static Best bestBlack;
  bestBlack.clear();

  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y+1 < maxY/FACTOR; ++point.y)
  {
    // HACK: assume square
    int radius = (maxX - minX) / 5 / 2; // image coords
    int size   = ((maxX - minX) / 5)/FACTOR; // integral coords
    int border = size / 2;

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    
    for(point.x = minX/FACTOR + border + 1; point.x + size + border+1 < maxX/FACTOR; ++point.x)
    {
      int innerBlack = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 2);
      int innerWhite = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      int innerGreen = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);

      int innerDark = ((size+1)*(size+1) - innerWhite - innerGreen);

      // at least 50%
      if (innerDark*3 > (size+1)*(size+1) && innerBlack > 0)
      {
        //int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 2);
        int outerWhite = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);

        double value = (double)(innerDark + outerWhite)/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + radius;
        center.y = point.y*FACTOR + radius;

        bestBlack.add(center, radius, value);
      }
    }
  }


  DEBUG_REQUEST("Vision:BallCandidateDetector:keyPoints",
    for(std::list<Best::BallCandidate>::iterator i = bestBlack.candidates.begin(); i != bestBlack.candidates.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::red, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );

  return bestBlack.candidates.size();
}

void BallCandidateDetector::calculateKeyPoints(Best& best) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getImage().height();
  for(int i = 0; i < fieldPolygon.length ; i++)
  {
    if(fieldPolygon.points[i].y < minY && fieldPolygon.points[i].y >= 0) {
      minY = fieldPolygon.points[i].y;
    }
  }

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y+1 < (int)getGameColorIntegralImage().getHeight(); ++point.y)
  {
    double radius = max( 6.0, estimatedBallRadius(point.x*FACTOR, point.y*FACTOR));
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*params.keyDetector.borderRadiusFactorClose/FACTOR+0.5);
    double radiusGuess = radius + radius*params.keyDetector.borderRadiusFactorClose;

    // HACK
    if(size < 40/FACTOR) {
      border = (int)(radius*params.keyDetector.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = max( 2, border);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    
    for(point.x = border + 1; point.x + size + border+1 < (int)getGameColorIntegralImage().getWidth(); ++point.x)
    {
      int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      //int green = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);
      
      double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

      // && greenPoints(point.x*FACTOR, point.y*FACTOR, (point.x+size)*FACTOR, (point.y+size)*FACTOR) < 0.3
      // at least 50%
      if (inner*2 > size*size && greenBelow > 0.3)
      {
        int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
        double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + (int)(radius+0.5);
        center.y = point.y*FACTOR + (int)(radius+0.5);

        best.add(center, radiusGuess, value);
      }
    }
  }
}

double BallCandidateDetector::greenPoints(int minX, int minY, int maxX, int maxY) const
{
  const size_t sampleSize = 21;

  size_t greenPoints = 0;
  Pixel pixel;
  for(size_t i = 0; i < sampleSize; i++)
  {
    int x = Math::random(minX, maxX);
    int y = Math::random(minY, maxY);
    getImage().get(x, y, pixel);
    
    if(getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
      greenPoints++;
    }
      
    DEBUG_REQUEST("Vision:BallCandidateDetector:drawSamples",
      if(getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
        POINT_PX(ColorClasses::red, x, y);
      } else {
        POINT_PX(ColorClasses::blue, x, y);
      }
    );
  }

  return static_cast<double>(greenPoints) / static_cast<double>(sampleSize);
}

void BallCandidateDetector::subsampling(std::vector<unsigned char>& data, int x0, int y0, int x1, int y1) const 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)getImage().width()-1, x1);
  y1 = std::min((int)getImage().height()-1, y1);

  const double size = 12.0;
  data.resize((int)(size*size));

  double width_step = static_cast<double>(x1 - x0) / size;
  double height_step = static_cast<double>(y1 - y0) / size;

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) {
      unsigned char yy = getImage().getY((int)(x + 0.5), (int)(y + 0.5));
      data[xi*12 + yi] = yy;
      yi++;
    }
    xi++;
  }
}

void BallCandidateDetector::subsampling(std::vector<BallCandidates::ClassifiedPixel>& data, Moments2<2>& moments, int x0, int y0, int x1, int y1) const 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)getImage().width()-1, x1);
  y1 = std::min((int)getImage().height()-1, y1);

  //int k1 = sizeof(Pixel);
  //int k2 = sizeof(ColorClasses::Color);
  //int k3 = sizeof(BallCandidates::ClassifiedPixel);

  const double size = 12.0;
  data.resize((int)(size*size));

  double width_step = static_cast<double>(x1 - x0) / size;
  double height_step = static_cast<double>(y1 - y0) / size;

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) {
      BallCandidates::ClassifiedPixel& p = data[xi*12 + yi];
      getImage().get((int)(x + 0.5), (int)(y + 0.5), p.pixel);

      if(getFieldColorPercept().greenHSISeparator.noColor(p.pixel)) {
        p.c = (unsigned char)ColorClasses::white;
        
        if(p.pixel.y > getFieldColorPercept().greenHSISeparator.getParams().brightnesConeOffset) {
          moments.add(Vector2i((int)(x + 0.5), (int)(y + 0.5)));
        }
      
      } else if(getFieldColorPercept().greenHSISeparator.isChroma(p.pixel)) {
        p.c = (unsigned char)ColorClasses::green;
      } else {
        p.c = (unsigned char)ColorClasses::none;
      }

      yi++;
    }
    xi++;
  }
}


double BallCandidateDetector::estimatedBallRadius(int x, int y) const
{
  const double ballRadius = 50.0;
  Vector2d pointOnField;
  if(!CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  x, 
		  y, 
		  ballRadius,
		  pointOnField))
  {
    return -1;
  }

  Vector3d d = getCameraMatrix().invert()*Vector3d(pointOnField.x, pointOnField.y, ballRadius);
  double cameraBallDistance = d.abs();
  if(cameraBallDistance > ballRadius) {
    double a = atan2(ballRadius, cameraBallDistance);
    return a / getImage().cameraInfo.getOpeningAngleHeight() * getImage().cameraInfo.resolutionHeight;
  }
  
  return -1;
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
    // HACK: check the global poseition
    Vector2d ballPositionField = getRobotPose()*ballPercept.positionOnField;
    if(!params.heuristic.onlyOnField || (getRobotPose().isValid && getFieldInfo().carpetRect.inside(ballPositionField))) 
    {
      ballPercept.cameraId = cameraID;
      ballPercept.centerInImage = center;
      ballPercept.radiusInImage = radius;

      getMultiBallPercept().add(ballPercept);
      getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
    }
  }
}


double BallCandidateDetector::blackPointsCount(BallCandidates::PatchYUVClassified& p, double blackWhiteOffset) const
{
  double meanWhite = 0;
  double whiteCount = 0;
  for(size_t k = 0; k < p.data.size(); ++k) {
    if(p.data[k].c == ColorClasses::white) {
      meanWhite += p.data[k].pixel.y;
      ++whiteCount;
    }
  }

  if(whiteCount > 0) {
    meanWhite /= whiteCount;
  } else {
    return 0;
  }

  double blackCount = 0;
  for(size_t k = 0; k < p.data.size(); ++k) {
    if(p.data[k].c != ColorClasses::green && p.data[k].pixel.y + blackWhiteOffset < meanWhite) {
      ++blackCount;
    }
  }

  return blackCount;
}


Vector2d BallCandidateDetector::spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints, int max_length) const
{
  Vector2d goodBorderPointCount;
  goodBorderPointCount += scanForEdges(start, Vector2d( 1, 0), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1, 0), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d( 0, 1), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d( 0,-1), endPoints, max_length);

  goodBorderPointCount += scanForEdges(start, Vector2d( 1, 1).normalize(), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1, 1).normalize(), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d( 1,-1).normalize(), endPoints, max_length);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1,-1).normalize(), endPoints, max_length);

  return goodBorderPointCount;
}

Vector2d BallCandidateDetector::scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& points, int max_length) const
{
  Vector2i point(start);
  BresenhamLineScan scanner(point, direction, getImage().cameraInfo);

  // initialize the scanner
  Vector2i peak_point_min(start);
  Vector2i peak_point_max(start);
  MaximumScan<Vector2i,double> negativeScan(peak_point_min, params.thresholdGradientUV);
  MaximumScan<Vector2i,double> positiveScan(peak_point_max, params.thresholdGradientUV);

  Filter<Prewitt3x1, Vector2i, double, 3> filter;

  Pixel pixel;
  double stepLength = 0;
  Vector2i lastPoint(point); // needed for step length

  Vector2i lastJump;
  double span = 0;

  //int max_length = 6;
  int jumps = 0;
  int i = 0;
  while(scanner.getNextWithCheck(point) && i < max_length)
  {
    getImage().get(point.x, point.y, pixel);
    int f_y = (int)pixel.y;

    filter.add(point, f_y);
    if(!filter.ready()) {
      // assume the step length is constant, so we only calculate it in the starting phase of the filter
      stepLength += Vector2d(point - lastPoint).abs();
      lastPoint = point;
      ASSERT(stepLength > 0);
      continue;
    }

    DEBUG_REQUEST("Vision:BallCandidateDetector:drawScanlines",
      POINT_PX(ColorClasses::blue, point.x, point.y);
    );


    // jump down found: begin
    // NOTE: we scale the filter value with the stepLength to acount for diagonal scans
    if(negativeScan.add(filter.point(), -filter.value()/stepLength))
    {
      DEBUG_REQUEST("Vision:BallCandidateDetector:drawScanlines",
        POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y);
      );
      points.push_back(peak_point_min);

      span += (peak_point_min-lastJump).abs();
      lastJump = peak_point_min;
      jumps++;
    }

    // end found
    if(positiveScan.add(filter.point(), filter.value()/stepLength))
    {
      DEBUG_REQUEST("Vision:BallCandidateDetector:drawScanlines",
        POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y);
      );

      span += (peak_point_min-lastJump).abs();
      lastJump = peak_point_max;
      jumps++;
    }

    i++;
  }//end while


  Vector2d result(jumps, span/(double)jumps);
  return result; //getFieldColorPercept().isFieldColor(pixel);
}//end scanForEdges

