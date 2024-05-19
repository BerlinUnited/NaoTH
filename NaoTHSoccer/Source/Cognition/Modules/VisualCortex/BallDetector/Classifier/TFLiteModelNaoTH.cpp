#include "TFLiteModelNaoTH.h"
#include <iostream>
#include <vector>

int getTensorSize(const TfLiteTensor* tensor) {
    // Initialize the size to 1 (multiplicative identity)
    int size = 1;

    // Multiply the dimensions to compute the total number of elements
    for (int i = 0; i < tensor->dims->size; ++i) {
        size *= tensor->dims->data[i];
    }

    return size;
}

void TFLiteModelNaoTH::predict(const BallCandidates::PatchYUVClassified &patch, double meanBrightness)
{
    // create input data from patch (TODO: why not use a Y-patch directly and save the copy operation?)
    ASSERT(patch.size() == 16);
      

    // Copy patch data to input tensor
    //float* input = interpreter->typed_input_tensor<float>(0);
	
    int patchSize = patch.size();

     for (size_t x = 0; x < patchSize; x++)
    {
        for (size_t y = 0; y < patchSize; y++)
        {
            float value = (static_cast<float>((patch.data[patchSize * x + y].pixel.y)) / 255.0f) - 0.5 - static_cast<float>(meanBrightness); // - 0.56;
            inputTensor->data.raw[patchSize * x + y] = value;
        }
    }
	//memcpy(inputTensor->data.raw, myData, inputTensor->bytes);


    // Perform inference
     MY_ASSERT_EQ(TfLiteInterpreterInvoke(interpreter), kTfLiteOk);

    // Retrieve the full output tensor 
    const float* outputData = TfLiteInterpreterGetOutputTensor(interpreter, 0)->data.f;
    int numOutputElements = TfLiteTensorByteSize(TfLiteInterpreterGetOutputTensor(interpreter, 0)) / sizeof(float);
    result = std::vector<float>(outputData, outputData + numOutputElements);

    // Print the result
    // std::cout << "The result is: " << result[1] << std::endl;
}

double TFLiteModelNaoTH::getBallConfidence() const 
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_confidence) {
    return result[1];
  }
  return 1.0;
}

double TFLiteModelNaoTH::getRadius() const
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_radius) {
    return result[0];
  }

  return 1.0;
}

Vector2d TFLiteModelNaoTH::getCenter() const
{
  // make sure there is an output tensor
  ASSERT(result.size() > 0);

  if (has_center) {
    return Vector2d(result[2], result[3]);
  }

  return Vector2d(0.5, 0.5);
}