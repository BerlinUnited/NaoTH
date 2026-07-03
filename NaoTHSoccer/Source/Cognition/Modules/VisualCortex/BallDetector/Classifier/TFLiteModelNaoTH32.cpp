#include "TFLiteModelNaoTH32.h"
#include <iostream>
#include <vector>

void TFLiteModelNaoTH32::predict(const BallCandidates::PatchYUVClassified &patch, double meanBrightness)
{
    // create input data from patch (TODO: why not use a Y-patch directly and save the copy operation?)
    ASSERT(patch.size() == 16);


    // Copy patch data to input tensor
    //float* input = interpreter->typed_input_tensor<float>(0);

    // Ensure input tensor is properly accessed
    float* input = inputTensor->data.f;
    ASSERT(input != nullptr);

    int patchSize = patch.size();

    // Copy the input data Y-channel directly into the input tensor
    for (size_t x = 0; x < patchSize; x++)
    {
        for (size_t y = 0; y < patchSize; y++)
        {
            // Directly copy Y-channel data with brightness adjustment
            float value = (static_cast<float>(patch.data[patchSize * x + y].pixel.y) / 255.0f) + static_cast<float>(meanBrightness);
            input[patchSize * x + y] = value;
        }
    }

    // Perform inference
     MY_ASSERT_EQ(TfLiteInterpreterInvoke(interpreter), kTfLiteOk);

    // Retrieve the full output tensor
    const float* outputData = TfLiteInterpreterGetOutputTensor(interpreter, 0)->data.f;
    int numOutputElements = TfLiteTensorByteSize(TfLiteInterpreterGetOutputTensor(interpreter, 0)) / sizeof(float);
    result = std::vector<float>(outputData, outputData + numOutputElements);

    // Print the result
    // std::cout << "The result is: " << result[0] << ", " << result[1]  << std::endl;
}

double TFLiteModelNaoTH32::getBallConfidence() const
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_confidence) {
    return result[1];
  }
  return 1.0;
}

/** This is actually the "no ball" confidence */
double TFLiteModelNaoTH32::getRadius() const
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_radius) {
    return result[0];
  }

  return 1.0;
}

Vector2d TFLiteModelNaoTH32::getCenter() const
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_center) {
    return Vector2d(result[2], result[3]);
  }

  return Vector2d(0.5, 0.5);
}
