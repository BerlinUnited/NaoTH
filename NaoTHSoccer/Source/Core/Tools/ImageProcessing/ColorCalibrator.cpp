#include "ColorCalibrator.h"

ColorCalibrator::ColorCalibrator(string name, ColorClasses::Color color)
:
  name(name),
  color(color)
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_" + string(ColorClasses::getColorName(color)) + "_" + name, " ", false);
 
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_" + string(ColorClasses::getColorName(color)) + "_" + name, " ", false);
  
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_field", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_ball", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_blue_goal", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_yellow_goal", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_pinkWaistBand", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_blueWaistBand", " ", false);
  
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_field", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_ball", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_blue_goal", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_yellow_goal", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_pinkWaistBand", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_blueWaistBand", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:reset_calibration_data", " ", false);

  reset();

  //PixelT<int> chDist;
  //PixelT<int> chIdx;
  //getBaseColorRegionPercept().orangeBall.get(chIdx, chDist);
  //orangeBallParams.set(chIdx, chDist);    
  //getBaseColorRegionPercept().blueGoal.get(chIdx, chDist);
  //blueGoalParams.set(chIdx, chDist);    
  //getBaseColorRegionPercept().yellowGoal.get(chIdx, chDist);
  //yellowGoalParams.set(chIdx, chDist);    
  //getBaseColorRegionPercept().pinkWaistBand.get(chIdx, chDist);
  //pinkWaistBandParams.set(chIdx, chDist);    
  //getBaseColorRegionPercept().blueWaistBand.get(chIdx, chDist);
  //blueWaistBandParams.set(chIdx, chDist);    
}

//void ColorCalibrator::addCalibrationRect(string rectName, int x0, int y0, int x1, int y1)
//{
//  //calibRect.registerParameters();
//  calibrationRectangles.push_back(CalibrationRect(rectName, color, x0, y0, x1, y1));
//}

void ColorCalibrator::addCalibrationRect(CalibrationRect& calibRect)
{
  calibrationRectangles.push_back(&calibRect);
}

void ColorCalibrator::reset()
{
  initHistograms(histColorChannel, histDifference);
}

void  ColorCalibrator::initHistograms
(
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference
)
{
  histColorChannel.clear();
  histDifference.clear();

  vector<double> VminusU(512);
  histDifference.push_back(VminusU);
  vector<double> UminusY(512);
  histDifference.push_back(UminusY);
  vector<double> VminusY(512);
  histDifference.push_back(VminusY);

  vector<double> Y(256);
  histColorChannel.push_back(Y);
  vector<double> U(256);
  histColorChannel.push_back(U);
  vector<double> V(256);
  histColorChannel.push_back(V);
}

void ColorCalibrator::execute(const naoth::Image& image)
{  
   calibrateColorRegions(image);
}//end execute

void ColorCalibrator::calibrateColorRegions(const naoth::Image& image)
{
  switch(color)
  {
    case ColorClasses::green:
      /// green field
      getAverageDistances(image, greenParams); 
      break;

    case ColorClasses::orange:
      /// orange ball
      getAverageDistances(image, orangeParams);
      break;

    case ColorClasses::yellow:
    case ColorClasses::skyblue:
      calibrateColorRegionGoal(image);
      break;

    case ColorClasses::pink:
      //pink waistband
      getAverageDistances(image, pinkParams);
      break;

    case ColorClasses::blue:
      //blue waistband
      getAverageDistances(image, blueParams);
      break;

    case ColorClasses::white:
      /// white lines
      getAverageDistances(image, whiteParams); 
      break;

    default:
      break;

  }
}

void ColorCalibrator::calibrateColorRegionGoal(const naoth::Image& image)
{
  /// goals
  colorPixel ccdIdx; ccdIdx.VminusU = 0; ccdIdx.UminusY = 0; ccdIdx.VminusY = 0;
  colorPixel ccdDist; ccdDist.VminusU = 0; ccdDist.UminusY = 0; ccdDist.VminusY = 0;
  PixelT<int> ccIdx; ccIdx.y = 0; ccIdx.u = 0; ccIdx.v = 0;
  PixelT<int> ccDist; ccDist.y = 0; ccDist.u = 0; ccDist.v = 0;

  getAverageDistances(image, ccdIdx, ccdDist, ccIdx, ccDist);

  calculateRegions(ccdIdx, ccdDist, ccIdx, ccDist);

  if(color == ColorClasses::yellow && ccdIdx.VminusU > 0)
  {
    yellowParams.set(ccdIdx, ccdDist);
    yellowParams.set(ccIdx, ccDist);
  }
  else if(color == ColorClasses::skyblue && ccdIdx.VminusU < 0)
  {
    skyblueParams.set(ccdIdx, ccdDist);
    skyblueParams.set(ccIdx, ccDist);
  }
}

void ColorCalibrator::getAverageDistances
(
  const naoth::Image& image,
  ColorRegion& param
)
{
  colorPixel ccdIdx;
  colorPixel ccdDist;
  PixelT<int> ccIdx; ccIdx.y = 0; ccIdx.u = 0; ccIdx.v = 0;
  PixelT<int> ccDist; ccDist.y = 0; ccDist.u = 0; ccDist.v = 0;

  getAverageDistances(image, ccdIdx, ccdDist, ccIdx, ccDist);
  calculateRegions(ccdIdx, ccdDist, ccIdx, ccDist);

  param.set(ccdIdx, ccdDist);
  param.set(ccIdx, ccDist);
}

void ColorCalibrator::getAverageDistances
(
  const naoth::Image& image,
  colorPixel& ccdIdx,
  colorPixel& ccdDist,
  PixelT<int>& ccIdx,
  PixelT<int>& ccDist
)
{
  for(unsigned int rectIdx = 0; rectIdx < calibrationRectangles.size(); rectIdx++)
  {
    CalibrationRect& calibRect = *calibrationRectangles[rectIdx];
    calibRect.draw();

    int x = Math::clamp<int>((calibRect.upperRight.x + calibRect.lowerLeft.x) / 2, 0, image.width() - 1 );
    for(int y = calibRect.lowerLeft.y; y <= calibRect.upperRight.y; y++)
    {
      getAverageDistancesLoopBody(image, x, y);
    }

    int y = Math::clamp<int>((calibRect.upperRight.y + calibRect.lowerLeft.y) / 2, 0, image.height() - 2);
    for(int x = calibRect.lowerLeft.x; x <= calibRect.upperRight.x; x++)
    {
      getAverageDistancesLoopBody(image, x, y);
    }
  }
}

void ColorCalibrator::getAverageDistancesLoopBody
(
  const naoth::Image& image,
  int& x,
  int& y
)
{
  Pixel pixel = image.get(x, y);
  
  colorPixel dist;
  dist.VminusU = pixel.v - pixel.u;
  dist.UminusY = pixel.u - pixel.y;
  dist.VminusY = pixel.v - pixel.y;

  histDifference[0][255 + dist.VminusU]++;
  histDifference[1][255 + dist.UminusY]++;
  histDifference[2][255 + dist.VminusY]++;
  histColorChannel[0][pixel.y]++;
  histColorChannel[1][pixel.u]++;
  histColorChannel[2][pixel.v]++;
}

void ColorCalibrator::calculateRegions
(
  colorPixel& ccdIdx,
  colorPixel& ccdDist,
  PixelT<int>& ccIdx,
  PixelT<int>& ccDist
)
{
  Vector3<double> m1;
  Vector3<double> m2;
  Vector3<double> z2;
  Vector3<double> sigma;

  Vector3<double> sum;

  vector<vector<double> > histColor;
  vector<vector<double> > histDiff;
  vector<vector<double> > cumHistColor;
  vector<vector<double> > cumHistDiff;

  initHistograms(histColor, histDiff);
  initHistograms(cumHistColor, cumHistDiff);

  for(int i = 0; i < 512; i++)
  {
    sum.x += histDifference[0][i];
    sum.y += histDifference[1][i];
    sum.z += histDifference[2][i];
  }

  Vector3<double> medianVal;
  Vector3<int> medianIdx;

  for(int i = 0; i < 512; i++)
  {
    histDiff[0][i] = histDifference[0][i] / sum.x;
    histDiff[1][i] = histDifference[1][i] / sum.y;
    histDiff[2][i] = histDifference[2][i] / sum.z;
    if(i > 0)
    {
      cumHistDiff[0][i] = cumHistDiff[0][i - 1] + histDiff[0][i];
      cumHistDiff[1][i] = cumHistDiff[1][i - 1] + histDiff[1][i];
      cumHistDiff[2][i] = cumHistDiff[2][i - 1] + histDiff[2][i];
    }
    else
    {
      cumHistDiff[0][i] = histDiff[0][i];
    }
    if(medianVal.x == 0.0 && cumHistDiff[0][i] > 0.5)
    {
      medianVal.x = cumHistDiff[0][i];
      medianIdx.x = i;
    }
    if(medianVal.y == 0.0 && cumHistDiff[1][i] > 0.5)
    {
      medianVal.y = cumHistDiff[1][i];
      medianIdx.y = i;
    }
    if(medianVal.z == 0.0 && cumHistDiff[2][i] > 0.5)
    {
      medianVal.z = cumHistDiff[2][i];
      medianIdx.z = i;
    }
  }

  double max = 0.0;
  int maxIdx = 0;

  for(int i = 0; i < 512; i++)
  {
     double temp =  (i - 255) * histDiff[0][i];
     m1.x += temp;
     m2.x += (i - 255) * temp;
     temp = (i - 255) * histDiff[1][i];
     m1.y += temp;
     m2.y += (i - 255) * temp;
     temp =  (i - 255) * histDiff[2][i];
     m1.z += temp;
     m2.z += (i - 255) * temp;  
     
     if (max < histDiff[0][i])
     {
        max = histDiff[0][i];
        maxIdx = i;
     }
  }
  z2.x = m2.x - (m1.x * m1.x);
  z2.y = m2.y - (m1.y * m1.y);
  z2.z = m2.z - (m1.z * m1.z);

  sigma.x = sqrt(fabs(z2.x));
  sigma.y = sqrt(fabs(z2.y));
  sigma.z = sqrt(fabs(z2.z));

  ccdIdx.VminusU = (int) Math::clamp<double>(m1.x, -255.0, 255.0);
  ccdIdx.UminusY = (int) Math::clamp<double>(m1.y, -255.0, 255.0);
  ccdIdx.VminusY = (int) Math::clamp<double>(m1.z, -255.0, 255.0); 
  //ccdIdx.VminusU = (int) Math::clamp<double>(medianIdx.x, -255.0, 255.0);
  //ccdIdx.UminusY = (int) Math::clamp<double>(medianIdx.y, -255.0, 255.0);
  //ccdIdx.VminusY = (int) Math::clamp<double>(medianIdx.z, -255.0, 255.0); 
  ccdDist.VminusU = (int) Math::clamp<double>(1.3 * sigma.x, -255.0, 255.0);
  ccdDist.UminusY = (int) Math::clamp<double>(1.3 * sigma.y, -255.0, 255.0);
  ccdDist.VminusY = (int) Math::clamp<double>(1.3 * sigma.z, -255.0, 255.0);

  m1.x = 0;
  m1.y = 0;
  m1.z = 0;
  m2.x = 0;
  m2.y = 0;
  m2.z = 0;
  z2.x = 0;
  z2.y = 0;
  z2.z = 0;
  sigma.x = 0;
  sigma.y = 0;
  sigma.z = 0;

  sum.x = 0;
  sum.y = 0;
  sum.z = 0;
  for(int i = 0; i < 256; i++)
  {
    sum.x += histColorChannel[0][i];
    sum.y += histColorChannel[1][i];
    sum.z += histColorChannel[2][i];
  }

  medianVal.x = 0.0;
  medianVal.y = 0.0;
  medianVal.z = 0.0;
  medianIdx.x = 0;
  medianIdx.y = 0;
  medianIdx.z = 0;

  for(int i = 0; i < 256; i++)
  {
    histColor[0][i] = histColorChannel[0][i] / sum.x;
    histColor[1][i] = histColorChannel[1][i] / sum.y;
    histColor[2][i] = histColorChannel[2][i] / sum.z;
    if(i > 0)
    {
      cumHistColor[0][i] = cumHistColor[0][i - 1] + histColor[0][i];
      cumHistColor[1][i] = cumHistColor[1][i - 1] + histColor[1][i];
      cumHistColor[2][i] = cumHistColor[2][i - 1] + histColor[2][i];
    }
    else
    {
      cumHistColor[0][i] = histColor[0][i];
    }
    if(medianVal.x == 0.0 && cumHistColor[0][i] > 0.5)
    {
      medianVal.x = cumHistColor[0][i];
      medianIdx.x = i;
    }
    if(medianVal.y == 0.0 && cumHistColor[1][i] > 0.5)
    {
      medianVal.y = cumHistColor[1][i];
      medianIdx.y = i;
    }
    if(medianVal.z == 0.0 && cumHistColor[2][i] > 0.5)
    {
      medianVal.z = cumHistColor[2][i];
      medianIdx.z = i;
    }
  }

  max = 0.0;
  maxIdx = 0;

  for(int i = 0; i < 256; i++)
  {
     double temp = i * histColor[0][i];
     m1.x += temp;
     m2.x += i * temp;
     temp = i * histColor[1][i];
     m1.y += temp;
     m2.y += i * temp;
     temp = i * histColor[2][i];
     m1.z += temp;
     m2.z += i * temp;

     if (max < histDiff[0][i])
     {
        max = histDiff[0][i];
        maxIdx = i;
     }
  }
  z2.x = m2.x - (m1.x * m1.x);
  z2.y = m2.y - (m1.y * m1.y);
  z2.z = m2.z - (m1.z * m1.z);

  sigma.x = sqrt(fabs(z2.x));
  sigma.y = sqrt(fabs(z2.y));
  sigma.z = sqrt(fabs(z2.z));

  //ccIdx.y = (int) Math::clamp<double>(m1.x, 0.0, 255.0);
  //ccIdx.u = (int) Math::clamp<double>(m1.y, 0.0, 255.0);
  //ccIdx.v = (int) Math::clamp<double>(m1.z, 0.0, 255.0);
  ccIdx.y = (int) Math::clamp<double>(medianIdx.x, 0.0, 255.0);
  ccIdx.u = (int) Math::clamp<double>(medianIdx.y, 0.0, 255.0);
  ccIdx.v = (int) Math::clamp<double>(medianIdx.z, 0.0, 255.0);
  ccDist.y = (int) Math::clamp<double>(1.3 * sigma.x, 0.0, 255.0);
  ccDist.u = (int) Math::clamp<double>(1.3 * sigma.y, 0.0, 255.0);
  ccDist.v = (int) Math::clamp<double>(1.3 * sigma.z, 0.0, 255.0);
}

void ColorCalibrator::get(PixelT<int>& idx, PixelT<int>& dist)
{
  switch(color)
  {
    case ColorClasses::green:
      return greenParams.get(idx, dist);

    case ColorClasses::orange:
      return orangeParams.get(idx, dist);

    case ColorClasses::yellow:
      return yellowParams.get(idx, dist);

    case ColorClasses::skyblue:
      return skyblueParams.get(idx, dist);

    case ColorClasses::pink:
      return pinkParams.get(idx, dist);

    case ColorClasses::blue:
      return blueParams.get(idx, dist);

    case ColorClasses::white:
      return whiteParams.get(idx, dist);
    
    default:
      break;
  }
}
  
void ColorCalibrator::get(colorPixel& idx, colorPixel& dist)
{
  switch(color)
  {
    case ColorClasses::green:
      return greenParams.get(idx, dist);

    case ColorClasses::orange:
      return orangeParams.get(idx, dist);

    case ColorClasses::yellow:
      return yellowParams.get(idx, dist);

    case ColorClasses::skyblue:
      return skyblueParams.get(idx, dist);

    case ColorClasses::pink:
      return pinkParams.get(idx, dist);

    case ColorClasses::blue:
      return blueParams.get(idx, dist);

    case ColorClasses::white:
      return whiteParams.get(idx, dist);

    default:
      break;
  }
}

void ColorCalibrator::drawCalibrationAreaRects()
{
  for(unsigned int rectIdx = 0; rectIdx < calibrationRectangles.size(); rectIdx++)
  {
    CalibrationRect& calibRect = *calibrationRectangles[rectIdx];
    calibRect.draw();
  }
}

void ColorCalibrator::runDebugRequests(naoth::Image& image)
{
  bool show = false;
  greenParams.sync();
  orangeParams.sync();
  skyblueParams.sync();   
  yellowParams.sync();
  pinkParams.sync();
  blueParams.sync();
  whiteParams.sync();

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors" + string(ColorClasses::getColorName(color)) + "_" + name,
    show = true;
  );

  for(unsigned int x = 0; x < image.width(); x++)
  {
    for(unsigned int y = 0; y < image.height(); y++)
    {
      const Pixel& pixel = image.get(x, y);

      if(color == ColorClasses::green && show && greenParams.inside(pixel) )
      {
        POINT_PX(ColorClasses::green, x, y);
      }

      if(color == ColorClasses::orange && show && orangeParams.inside(pixel))
      {
        POINT_PX(ColorClasses::red, x, y);
      }

      if(color == ColorClasses::skyblue && show && skyblueParams.inside(pixel))
      {
        POINT_PX(ColorClasses::skyblue, x, y);
      }

      if(color == ColorClasses::yellow && show && yellowParams.inside(pixel) )
      {
        POINT_PX(ColorClasses::yellow, x, y);
      }

      if(color == ColorClasses::pink && show && pinkParams.inside(pixel))
      {
        POINT_PX(ColorClasses::pink, x, y);
      }

      if(color == ColorClasses::blue && show && blueParams.inside(pixel) )
      {
        POINT_PX(ColorClasses::blue, x, y);
      }
    }
  }
}
