#include "ScanGridProvider.h"

ScanGridProvider::ScanGridProvider()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_verticals", "draw vertical scanlines", false);
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
    min_scan_y = pointInImage.y;
  } else {
    min_scan_y = min_horizon;
  }
  int min_horizontal = std::max(min_scan_y, max_horizon);
  // calculate the number of vertical scanlines
  double fieldWidthCovered = calculateMaxFieldWidth(min_horizontal);
  if (fieldWidthCovered < 0) {
    return;
  }
  int numberOfVerticals = (int) (fieldWidthCovered / parameters.vertical_gap_mm);
  if (numberOfVerticals > parameters.max_vertical_scanlines)
  {
    numberOfVerticals = parameters.max_vertical_scanlines;
  }

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
  auto line = linesIncreasingLength.begin();
  int frequency;
  for(int i=1; i<=numberOfVerticals; i*=2) {
    frequency = i*2;

    for(int j=i-1; j<numberOfVerticals; j+=frequency) {
      int x = (int) (j*minGap);
      ScanGrid::VScanLine scanline;
      scanline.x = x;
      scanline.bottom = (*line).bottom;
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

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_verticals",
    for(const ScanGrid::VScanLine line: getScanGrid().vertical)
    {
      LINE_PX(ColorClasses::blue, line.x, getScanGrid().vScanPattern[line.bottom], line.x, getScanGrid().vScanPattern[line.top]);
      for(size_t i=line.top; i<line.bottom+1; ++i)
      {
        POINT_PX(ColorClasses::red, line.x, getScanGrid().vScanPattern[i]);
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
