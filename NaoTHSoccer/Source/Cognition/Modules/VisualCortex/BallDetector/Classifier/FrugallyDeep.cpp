#include "FrugallyDeep.h"

// NOTE: assumes frugally 0.7.8-p0 is used

FrugallyDeep::FrugallyDeep(std::string file)
{
    model = std::make_shared<fdeep::model>(fdeep::load_model("Config/" + file));
}

FrugallyDeep::~FrugallyDeep()
{
}

void FrugallyDeep::predict(const BallCandidates::PatchYUVClassified &patch, double meanBrightness)
{
    // create input data from patch (TODO: why not use a Y-patch directly and save the copy operation?)
    ASSERT(patch.size() == 16);
    fdeep::tensor5 inputTensor = fdeep::tensor5(fdeep::shape5(1, 1, 16, 16, 1), 0);

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

Vector2d FrugallyDeep::getCenter()
{
    if (result.size() == 1)
    {
        float x = result[0].get(0, 0, 0, 0, 1);
        float y = result[0].get(0, 0, 0, 0, 2);
        return Vector2d(x, y);
    }
    else
    {
        return Vector2d(0.5, 0.5);
    }
}

double FrugallyDeep::getRadius()
{
    if (result.size() == 1)
    {
        return result[0].get(0, 0, 0, 0, 0);
    }
    else
    {
        return 0.0;
    }
}
