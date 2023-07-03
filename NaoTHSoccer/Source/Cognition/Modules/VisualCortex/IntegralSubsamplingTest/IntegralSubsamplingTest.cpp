/**
* @file IntegralSubsamplingTest.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*/

#include "IntegralSubsamplingTest.h"
#include <cmath>

IntegralSubsamplingTest::IntegralSubsamplingTest()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:IntegralSubsamplingTest:draw_grid", "", false);
  getDebugParameterList().add(&params);
}

IntegralSubsamplingTest::~IntegralSubsamplingTest()
{
  getDebugParameterList().remove(&params);
}

void IntegralSubsamplingTest::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  // Get the camera parameters of the camera image we run the detection on
  Parameters::cam_params cam_params = cameraID==CameraInfo::Top? params.top:
                                                                 params.bottom;

  // TODO: maybe rename getBallDetectorIntegralImage to IntegralImage
  // TODO: isValid necessary?
  if(!getBallDetectorIntegralImage().isValid()) {
    return;
  }

  const double width  = getImage().width();
  const double height = getImage().height();

  // calculate theoretical cell sizes
  const double cell_width  = width  / cam_params.column_count;
  const double cell_height = height / cam_params.row_count;

  if (std::min(cell_width, cell_height) < 1) {
      // cells are to small -> invalid parameters
      return;
  }

  // iterate columns
  Cell cell;
  for (double x = 0.0; x + cell_width-1 < width; x += cell_width)
  {
    // calculate column bounds - The right cell x coordinate (cell.maxX) is part of the cell area
    cell.min.x = static_cast<int>(x);
    cell.max.x = static_cast<int>(x + (cell_width-1)); // cell_width-1 because n * cell_width = width

    // iterate rows; scan from bottom to top until min_scan_y
    for(double y = height - 1; y - (cell_height-1) >= 0; y -= cell_height)
    {
      // calculate row bounds
      cell.max.y = static_cast<int>(y);
      cell.min.y = static_cast<int>(y - (cell_height-1));

      // check if the cell is mostly green
      double cell_green_density = cell.calc_green_density(getBallDetectorIntegralImage());
      
      //cell_green_density = Math::clamp(cell_green_density, 0.0, 1.0);

      DEBUG_REQUEST("Vision:IntegralSubsamplingTest:draw_grid",
        IMAGE_DRAWING_CONTEXT;
        CANVAS((cameraID==CameraInfo::Top? "ImageTop": "ImageBottom"));
        
        double v = 1.0 - cell_green_density;
        Color color(v,v,v,0.8);
        PEN(color, 1);
        FILLBOX(cell.min.x, cell.min.y, cell.max.x, cell.max.y);
        PEN(Color::black, 2);

        int percent = ((int)(cell_green_density*100 + 0.5));
        TEXT_DRAWING2( (cell.max.x + cell.min.x)/2, (cell.max.y + cell.min.y)/2, 0.025, percent );
      );
    }
  }

}
