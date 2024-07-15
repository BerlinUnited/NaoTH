#ifndef POSEDETECTOR_H
#define POSEDETECTOR_H

#include <iostream>


#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include <Representations/Debug/Stopwatch.h>

// Representations
#include <Representations/Infrastructure/Image.h>
#include "Representations/Motion/MotionStatus.h"

// Tools
#include "Tools/CameraGeometry.h"
#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>
#include "Tools/naoth_opencv.h"

#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h"


BEGIN_DECLARE_MODULE(PoseDetector)
  REQUIRE(ImageTop)

  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(PoseDetector)

// TFlite C API based on HTWK Implementation
// https://github.com/NaoHTWK/HTWKVision/
#define MY_ASSERT_NE(v, e)                                                                        \
    do {                                                                                          \
        if ((v) == (e)) {                                                                         \
            fprintf(stderr, "%s (%s:%d): This is kaputt! Exit!\n", __func__, __FILE__, __LINE__); \
            fflush(stderr);                                                                       \
            exit(1);                                                                              \
        }                                                                                         \
    } while (0)

#define MY_ASSERT_EQ(v, e)                                                                        \
    do {                                                                                          \
        if ((v) != (e)) {                                                                         \
            fprintf(stderr, "%s (%s:%d): This is kaputt! Exit!\n", __func__, __FILE__, __LINE__); \
            fflush(stderr);                                                                       \
            exit(1);                                                                              \
        }                                                                                         \
    } while (0)
    


struct TfLiteInterpreter;
struct TfLiteDelegate;


class PoseDetector: public PoseDetectorBase
{
public:
  PoseDetector();
  ~PoseDetector();
  static void error_reporter(void* /*user_data*/, const char* format, va_list args) {
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
  }

  virtual void execute();

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("PoseDetector")
    {
      PARAMETER_REGISTER(tflite_model_file) = "posenet-mobilenet.tflite";
      syncWithConfig();
    }
    std::string tflite_model_file;
  } params;
  private:
    int numThreads = 2;
    std::vector<int> input_dims = {1, 192, 192, 3};
    TfLiteTensor* inputTensor;
    std::vector<float> result;
    TfLiteInterpreter* interpreter = nullptr;
    TfLiteDelegate* delegate = nullptr;
};

#endif // POSEDETECTOR_H
