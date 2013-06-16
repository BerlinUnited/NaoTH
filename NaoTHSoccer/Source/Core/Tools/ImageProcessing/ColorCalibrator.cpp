#include "ColorCalibrator.h"

ColorCalibrator::ColorCalibrator(string name, ColorClasses::Color color)
:
  strength(1.3),
  name(name),
  color(color)
{
  reset();
}

ColorCalibrator::ColorCalibrator(double strength, string name, ColorClasses::Color color)
:
  strength(strength),
  name(name),
  color(color)
{
  reset();
}

void ColorCalibrator::addCalibrationRect(CalibrationRect& calibRect)
{
  calibrationRectangles.push_back(&calibRect);
}

void ColorCalibrator::reset()
{
  initHistograms();
}

void  ColorCalibrator::initHistograms()
{
  histDifferenceVminusU.clear();
  histDifferenceUminusY.clear();
  histDifferenceVminusY.clear();
  histColorChannelY.clear();
  histColorChannelU.clear();
  histColorChannelV.clear();
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

    //int x = Math::clamp<int>((calibRect.upperRight.x + calibRect.lowerLeft.x) / 2, 0, image.width() - 1 );
    //int y = Math::clamp<int>((calibRect.upperRight.y + calibRect.lowerLeft.y) / 2, 0, image.height() - 2);

    //use every enclosed pixel for descriptive statistic
    for(int y = calibRect.lowerLeft.y; y <= calibRect.upperRight.y; y++)
    {
      for(int x = calibRect.lowerLeft.x; x <= calibRect.upperRight.x; x++)
      {
        getAverageDistancesLoopBody(image, x, y);
      }
    }

    //use only pixels along the middle cross of the selected regions for descriptive statistic
    //int x = Math::clamp<int>((calibRect.upperRight.x + calibRect.lowerLeft.x) / 2, 0, image.width() - 1 );
    //for(int y = calibRect.lowerLeft.y; y <= calibRect.upperRight.y; y++)
    //{
    //  getAverageDistancesLoopBody(image, x, y);
    //}

    //int y = Math::clamp<int>((calibRect.upperRight.y + calibRect.lowerLeft.y) / 2, 0, image.height() - 2);
    //for(int x = calibRect.lowerLeft.x; x <= calibRect.upperRight.x; x++)
    //{
    //  getAverageDistancesLoopBody(image, x, y);
    //}
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

  histDifferenceVminusU.add(255 + dist.VminusU);
  histDifferenceUminusY.add(255 + dist.UminusY);
  histDifferenceVminusY.add(255 + dist.VminusY);
  histColorChannelY.add(pixel.y);
  histColorChannelU.add(pixel.u);
  histColorChannelV.add(pixel.v);
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
  Vector3<double> sigma;
  Vector3<int> medianIdx;

  histDifferenceVminusU.calculate();
  histDifferenceUminusY.calculate();
  histDifferenceVminusY.calculate();

  m1.x = histDifferenceVminusU.mean - 255.0;
  m1.y = histDifferenceUminusY.mean - 255.0;
  m1.z = histDifferenceVminusY.mean - 255.0;
  medianIdx.x = histDifferenceVminusU.median - 255;
  medianIdx.y = histDifferenceUminusY.median - 255;
  medianIdx.z = histDifferenceVminusY.median - 255;
  sigma.x = histDifferenceVminusU.sigma;
  sigma.y = histDifferenceUminusY.sigma;
  sigma.z = histDifferenceVminusY.sigma;

  ccdIdx.VminusU = (int) Math::clamp<double>(m1.x, -255.0, 255.0);
  ccdIdx.UminusY = (int) Math::clamp<double>(m1.y, -255.0, 255.0);
  ccdIdx.VminusY = (int) Math::clamp<double>(m1.z, -255.0, 255.0); 
  //ccdIdx.VminusU = (int) Math::clamp<double>(medianIdx.x, -255.0, 255.0);
  //ccdIdx.UminusY = (int) Math::clamp<double>(medianIdx.y, -255.0, 255.0);
  //ccdIdx.VminusY = (int) Math::clamp<double>(medianIdx.z, -255.0, 255.0); 
  ccdDist.VminusU = (int) Math::clamp<double>(strength * sigma.x, -255.0, 255.0);
  ccdDist.UminusY = (int) Math::clamp<double>(strength * sigma.y, -255.0, 255.0);
  ccdDist.VminusY = (int) Math::clamp<double>(strength * sigma.z, -255.0, 255.0);

  histColorChannelY.calculate();
  histColorChannelU.calculate();
  histColorChannelV.calculate();

  m1.x = histColorChannelY.mean;
  m1.y = histColorChannelU.mean;
  m1.z = histColorChannelV.mean;
  medianIdx.x = histColorChannelY.median;
  medianIdx.y = histColorChannelU.median;
  medianIdx.z = histColorChannelV.median;
  sigma.x = histColorChannelY.sigma;
  sigma.y = histColorChannelU.sigma;
  sigma.z = histColorChannelV.sigma;

  ccIdx.y = (int) Math::clamp<double>(m1.x, 0.0, 255.0);
  ccIdx.u = (int) Math::clamp<double>(m1.y, 0.0, 255.0);
  ccIdx.v = (int) Math::clamp<double>(m1.z, 0.0, 255.0);
  //ccIdx.y = (int) Math::clamp<double>(medianIdx.x, 0.0, 255.0);
  //ccIdx.u = (int) Math::clamp<double>(medianIdx.y, 0.0, 255.0);
  //ccIdx.v = (int) Math::clamp<double>(medianIdx.z, 0.0, 255.0);
  ccDist.y = (int) Math::clamp<double>(strength * sigma.x, 0.0, 255.0);
  ccDist.u = (int) Math::clamp<double>(strength * sigma.y, 0.0, 255.0);
  ccDist.v = (int) Math::clamp<double>(strength * sigma.z, 0.0, 255.0);
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

void ColorCalibrator::drawCalibrationAreaRects(CameraInfo::CameraID camID)
{
  for(unsigned int rectIdx = 0; rectIdx < calibrationRectangles.size(); rectIdx++)
  {
    CalibrationRect& calibRect = *calibrationRectangles[rectIdx];
    if(camID == CameraInfo::Top)
    {
      calibRect.drawTop();
    }
    else
    {
      calibRect.draw();
    }
  }
}
