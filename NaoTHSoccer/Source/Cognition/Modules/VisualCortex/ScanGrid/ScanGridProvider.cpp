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
  this->cameraID = id;

  if(!getCameraMatrix().valid) {
      return;
  }
  getScanGrid().reset();

  const int width = getImage().width();
  const int height = getImage().height();

  // calculate horizon
  int horizon_offset = 2;
  int max_horizon =
      std::max((int) (getArtificialHorizon().begin().y + horizon_offset), 0);
  int min_horizon =
      std::max((int) (getArtificialHorizon().end().y + horizon_offset), 0);

  if (min_horizon > max_horizon) {
    std::swap(min_horizon, max_horizon);
  }

  if (max_horizon >= height) {
    // camera doesn't point to the ground
    return;
  }

  Vector2i pointInImage;

  // project max_scan_distance_mm on the image plain
  // to obtain the upper limit of vertical scanlines
  Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x,
                                         getCameraMatrix().translation.y,
                                         0);
  Vector3d farthestPoint = cameraMatrixOffset +
      (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) *
       Vector3d(parameters.max_scan_distance_mm, 0, 0));

  bool projectionSucces = CameraGeometry::relativePointToImage(
        getCameraMatrix(), getCameraInfo(), farthestPoint, pointInImage);

  int min_scan_y;
  if(projectionSucces)
  {
    // should not be heigher than the top of the image
    min_scan_y = std::max(pointInImage.y, 0);
    // should not be lower than the bottom of the image
    if(min_scan_y >= height) {
      return;
    }
  } else {
    return;
  }
  // should not be over the horizon
  min_scan_y = std::max(min_scan_y, max_horizon);

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_vertical_min_y",
    LINE_PX(ColorClasses::black, 0, min_scan_y, width-1, min_scan_y);
  );

  double fieldWidthCovered = calculateMaxFieldWidth(min_scan_y);
  if (fieldWidthCovered <= 0) {
    return;
  }
  // calculate number of vertical scanlines with a horizontal_gap_mm
  int numberOfVerticals = std::min(
        (int) (fieldWidthCovered / parameters.horizontal_gap_mm),
        parameters.max_vertical_scanlines);

  double minGap = width / (double) numberOfVerticals;
  // transforms horizontal gap sizes into vertical gap sizes
  double gapRatio = parameters.vertical_gap_mm / parameters.horizontal_gap_mm;

  double focalLength = getCameraInfo().getFocalLength();
  double cameraHeight = getCameraMatrix().translation.z;

  int max_scan_y;
  double y = min_scan_y;
  double distance;
  std::vector<size_t> line_start_increasing_length;
  std::vector<double> v_gaps;

  for(double gap = minGap; gap < width; gap *= 2)
  {
    // distance of the gap if it would have
    // the size of vertical_gap_mm on the field
    distance = parameters.horizontal_gap_mm * focalLength / gap;

    // determine the start of the vertical scanline
    if(distance < cameraHeight) {
      max_scan_y = height-1;
    } else {
      distance = std::sqrt(distance * distance - cameraHeight * cameraHeight);
      // project gap distance onto the image to obtain the start of the scanline
      Vector3d pointOnField = cameraMatrixOffset +
          (RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle())
           * Vector3d(distance, 0, 0));

      bool projectionSucces = CameraGeometry::relativePointToImage(
            getCameraMatrix(), getCameraInfo(),
            pointOnField,
            pointInImage);
      if(!projectionSucces) {
        break;
      }
      max_scan_y = std::min(pointInImage.y, height-1);
      if (max_scan_y <= min_scan_y) {
        continue;
      }
    }
    // fill vertical scan pattern
    double ySkip = std::max(gapRatio * gap,
                            (double) parameters.min_vertical_gap_px);
    while(y <= max_scan_y)
    {
      getScanGrid().vScanPattern.push_back((int) y);
      y += ySkip;
    }

    // index of vScanPattern there the line starts
    size_t bottom_idx = getScanGrid().vScanPattern.size()-1;
    line_start_increasing_length.push_back(bottom_idx);
    // vertical gap sizes
    v_gaps.push_back(gap);

    if(max_scan_y >= height-1) {
      // bottom of the image reached
      break;
    }
  }

  if(line_start_increasing_length.empty() || getScanGrid().vScanPattern.empty()) {
    return;
  }

  // fill the image with vertical scanlines
  getScanGrid().vertical.resize(numberOfVerticals);

  std::vector<size_t>::iterator line_start_itr =
      line_start_increasing_length.begin();

  int frequency;
  for(int i=1; i<=numberOfVerticals; i*=2) {
    frequency = i*2;

    for(int j=i-1; j<numberOfVerticals; j+=frequency) {
      int x = (int) (j*minGap);
      ScanGrid::VScanLine scanline;
      scanline.x = x;
      scanline.bottom = *line_start_itr;
      scanline.top = 0;

      getScanGrid().vertical[j] = scanline;

      //if(next(line) == linesIncreasingLength.end()) {
      //  getScanGrid().longverticals.push_back(j);
      //}
    }
    if(next(line_start_itr) != line_start_increasing_length.end()){
      ++line_start_itr;
    }
  }

  // calculate approximate number of horizontals with the current v_gaps
  line_start_itr = line_start_increasing_length.begin();
  double last_y = min_scan_y;
  double n_horizontals = 0;
  for(double v_gap : v_gaps) {
    y = getScanGrid().vScanPattern[*line_start_itr];
    n_horizontals += (y - last_y) / v_gap;
    last_y = y;

    ++line_start_itr;
  }

  // adjust v_gaps if too much horizontals
  if (n_horizontals > parameters.max_horizontal_scanlines) {
    double gap_modifier = n_horizontals / parameters.max_horizontal_scanlines;
    for(double& v_gap : v_gaps) {
      v_gap *= gap_modifier;
    }
  }

  getScanGrid().horizontal.reserve(parameters.max_horizontal_scanlines);

  // fill the image with horizontal scanlines
  size_t i = 0;
  y = min_scan_y;
  double h_skip = minGap;
  while(y < height) {
    ScanGrid::HScanLine horizontal;
    horizontal.left_x = 0;
    horizontal.right_x = width-1;
    horizontal.y = (int) y;
    horizontal.skip = std::max((int) h_skip, parameters.min_horizontal_gap_px);

    getScanGrid().horizontal.push_back(horizontal);

    double v_gap = v_gaps[i];
    size_t h_line_start = line_start_increasing_length[i];

    if(y + v_gap > getScanGrid().vScanPattern[h_line_start]) {
      if(++i >= v_gaps.size()) {
        break;
      }
      // adjust gaps
      v_gap = v_gaps[i];
      h_line_start = line_start_increasing_length[i];
      h_skip *= 2;
    }
    y += v_gap;
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
      LINE_PX(ColorClasses::blue, scanline.left_x, scanline.y, scanline.right_x, scanline.y);
      for(int x=scanline.left_x; x<=scanline.right_x; x+=scanline.skip)
      {
        POINT_PX(ColorClasses::red, x, scanline.y);

        //Test
        getImage().getY(x, scanline.y);
      }
    }
  );

}//end execute


double ScanGridProvider::calculateMaxFieldWidth(int y) {
  // project the left and right image corner onto the field
  // and caluclate the distance between thems
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
