#include "ScanGridProvider.h"

ScanGridProvider::ScanGridProvider()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_verticals", "draw vertical scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_horizontals", "draw horizontal scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_vertical_min_y", "", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridProvider:draw_v_scan_pattern", "", false);
  getDebugParameterList().add(&params);
}

ScanGridProvider::~ScanGridProvider()
{
  getDebugParameterList().remove(&params);
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
       Vector3d(params.max_scan_distance_mm, 0, 0));

  if(!CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(),
                                           farthestPoint, pointInImage))
  {
    // projection failed
    return;
  }

  // should be under the horizon and not higher than top of the image
  int min_scan_y = std::max(pointInImage.y, max_horizon);
  // should not be lower than the bottom of the image
  if(min_scan_y >= height) {
    return;
  }

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_vertical_min_y",
    LINE_PX(ColorClasses::black, 0, min_scan_y, width-1, min_scan_y);
  );

  double fieldWidthCovered = calculateMaxFieldWidth(min_scan_y);
  if (fieldWidthCovered <= 0) {
    return;
  }
  // calculate the number of vertical scanlines at min_scan_y,
  // so the gap at min_scan_y is horizontal_gap_mm
  int numberOfVerticals = std::min(
        (int) (fieldWidthCovered / params.horizontal_gap_mm),
        params.max_vertical_scanlines);

  // calculate vertical scan pattern by creating points
  // on the field and projecting them back onto the image
  calculate_vertical_scan_pattern(min_scan_y);
  if(getScanGrid().vScanPattern.empty()) {
    return;
  }

  double minGap = width / (double) numberOfVerticals;

  double focalLength = getCameraInfo().getFocalLength();
  double cameraHeight = getCameraMatrix().translation.z;

  int max_scan_y;
  double distance;
  std::vector<size_t> line_start_increasing_length;
  std::vector<double> v_gaps;

  double gap_modifier = params.vertical_gap_mm / params.horizontal_gap_mm;

  int vScan_idx = static_cast<int>(getScanGrid().vScanPattern.size()) - 1;
  
  for(double gap = minGap; gap < width; gap *= 2)
  {
    // distance to an object with a size of "gap" in the image,
    // if it would have a size of "horizontal_gap_mm" on the field
    distance = params.horizontal_gap_mm * focalLength / gap;

    // determine the start of the vertical scanline
    if(distance < cameraHeight) {
      max_scan_y = height-1;
    } else {
      // project distance onto the ground plane
      distance = std::sqrt(distance * distance - cameraHeight * cameraHeight);
      // project point in the distance onto the image,
      // to obtain the start of the scanline
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
    // determine start in vertical scan pattern,
    // iterate backwards because most scanlines are short
    size_t bottom_idx = 0;
    for(; vScan_idx >= 0; vScan_idx--)
    {
      int y = getScanGrid().vScanPattern[static_cast<size_t>(vScan_idx)];
      if(y > max_scan_y) {
        bottom_idx = std::min(getScanGrid().vScanPattern.size()-1, static_cast<size_t>(vScan_idx)+1);
        break;
      }
    }

    line_start_increasing_length.push_back(bottom_idx);

    // vertical gap sizes
    v_gaps.push_back(gap_modifier * gap);

    if(max_scan_y >= height-1) {
      // bottom of the image reached
      break;
    }
  }

  if(line_start_increasing_length.empty()) {
    return;
  }

  // set all lines to the same lenghts
  if(params.uniform_vertical_lengths) {
    std::fill(line_start_increasing_length.begin(), line_start_increasing_length.end(), line_start_increasing_length.back());
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
      scanline.top = getScanGrid().vScanPattern.size()-1;

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
    double y = getScanGrid().vScanPattern[*line_start_itr];
    n_horizontals += (y - last_y) / v_gap;
    last_y = y;

    ++line_start_itr;
  }

  // adjust v_gaps if too many horizontals
  if (n_horizontals > params.max_horizontal_scanlines) {
    gap_modifier = n_horizontals / params.max_horizontal_scanlines;
    for(double& v_gap : v_gaps) {
      v_gap *= gap_modifier;
    }
  }

  getScanGrid().horizontal.reserve(params.max_horizontal_scanlines);

  gap_modifier = params.h_field_scan_rate_mm / params.horizontal_gap_mm;

  // fill the image with horizontal scanlines
  size_t i = 0;
  double y = min_scan_y;
  double h_skip = gap_modifier * minGap;
  while(y < height) {
    ScanGrid::HScanLine horizontal;
    horizontal.left_x = 0;
    horizontal.right_x = width-1;
    horizontal.y = (int) y;
    horizontal.skip = std::max((int) h_skip, params.min_horizontal_gap_px);

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
      h_skip = gap_modifier * v_gap;
    }
    y += v_gap;
  }

  DEBUG_REQUEST("Vision:ScanGridProvider:draw_verticals",
    for(const ScanGrid::VScanLine& scanline: getScanGrid().vertical)
    {
      LINE_PX(ColorClasses::blue,
              scanline.x, getScanGrid().vScanPattern.at(scanline.bottom),
              scanline.x, getScanGrid().vScanPattern.at(scanline.top));
      for(size_t ii=scanline.bottom; ii<=scanline.top; ++ii)
      {
        POINT_PX(ColorClasses::red, scanline.x, getScanGrid().vScanPattern.at(ii));

        //Test
        const int xx = scanline.x;
        const int yy = getScanGrid().vScanPattern.at(ii);
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


double ScanGridProvider::calculateMaxFieldWidth(int y)
{
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

bool ScanGridProvider::calculate_vertical_scan_pattern(int min_scan_y)
{
  std::vector<int>& scan_pattern = getScanGrid().vScanPattern;
  //std::vector<int> scan_pattern;

  const double width = getImage().width();
  const double height = getImage().height();

  // get depicted field point at the bottom of the image
  Vector2d field_point;
  if(!CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(), getCameraInfo(),
    width/2,
    height,
    0.0,
    field_point))
  {
    return false;
  }
  Vector3d field_point_3d(field_point.x, field_point.y, 0);

  // camera direction
  Vector3d direction_3d = RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) *
                          Vector3d(params.v_field_scan_rate_mm, 0, 0);

  // create scan points starting from the bottom of the image
  int y = getImage().height()-1;
  Vector2i image_point;
  bool skip_to_small = false;
  while(y > min_scan_y)
  {
    DEBUG_REQUEST("Vision:ScanGridProvider:draw_v_scan_pattern",
      POINT_PX(ColorClasses::red, getImage().width()/2, y);
    );
    scan_pattern.push_back(y);

    if(!skip_to_small) {
      field_point_3d += direction_3d;

      bool projectionSucces = CameraGeometry::relativePointToImage(
            getCameraMatrix(), getCameraInfo(),
            field_point_3d,
            image_point);
      if(!projectionSucces) {
        return false;
      }

      if(y - image_point.y < params.min_vertical_gap_px) {
        y -= params.min_vertical_gap_px;
        skip_to_small = true;
      } else {
        y = image_point.y;
      }
    } else {
      y -= params.min_vertical_gap_px;
    }
  }
  return true;
}
