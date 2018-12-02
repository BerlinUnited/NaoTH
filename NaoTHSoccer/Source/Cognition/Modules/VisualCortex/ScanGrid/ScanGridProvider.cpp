#include "ScanGridProvider.h"

ScanGridProvider::ScanGridProvider()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_verticals", "draw vertical scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_horizontals", "draw horizontal scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_vertical_min_y", "", false);
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

  const int width = getImage().width();
  const int height = getImage().height();

  // calculate horizon
  int horizon_offset = 2;
  int max_horizon = std::max((int) (getArtificialHorizon().begin().y + horizon_offset), 0);
  int min_horizon = std::max((int) (getArtificialHorizon().end().y + horizon_offset), 0);
  if (min_horizon > max_horizon) {
    std::swap(min_horizon, max_horizon);
  }
  if (max_horizon >= height) {
    // camera doesn't point to the ground
    return;
  }

  Vector2i pointInImage;
  int min_scan_y;
  // project max_scan_distance_mm on the image plain to obtain the upper limit of vertical scanlines
  Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
  if(CameraGeometry::relativePointToImage(
       getCameraMatrix(), getCameraInfo(),
       (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) * Vector3d(parameters.max_scan_distance_mm, 0, 0))
        + cameraMatrixOffset,
       pointInImage)
    )
  {
    min_scan_y = std::max(pointInImage.y, 0);
    if(min_scan_y >= height-1) {
      return;
    }
  } else {
    return;
  }
  min_scan_y = std::max(min_scan_y, max_horizon);

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_vertical_min_y",
    LINE_PX(ColorClasses::black, 0, min_scan_y, width-1, min_scan_y);
  );

  double fieldWidthCovered = calculateMaxFieldWidth(min_scan_y);
  if (fieldWidthCovered <= 0) {
    return;
  }
  // calculate number of vertical scanlines with a vertical_gap_mm
  int numberOfVerticals = std::min((int) (fieldWidthCovered / parameters.vertical_gap_mm), parameters.max_vertical_scanlines);

  double minGap = width / (double) numberOfVerticals;
  double gapRatio = parameters.horizontal_gap_mm / parameters.vertical_gap_mm;

  int max_scan_y;
  double y = min_scan_y;
  double distance;
  std::vector<ScanGrid::VScanLine> linesIncreasingLength;

  for(double gap=2*minGap; gap < width; gap*=2)
  {
    // determine the lower bound of the vertical scanline
    // distance of gap if it would be the size of vertical_gap_mm
    distance = parameters.vertical_gap_mm * getCameraInfo().getFocalLength() / gap;
    if(distance < getCameraMatrix().translation.z) {
      max_scan_y = height-1;
    } else {
      distance = std::sqrt(distance*distance - getCameraMatrix().translation.z*getCameraMatrix().translation.z);
      Vector3d pointOnField = (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle())
                              * Vector3d(distance, 0, 0))
                              + cameraMatrixOffset;
      if(!CameraGeometry::relativePointToImage(
        getCameraMatrix(), getCameraInfo(),
        pointOnField,
        pointInImage)) {
        break;
      }
      max_scan_y = std::min(pointInImage.y, height-1);
      if (max_scan_y <= min_scan_y) {
        continue;
      }
    }
    // fill vertical scan pattern
    double ySkip = std::max(gapRatio * gap/2, parameters.min_horizontal_gap_px);
    for(;y <= max_scan_y; y+=ySkip)
    {
      getScanGrid().vScanPattern.push_back((int) y);
    }

    // scanline with vScanPattern index
    ScanGrid::VScanLine scanline;
    scanline.top = 0;
    scanline.bottom = getScanGrid().vScanPattern.size()-1;
    linesIncreasingLength.push_back(scanline);
    if(max_scan_y >= height-1) {
      break;
    }
  }
  if(linesIncreasingLength.empty() || getScanGrid().vScanPattern.empty()) {
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
      LINE_PX(ColorClasses::blue, scanline.x, getScanGrid().vScanPattern.at(scanline.bottom), scanline.x, getScanGrid().vScanPattern.at(scanline.top));
      for(size_t i=scanline.top; i<=scanline.bottom; ++i)
      {
        POINT_PX(ColorClasses::red, scanline.x, getScanGrid().vScanPattern.at(i));

        //Test
        const int xx = scanline.x;
        const int yy = getScanGrid().vScanPattern.at(i);
        getImage().getY(xx, yy);
      }
    }
  );

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_horizontals",
    for(const ScanGrid::HScanLine& scanline: getScanGrid().horizontal)
    {
      LINE_PX(ColorClasses::blue, getScanGrid().hScanPattern.at(scanline.left), scanline.y, getScanGrid().hScanPattern.at(scanline.right), scanline.y);
      for(size_t i=scanline.left; i<=scanline.right; i+=scanline.skip)
      {
        POINT_PX(ColorClasses::red, getScanGrid().hScanPattern.at(i), scanline.y);

        //Test
        const int xx = getScanGrid().hScanPattern.at(i);
        const int yy = scanline.y;
        getImage().getY(xx, yy);
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
