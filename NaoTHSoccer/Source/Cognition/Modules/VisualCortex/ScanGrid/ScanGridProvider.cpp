#include "ScanGridProvider.h"

ScanGridProvider::ScanGridProvider()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_verticals", "draw vertical scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_horizontals", "draw horizontal scanlines", false);
  getDebugParameterList().add(&parameters);
}

ScanGridProvider::~ScanGridProvider()
{
  getDebugParameterList().remove(&parameters);
}

void ScanGridProvider::execute(CameraInfo::CameraID id)
{
  if(!getCameraMatrix().valid) {
      return;
  }
  this->cameraID = id;
  getScanGrid().reset();

  int max_horizon = (int) std::max(getArtificialHorizon().begin().y + 1., 0.);
  int min_horizon = (int) std::max(getArtificialHorizon().end().y + 1., 0.);
  if (min_horizon > max_horizon) {
    std::swap(min_horizon, max_horizon);
  }

  Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);

  Vector2i pointInImage;
  int min_scan_y;
  if(CameraGeometry::relativePointToImage(
       getCameraMatrix(), getCameraInfo(),
       (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) * Vector3d(parameters.max_scan_distance_mm, 0, 0))
        + cameraMatrixOffset,
       pointInImage)
    )
  {
    min_scan_y = std::max(pointInImage.y, 0);
  } else {
    min_scan_y = min_horizon;
  }
  int min_horizontal = std::max(min_scan_y, max_horizon);
  // calculate the number of vertical scanlines
  double fieldWidthCovered = calculateMaxFieldWidth(min_horizontal);
  if (fieldWidthCovered < 0) {
    return;
  }
  int numberOfVerticals = std::min((int) (fieldWidthCovered / parameters.vertical_gap_mm), parameters.max_vertical_scanlines);

  int width = getImage().width();
  int height = getImage().height();

  double minGap = getImage().width() / (double) numberOfVerticals;
  double gapRatio = parameters.horizontal_gap_mm / parameters.vertical_gap_mm;

  int vScanEnd;
  double ySkip = parameters.min_horizontal_gap_px;
  double y = min_scan_y;
  double distance;
  std::vector<ScanGrid::VScanLine> linesIncreasingLength;

  for(double gap=2*minGap; gap < width; gap*=2)
  {
    // distance of gap if it would be the size of vertical_gap_mm
    distance = parameters.vertical_gap_mm * getCameraInfo().getFocalLength() / gap;
    if(distance < getCameraMatrix().translation.z) {
      vScanEnd = height-1;
    } else {
      distance = std::sqrt(distance*distance - getCameraMatrix().translation.z*getCameraMatrix().translation.z);
      Vector3d pointOnField = (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle())
                              * Vector3d(distance, 0, 0))
                              + cameraMatrixOffset;
      if(!CameraGeometry::relativePointToImage(
        getCameraMatrix(), getCameraInfo(),
        pointOnField,
        pointInImage)
        ) break;
      vScanEnd = std::max(min_scan_y, pointInImage.y);
      vScanEnd = std::min(vScanEnd, height-1);
      if(std::abs(y-vScanEnd) < 1) {
        continue;
      }
    }
    // fill vertical scan pattern
    ySkip = std::max(gapRatio * gap/2, parameters.min_horizontal_gap_px);
    for(;y <= vScanEnd; y+=ySkip)
    {
      getScanGrid().vScanPattern.push_back((int) y);
    }

    // scanline with vScanPattern index
    ScanGrid::VScanLine scanline;
    scanline.top = 0;
    scanline.bottom = getScanGrid().vScanPattern.size()-1;
    linesIncreasingLength.push_back(scanline);
    if(vScanEnd >= height-1) {
      break;
    }
  }
  if(linesIncreasingLength.empty()) {
    return;
  }

  getScanGrid().vertical.resize(numberOfVerticals);
  std::vector<ScanGrid::VScanLine>::iterator line = linesIncreasingLength.begin();
  int frequency;
  for(int i=1; i<=numberOfVerticals; i*=2) {
    frequency = i*2;

    for(int j=i-1; j<numberOfVerticals; j+=frequency) {
      int x = (int) (j*minGap);
      ScanGrid::VScanLine scanline;
      scanline.x = x;
      scanline.bottom = line->bottom;
      scanline.top = 0;
      /*
      int horizon = (int) getArtificialHorizon().projection(Vector2d(x,0)).y + 1;
      for(;scanline.top < getScanGrid().vScanPattern.size(); ++scanline.top) {
        if(getScanGrid().vScanPattern[scanline.top] >= horizon) {
          break;
        }
      }
      if(scanline.top > scanline.bottom) {
        scanline.bottom = scanline.top;
      }*/
      getScanGrid().vertical[j] = scanline;

      if(next(line) == linesIncreasingLength.end()) {
        getScanGrid().longverticals.push_back(j);
      }
    }
    if(next(line) != linesIncreasingLength.end()){
      ++line;
    }
  }

  // fill horizontal scan pattern
  for (const ScanGrid::VScanLine& vertical : getScanGrid().vertical) {
    getScanGrid().hScanPattern.push_back(vertical.x);
  }

  // create horizontal scanlines
  getScanGrid().horizontal.resize(getScanGrid().vScanPattern.size());
  line = linesIncreasingLength.begin();
  int hSkip = 1;
  int i=0;
  for(const int& y : getScanGrid().vScanPattern) {
    if(y > getScanGrid().vScanPattern[line->bottom]) {
      ++line;
      hSkip *= 2;
    }
    ScanGrid::HScanLine horizontal;
    horizontal.left = 0;
    horizontal.right = getScanGrid().hScanPattern.size()-1;
    horizontal.y = y;
    horizontal.skip = hSkip;

    getScanGrid().horizontal[i] = horizontal;
    ++i;
  }

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_verticals",
    for(const ScanGrid::VScanLine& scanline: getScanGrid().vertical)
    {
      LINE_PX(ColorClasses::blue, scanline.x, getScanGrid().vScanPattern[scanline.bottom], scanline.x, getScanGrid().vScanPattern[scanline.top]);
      for(size_t i=scanline.top; i<scanline.bottom+1; ++i)
      {
        POINT_PX(ColorClasses::red, scanline.x, getScanGrid().vScanPattern[i]);

        //Test
        Pixel pixel;
        getImage().get(scanline.x, getScanGrid().vScanPattern[i], pixel);
      }
    }
  );

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_horizontals",
    for(const ScanGrid::HScanLine& scanline: getScanGrid().horizontal)
    {
      LINE_PX(ColorClasses::blue, getScanGrid().hScanPattern[scanline.left], scanline.y, getScanGrid().hScanPattern[scanline.right], scanline.y);
      for(size_t i=scanline.left; i<scanline.right+1; i+=scanline.skip)
      {
        POINT_PX(ColorClasses::red, getScanGrid().hScanPattern[i], scanline.y);

        //Test
        Pixel pixel;
        getImage().get(getScanGrid().hScanPattern[i], scanline.y, pixel);
      }
    }
  );

}//end execute


double ScanGridProvider::calculateMaxFieldWidth(int y) {
  Vector2d projectedLeftCorner;
  if(!CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(), getCameraInfo(),
    0,
    y,
    0.0,
    projectedLeftCorner))
  {
    return -1.;
  }
  Vector2d projectedRightCorner;
  if(!CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(), getCameraInfo(),
    getImage().width()-1,
    y,
    0.0,
    projectedRightCorner))
  {
    return -1.;
  }
  return (projectedLeftCorner - projectedRightCorner).abs();
}
