#include "FrugallyDeep.h"


void FrugallyDeep::predict(const BallCandidates::PatchYUVClassified &patch, double meanBrightness)
{
    // create input data from patch (TODO: why not use a Y-patch directly and save the copy operation?)
    ASSERT(patch.size() == 16);
    fdeep::tensor inputTensor = fdeep::tensor(fdeep::tensor_shape(16, 16, 1), 0);

    for (size_t x = 0; x < patch.size(); x++)
    {
        for (size_t y = 0; y < patch.size(); y++)
        {
            // TODO: check if x and y are correct
            // The average brightness should have value 0.0
            float value = (static_cast<float>((patch.data[patch.size() * x + y].pixel.y)) / 255.0f) - static_cast<float>(meanBrightness);
            inputTensor.set(0, 0, y, x, 0, value);
        }
    }

    result = model->predict({inputTensor});
}

double FrugallyDeep::getBallConfidence() const 
{
  ASSERT(result.size() >= 1);

  if(has_confidence) {
    return result[0].get(0, 0, 0, 0, 0);
  }
}

double FrugallyDeep::getRadius() const
{
  ASSERT(result.size() >= 1);

  // NOTE: radius value is the same as confidence
  if(has_radius) {
    return result[0].get(0, 0, 0, 0, 0);
  }

  return 0.0;
}

Vector2d FrugallyDeep::getCenter() const
{
  ASSERT(result.size() >= 3);

  if(has_center) {
    float x = result[0].get(0, 0, 0, 0, 1);
    float y = result[0].get(0, 0, 0, 0, 2);
    return Vector2d(x, y);
  }

  return Vector2d(0.5, 0.5);
}

