#include "BallPatchDetector.h"

using namespace naoth;
using namespace std;

// BallPatchDetector searches the ball detector integral image for candidate ball
// patches and ranks them by a simple white-content quality score. The output is
// a sorted BestPatchList of non-overlapping patches that are likely ball regions.
BallPatchDetector::BallPatchDetector() : cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:draw_value", "", false);
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:drawPatches", "draw ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:drawPatches_px", "draw ball key points in the raw image", false);

  getDebugParameterList().add(&params);
}

BallPatchDetector::~BallPatchDetector() 
{
  getDebugParameterList().remove(&params);
}

void BallPatchDetector::execute(const CameraInfo::CameraID id) 
{
  cameraID = id;
  getBestPatchList().clear();

  // Compute candidate ball patches using the faster scan path.
  calculateKeyPointsFast(getBallDetectorIntegralImage(), getBestPatchList());

  // Debug drawing: show candidate patch boxes in the selected camera image.
  DEBUG_REQUEST("Vision:BallPatchDetector:drawPatches",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    PEN("FF0000", 1);
    for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
      BOX((*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );

  DEBUG_REQUEST("Vision:BallPatchDetector:drawPatches_px",
    for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
      RECT_PX(ColorClasses::red, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );
}

template<class ImageType>
void BallPatchDetector::calculateKeyPoints(const ImageType& integralImage, BestPatchList& best) const
{
  // STEP I: verify that the field polygon is known and we have a valid scan area.
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty or invalid.
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // Integral images are stored at reduced resolution, so image coordinates must be
  // scaled by FACTOR to convert between integral-image pixels and original pixels.
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2<unsigned int> point;

  for(point.y = minY/FACTOR; point.y < integralImage.getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
      static_cast<int>(point.x)*FACTOR, static_cast<int>(point.y)*FACTOR);

    double radius = std::max(6.0, estimatedRadius);
    unsigned int size   = (unsigned int)(radius*2.0/FACTOR+0.5);
    unsigned int border = (unsigned int)(radius*params.borderRadiusFactorClose/FACTOR+0.5);

    // HACK: different parameters depending on size
    if(size < 40/FACTOR) {
      border = (unsigned int)(radius*params.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = std::max( 2u, border);

    // smallest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border >= integralImage.getHeight()) {
      continue;
    }

    for(point.x = border; point.x+size+border < integralImage.getWidth(); ++point.x)
    {
      evaluatePatch(integralImage, best, point, size, border);
    }
  }
}


template<class ImageType>
void BallPatchDetector::calculateKeyPointsFast(const ImageType& integralImage, BestPatchList& best) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2<unsigned int> point;

  const unsigned int height = integralImage.getHeight();
  const unsigned int width = integralImage.getWidth();

  for(point.y = minY/FACTOR; point.y < height; ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
      static_cast<int>(getImage().width()/2), static_cast<int>(point.y)*FACTOR);

    estimatedRadius = estimatedRadius / FACTOR + 0.5;
    // Note: we have a minimal allowed radius
    unsigned int radius = (estimatedRadius < 2.0) ? 2 : static_cast<unsigned int>(estimatedRadius);

    // smallest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y < radius || point.y + radius >= height) {
      continue;
    }

    for(point.x = radius; point.x + radius < width; ++point.x)
    {
      // Skip points that lie on the robot's body contour for the bottom camera.
      if(cameraID == CameraInfo::Bottom && //point.y+radius >(int)(integralImage.getHeight()/2) &&
         getBodyContour().isOccupied(point.x*integralImage.FACTOR, (point.y+radius)*integralImage.FACTOR)) {
        continue;
      }

      // Evaluate the candidate ball patch by measuring the amount of white in the
      // square centered at this point and by checking the green density near the center.
      const unsigned int innerOffset = radius/2;
      const unsigned int area = 4*radius*radius;

      // the zero index for the color channel means non green pixels
      unsigned int inner = integralImage.getSumForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 0);
      // the 1 index for the color channel means green pixels
      double greenInner  = integralImage.getDensityForRect(point.x-innerOffset, point.y-innerOffset, point.x+innerOffset, point.y+innerOffset, 1);

      unsigned int below = 0;
      unsigned int radius_below = radius*2;

      if(point.y+radius_below < height) {
        // get number of non green pixels
        below = integralImage.getSumForRect(point.x-radius, point.y+radius, point.x+radius, point.y+radius_below, 0);
      }

      if (inner*2 > area && greenInner <= params.maxInnerGreenDensitiy && below*params.area_below_factor < area)
      {
        // value is the ratio of non white pixels in a given area
        double value = ((double)inner)/((double)(area));
        // FIXME add removes overlapping patches directly so we can only log patches that survive the overlapping removal
        // we should be able to save all the patches with the value so we can check later what value the actual best patch has
        best.add(
            static_cast<int>(point.x-radius)*integralImage.FACTOR,
            static_cast<int>(point.y-radius)*integralImage.FACTOR,
            static_cast<int>(point.x+radius)*integralImage.FACTOR,
            static_cast<int>(point.y+radius)*integralImage.FACTOR,
            value);
      }

      DEBUG_REQUEST("Vision:BallPatchDetector:draw_value",
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        double value = ((double)inner)/((double)(area));

        value = Math::clamp(value / 200.0, 0.0,1.0);
        PEN(Color(1.0,1.0-value,1.0-value,0.8),0.1);

        FILLBOX((point.x)*integralImage.FACTOR - integralImage.FACTOR/2,
                (point.y)*integralImage.FACTOR - integralImage.FACTOR/2,
                (point.x)*integralImage.FACTOR + integralImage.FACTOR/2,
                (point.y)*integralImage.FACTOR + integralImage.FACTOR/2);
      );
    }
  }
}


template<class ImageType>
void BallPatchDetector::calculateKeyPointsFull(const ImageType& integralImage, BestPatchList& best) const
{
  // find the maximal height minY to be scanned in the image
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  // TODO is this really necessary - we already have the valid check?? 
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // TODO: needs a better place
  // Integral images are stored at reduced resolution, so image coordinates must be
  // scaled by FACTOR to convert between integral-image pixels and original pixels.
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2i point;
  // TODO: this has to be made more general
  // Reset the temporary per-pixel candidate values array. This array stores a
  // score and radius for each potential ball center during the full search.
  for(int y = 1; y+1 < 480/4; ++y) {
    for(int x = 1; x+1 < 640/4; ++x) {
      values[x][y][0] = 0.0;
    }
  }

  // TODO: faster reset?
  //std::fill_n(&values[0][0][0], (480/4)*(640/4)*2, 0);

  for(point.y = minY/FACTOR; point.y < (int)integralImage.getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
      getImage().width()/2, point.y*FACTOR);


    int radius = (int)(estimatedRadius / FACTOR + 0.5);

    // smallest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y < radius || point.y + radius >= (int)integralImage.getHeight()) {
      continue;
    }

    for(point.x = radius; point.x+radius < (int)integralImage.getWidth(); ++point.x)
    {
      //evaluatePatch(integralImage, best, point, size, border);

      if(cameraID == CameraInfo::Bottom && getBodyContour().isOccupied(point.x*integralImage.FACTOR, (point.y+radius)*integralImage.FACTOR)) {
        values[point.x][point.y][0] = 0.0;
        values[point.x][point.y][1] = radius;
        continue;
      }

      int inner = integralImage.getSumForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 0);
      double greeInner = integralImage.getDensityForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 1);
      const int size = radius*2;

      if (inner*2 > size*size && greeInner <= params.maxInnerGreenDensitiy)
      {
        double value = ((double)inner)/((double)(size)*(size));
        values[point.x][point.y][0] = value;
        values[point.x][point.y][1] = radius;
      }
      else
      {
        values[point.x][point.y][0] = 0.0;
        values[point.x][point.y][1] = radius;
      }

      DEBUG_REQUEST("Vision:BallPatchDetector:draw_value",
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        double value = ((double)inner)/((double)(size*size));

        value = Math::clamp(value / 200.0, 0.0,1.0);
        PEN(Color(1.0,1.0-value,1.0-value,0.8),0.1);

        FILLBOX((point.x)*integralImage.FACTOR - integralImage.FACTOR/2,
                (point.y)*integralImage.FACTOR - integralImage.FACTOR/2,
                (point.x)*integralImage.FACTOR + integralImage.FACTOR/2,
                (point.y)*integralImage.FACTOR + integralImage.FACTOR/2);
      );
    }
  }


  // Perform non-maximum suppression on the candidate score map. Only local
  // peaks are turned into final patch proposals.
  for(int y = 1; y+1 < 480/4; ++y) {
    for(int x = 1; x+1 < 640/4; ++x) {
      if(values[x][y][0] > 0) {
        if( values[x][y][0] > values[x-1][y][0] &&
            values[x][y][0] > values[x+1][y][0] &&
            values[x][y][0] > values[x  ][y-1][0] &&
            values[x][y][0] > values[x  ][y+1][0] &&

            values[x][y][0] > values[x-1][y-1][0] &&
            values[x][y][0] > values[x+1][y-1][0] &&
            values[x][y][0] > values[x-1][y+1][0] &&
            values[x][y][0] > values[x+1][y+1][0])
        {
          double value = values[x][y][0];
          int rad = (int)values[x][y][1];
          best.add(
            (x-rad)*integralImage.FACTOR,
            (y-rad)*integralImage.FACTOR,
            (x+rad)*integralImage.FACTOR,
            (y+rad)*integralImage.FACTOR,
            value);
        }
      }
    }
  }
}

// Re-evaluate a previously found patch at multiple inner sizes and choose the
// best sub-region. This is used to refine a coarse candidate before ball fitting.
BestPatchList::Patch BallPatchDetector::refineKeyPoint(const BestPatchList::Patch& patch) const
{
  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;


  Vector2<unsigned int> min = patch.min / FACTOR;
  Vector2<unsigned int> max = patch.max / FACTOR;

  // enlarge the search area
  unsigned int outer_size = max.x - min.x;
  //min.x -= outer_size/4;
  //min.y -= outer_size/4;
  //max.x += outer_size/4;
  //max.y += outer_size/4;

  BestPatchList::Patch maxPatch;

  Vector2<unsigned int> point;

  // iterate different inner size
  for(unsigned int inner_size = outer_size/2; inner_size + inner_size/2 < outer_size; ++inner_size)
  {
    unsigned int border = inner_size/4;

    for(point.y = min.y+border; point.y + border + inner_size < max.y; ++point.y)
    {
      for(point.x = min.x+border; point.x + border + inner_size < max.x; ++point.x)
      {
        unsigned int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+inner_size, point.y+inner_size, 0);
        double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+inner_size, point.x+inner_size, point.y+inner_size+border, 1);

        if (inner*2 > inner_size*inner_size && greenBelow > 0.3)
        {
          unsigned int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+inner_size, point.x+inner_size+border, point.y+inner_size+border, 0);
          double value = (double)(inner - (outer - inner))/((double)(inner_size+border)*(inner_size+border));

          // scale the patch up to the image coordinates
          if(maxPatch.value == -1 || value > maxPatch.value)
          {
            maxPatch.min.x = static_cast<int>(point.x-border);
            maxPatch.min.y = static_cast<int>(point.y-border);
            maxPatch.max.x = static_cast<int>(point.x+inner_size+border);
            maxPatch.max.y = static_cast<int>(point.y+inner_size+border);
            maxPatch.value = value;
          }
        }
      }
    }
  }

  maxPatch.min *= FACTOR;
  maxPatch.max *= FACTOR;

  return maxPatch;
}

