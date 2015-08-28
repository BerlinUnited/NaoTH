/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifier.h"
#include "Tools/Math/Matrix2x2.h"

using namespace std;

FieldColorClassifier::FieldColorClassifier()
: 
  uniformGrid(getImage().width(), getImage().height(), 60, 40),
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:markCbClassification", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:markCbClassification", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:histogramUV", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:histogramUV", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:clearHistogramUV","", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:HistogramPlot", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:HistogramPlot", "", false);

  histogramUV.resize(256);
  for(size_t i = 0; i < histogramUV.size(); i++) {
    histogramUV[i].resize(256);
  }
}

FieldColorClassifier::~FieldColorClassifier()
{

}


void FieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  // TODO: set this global
  cameraID = id;
  Pixel pixel;


  // EXPERIMENTS 2D
  double alpha = 0.01;
  MODIFY("Vision:FieldColorClassifier:alpha", alpha);

  //DEBUG_REQUEST("Vision:FieldColorClassifier:clearHistogramUV",
    for(size_t i = 0; i < histogramUV.size(); i++) {
      for(size_t j = 0; j < histogramUV[i].size(); j++) {
        histogramUV[i][j] = 0;
      }
    }
  //);

  double blackOffset = 50;
  MODIFY("Vision:FieldColorClassifier:blackOffset",blackOffset);
  double brightnesConeRadiusUV = 50;
  MODIFY("Vision:FieldColorClassifier:brightnesConeRadiusUV",brightnesConeRadiusUV);
  double sigmaThresholdUV = 1;
  MODIFY("Vision:FieldColorClassifier:sigmaThresholdUV",sigmaThresholdUV);

  Moments2<2>& momentsGlobal = momentsGlobalArray[cameraID];
  
  Vector2d principleAxisMajorGlobal, principleAxisMinorGlobal;
  momentsGlobal.getAxes(principleAxisMajorGlobal, principleAxisMinorGlobal);
  Vector2d centroidGlobal = momentsGlobal.getCentroid();
  momentsGlobal.reset();

  Moments2<2> moments;

  hMax = 0;
  double yThreshold = 30;
  MODIFY("Vision:FieldColorClassifier:yThreshold", yThreshold);

  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      getImage().get(point.x, point.y, pixel);
      histogramUV[pixel.v][pixel.u]++;

      double gamma = 512 - pixel.v - pixel.u;
      gamma /= 512;

      if( pixel.u < 150 && pixel.v < 150 ) {
        // cut the conus
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;
        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset) 
        {
          Vector2i p(pixel.v, pixel.u);
          momentsGlobal.add(p, gamma);

          if((Vector2d(p) - centroidGlobal).abs() < 2*principleAxisMajorGlobal.abs()) {
            moments.add(p);
          }
        }
      }
    }
  }


  Vector2d principleAxisMajor, principleAxisMinor;
  moments.getAxes(principleAxisMajor, principleAxisMinor);
  Vector2d centroid = moments.getCentroid();

  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  PEN("FF0000", 1);
  OVAL_ROTATED(centroid.x, 
               centroid.y, 
               principleAxisMinor.abs(),
               principleAxisMajor.abs(),
               principleAxisMinor.angle());



  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:histogramUV",
    if(cameraID == CameraInfo::Bottom) draw_histogramUV();
  );
  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:histogramUV",
    if(cameraID == CameraInfo::Top) draw_histogramUV();
  );

  // END EXPERIMENT




  // reset stuff
  histogramV.clear();
  histogramU.clear();
  const int SCALE_FACTOR = 265 / histogramV.size;


  // calculate the histogram in the V channel
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      //POINT_PX(ColorClasses::red, point.x, point.y);
      unsigned char v = getImage().getV(point.x, point.y);
      histogramV.add(v / SCALE_FACTOR);
    }
  }

  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:HistogramPlot",
    plot((cameraID == CameraInfo::Top)?"Histograms:TopCam:V":"Histograms:BottomCam:V", histogramV);
  );

  // STEP 1: search for the maximal value in the weighted Cr histogramm 
  // CAUTION: the histogram created by the grid provider in the last frame is used
  double maxWeightedCr = 0.0;
  int maxWeightedIndexCr = -1;

  // the histogram is weighted with the function 
  // max^2(0,128-i)/128, i.e., we are interested only in the first half of it
  const int cr_max = histogramV.size/2 - histogramV.size/32;
  for(int i = 0; i < cr_max; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr =  ((double)(cr_max - i)) / (double)cr_max;
    double weightedCr = wCr * (double) histogramV.rawData[i];

    //HACK:
    histogramV.rawData[i] = (int)(1000*weightedCr+0.5);

    // search for maximum in the wighted Cr channel
    if(weightedCr > maxWeightedCr)
    {
      maxWeightedCr = weightedCr;
      maxWeightedIndexCr = i;
    }
  }

  // no green candidates found
  if(maxWeightedIndexCr < 0) {
    return;
  }

  for(int i = cr_max; i < histogramV.size; i++)
  {
    histogramV.rawData[i] = 0;
  }

  // ..and find optCr maxDiffs
  int maxIndexUpCr = maxWeightedIndexCr;
  int maxIndexDownCr = maxWeightedIndexCr;

  for (int i = maxWeightedIndexCr; i < histogramV.size; i++)
  {
    if (histogramV.rawData[i] > maxWeightedCr/8)
      maxIndexUpCr = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }
  for (int i = maxWeightedIndexCr; i >= 0; i--)
  {
    if (histogramV.rawData[i] > maxWeightedCr/8)
      maxIndexDownCr = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }

  Matrix2d mx(principleAxisMajor, principleAxisMinor);
  if(fabs(mx.det()) < 1e-12) return;
  mx = mx.invert();


  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markCrClassification",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        Vector2d diff = Vector2d(pixel.v, pixel.u) - centroid;
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;

        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset && (mx*diff).abs() < sigmaThresholdUV ) { //(mx*diff).abs() < sigmaThreshold ) {
          POINT_PX(ColorClasses::red, x, y); 
        }
      }
    }
  );



  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:markCrClassification",
    if(cameraID == CameraInfo::Top)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        Vector2d diff = Vector2d(pixel.v, pixel.u) - centroid;
        double yy = max(0.0,((double)pixel.y)-blackOffset) / 255.0;

        if( (Vector2d(128,128) - Vector2d(pixel.u,pixel.v)).abs() > yy*brightnesConeRadiusUV && pixel.y > blackOffset && (mx*diff).abs() < sigmaThresholdUV ) { //(mx*diff).abs() < sigmaThreshold ) {
          POINT_PX(ColorClasses::red, x, y); 
        }
      }
    }
  );


  //STEP 2
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      //POINT_PX(ColorClasses::red, point.x, point.y);
      getImage().get(point.x, point.y, pixel);
      
      //if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr ) {
        histogramU.add(pixel.u / SCALE_FACTOR);
      //}
    }
  }

  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:HistogramPlot",
    plot((cameraID == CameraInfo::Top)?"Histograms:TopCam:U":"Histograms:BottomCam:U", histogramU);
  );

  double maxWeightedCb = 0.0;
  int maxWeightedIndexCb = -1;

  // the histogram is weighted with the function 
  // max^2(0,128-i)/128, i.e., we are interested only in the first half of it
  const int cb_max = histogramU.size/2;
  for(int i = 0; i < cb_max; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCb =  ((double)(cb_max - i)) / (double)cb_max;
    double weightedCb = wCb * (double) histogramU.rawData[i];

    //HACK:
    histogramU.rawData[i] = (int)(1000*weightedCb+0.5);

    // search for maximum in the wighted Cr channel
    if(weightedCb > maxWeightedCb)
    {
      maxWeightedCb = weightedCb;
      maxWeightedIndexCb = i;
    }
  }

  // no green candidates found
  if(maxWeightedIndexCb < 0) {
    return;
  }

  for(int i = cb_max; i < histogramU.size; i++)
  {
    histogramU.rawData[i] = 0;
  }

  // ..and find optCr maxDiffs
  int maxIndexUpCb = maxWeightedIndexCb;
  int maxIndexDownCb = maxWeightedIndexCb;

  for (int i = maxWeightedIndexCb; i < histogramU.size; i++)
  {
    if (histogramU.rawData[i] > maxWeightedCb/8)
      maxIndexUpCb = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }
  for (int i = maxWeightedIndexCb; i >= 0; i--)
  {
    if (histogramU.rawData[i] > maxWeightedCb/8)
      maxIndexDownCb = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }


  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markCbClassification",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr && pixel.u > maxIndexDownCb && pixel.u < maxIndexUpCb ) {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

  maxIndexDownCr = 105 - 10;
  maxIndexUpCr   = 105 + 10;
  maxIndexDownCb = 115 - 10;
  maxIndexUpCb   = 115 + 10;

  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:markCbClassification",
    if(cameraID == CameraInfo::Top)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr && pixel.u > maxIndexDownCb && pixel.u < maxIndexUpCb ) {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

}//end execute


void FieldColorClassifier::draw_histogramUV() const
{
  static const int ySize = 256;
  static const int xSize = 256;
  double yWidth = 0.5;
  double xWidth = 0.5;

  FIELD_DRAWING_CONTEXT;
  Color white(1.0,1.0,1.0);
  Color black(0.0,0.0,0.0);

  double maxValue = 0;
  for(size_t i = 0; i < histogramUV.size(); i++) {
    for(size_t j = 0; j < histogramUV[i].size(); j++) {
      double l = histogramUV[i][j];//log(histogramUV[i][j]+1);
      maxValue = max(maxValue, l);
    }
  }

  if(maxValue <= 0) return;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x), yWidth*(2*y));
      
      double t = /*log(histogramUV[x][y]+1)*/ histogramUV[x][y] / maxValue;
      Color color = black*t + white*(1-t);
      //PEN(color, 20);
      //FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      getDebugImageDrawings().drawPointToImage((unsigned char)(t*254), 128, 128, x,y);
    }
  }
}//end draw_closest_points

