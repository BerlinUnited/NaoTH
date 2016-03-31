
/**
* @file BallCandidateDetectorBW.cpp
*
* Implementation of class BallCandidateDetectorBW
*
*/

#include "BallCandidateDetectorBW.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>

#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

#include <list>

BallCandidateDetectorBW::BallCandidateDetectorBW()
{
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:keyPoints", "draw key points extracted from integral image", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawCandidates", "draw ball candidates", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawPercepts", "draw ball percepts", false);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:forceBothCameras", "always record both cameras", true);

  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawScanlines", "", false);
  DEBUG_REQUEST_REGISTER("Vision:BallCandidateDetectorBW:drawScanEndPoints", "", false);

  getDebugParameterList().add(&params);
}

BallCandidateDetectorBW::~BallCandidateDetectorBW()
{
  getDebugParameterList().remove(&params);
}

bool BallCandidateDetectorBW::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getBallCandidates().reset();

  // todo: check validity of the intergral image
  if(getGameColorIntegralImage().getWidth() == 0) {
    return false;
  }


  best.clear();
  calculateCandidates(best);

  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:keyPoints",
    for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i) {
      int radius = (int)((*i).radius + 0.5);
      RECT_PX(ColorClasses::blue, (*i).center.x - radius, (*i).center.y - radius, (*i).center.x + radius, (*i).center.y + radius);
    }
  );

  if(params.classifier.cv_svm_histogram)
  {
    STOPWATCH_START("BallCandidateDetectorBW:neuronalClassification");
    executeOpenCVModel();
    STOPWATCH_STOP("BallCandidateDetectorBW:neuronalClassification");
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


  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawPercepts",
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if((*iter).cameraId == cameraID) {
        CIRCLE_PX(ColorClasses::orange, (int)((*iter).centerInImage.x+0.5), (int)((*iter).centerInImage.y+0.5), (int)((*iter).radiusInImage+0.5));
      }
    }
  );

  return getMultiBallPercept().wasSeen();
}

void BallCandidateDetectorBW::executeOpenCVModel()
{

  if(!histModel || histModel->empty())
  {
    // load model from config folder
    try
    {
      histModel = cv::Algorithm::load<cv::ml::SVM>("Config/model_histo.dat");
      assert(histModel->getSupportVectors().rows > 0);
      assert(histModel->isTrained());
      assert(histModel->isClassifier());
    }
    catch(cv::Exception ex)
    {
      // ignore
      std::cerr << "Could not load Config/model_histo.dat" << std::endl;
    }
  }

  if(histModel && !histModel->empty())
  {
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
        cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());

        cv::Mat in = createHistoFeat(wrappedImg);

        cv::Mat out;
        histModel->predict(in, out, 0);
        if(out.at<float>(0,0) > 0.0f) {
          ballFound = true;
          addBallPercept((*i).center, radius);
        }

        DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
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
  } // end if model valid
}

void BallCandidateDetectorBW::executeSVM()
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

      DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
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
  DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
    if(best_element != best.candidates.end()) {
      RECT_PX(ColorClasses::red, (*best_element).center.x - best_radius, (*best_element).center.y - best_radius,
        (*best_element).center.x + best_radius, (*best_element).center.y + best_radius);
    }
  );
}

void BallCandidateDetectorBW::executeHeuristic()
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
      
      /*
      int minYpos = 0;
      unsigned char minY = 255;
      for(size_t k = 0; k < p.data.size(); ++k) {
        if(p.data[k] < minY) {
          minY = p.data[k];
          minYpos = k;
        }
      }
      // xi*12 + yi
      Vector2i minPos( minYpos / 12 - 6, minYpos % 12 - 6);
      */

      endPoints.clear();
      Vector2d result = spiderScan((*i).center, endPoints, radius);

      if(result.x > params.minNumberOfJumps) {
        ballFound = true;
        addBallPercept((*i).center, radius);
      }

      DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawCandidates",
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

}



void BallCandidateDetectorBW::extractPatches()
{
  for(std::list<Best::BallCandidate>::iterator i = best.candidates.begin(); i != best.candidates.end(); ++i)
  {
    if(getFieldPercept().getValidField().isInside((*i).center))
    {
      int radius = (int)((*i).radius*1.5 + 0.5);

      BallCandidates::PatchYUVClassified& q = getBallCandidates().nextFreePatchYUVClassified();
      q.min = Vector2i((*i).center.x - radius, (*i).center.y - radius);
      q.max = Vector2i((*i).center.x + radius, (*i).center.y + radius);
      subsampling(q.data, q.min.x, q.min.y, q.max.x, q.max.y);
    }
  }
}

void BallCandidateDetectorBW::calculateCandidates(Best& best) const
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

  double borderRadiusFactor = 0.5;
  MODIFY("BallCandidateDetectorBW:borderRadiusFactor", borderRadiusFactor);

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y+1 < (int)getGameColorIntegralImage().getHeight(); ++point.y)
  {
    double radius = max( 6.0, estimatedBallRadius(point.x*FACTOR, point.y*FACTOR));
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*borderRadiusFactor/FACTOR+0.5);

    // HACK
    if(size < 40/FACTOR) {
      border = (int)(radius*borderRadiusFactor*4/FACTOR+0.5);
    }

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)getGameColorIntegralImage().getHeight()) {
      continue;
    }
    

    for(point.x = border + 1; point.x + size + border+1 < (int)getGameColorIntegralImage().getWidth(); ++point.x)
    {
      int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      
      // && greenPoints(point.x*FACTOR, point.y*FACTOR, (point.x+size)*FACTOR, (point.y+size)*FACTOR) < 0.3
      // at least 50%
      if (inner*2 > size*size)
      {
        int green = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);
        int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y-border, point.x+size+border, point.y+size+border, 0);
        double value = (double)(inner - (outer - inner) - 3*green)/((double)(size+border)*(size+border));

        center.x = point.x*FACTOR + (int)(radius+0.5);
        center.y = point.y*FACTOR + (int)(radius+0.5);

        best.add(center, radius, value);
      }
    }
  }
}

double BallCandidateDetectorBW::greenPoints(int minX, int minY, int maxX, int maxY) const
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
      
    DEBUG_REQUEST("Vision:BallDetector:draw_sanity_samples",
      if(getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
        POINT_PX(ColorClasses::red, x, y);
      } else {
        POINT_PX(ColorClasses::blue, x, y);
      }
    );
  }

  return static_cast<double>(greenPoints) / static_cast<double>(sampleSize);
}

void BallCandidateDetectorBW::subsampling(std::vector<unsigned char>& data, int x0, int y0, int x1, int y1) const 
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

void BallCandidateDetectorBW::subsampling(std::vector<BallCandidates::ClassifiedPixel>& data, int x0, int y0, int x1, int y1) const 
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


double BallCandidateDetectorBW::estimatedBallRadius(int x, int y) const
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

void BallCandidateDetectorBW::addBallPercept(const Vector2i& center, double radius) 
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


Vector2d BallCandidateDetectorBW::spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints, int max_length) const
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

Vector2d BallCandidateDetectorBW::scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& points, int max_length) const
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

    DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawScanlines",
      POINT_PX(ColorClasses::blue, point.x, point.y);
    );


    // jump down found: begin
    // NOTE: we scale the filter value with the stepLength to acount for diagonal scans
    if(negativeScan.add(filter.point(), -filter.value()/stepLength))
    {
      DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawScanlines",
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
      DEBUG_REQUEST("Vision:BallCandidateDetectorBW:drawScanlines",
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

