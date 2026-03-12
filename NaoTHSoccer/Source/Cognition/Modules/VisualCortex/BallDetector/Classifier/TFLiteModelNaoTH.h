#ifndef TFLITEMODEL_NAOTH_H
#define TFLITEMODEL_NAOTH_H

#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h"

#include "AbstractCNNClassifier.h"

// TFlite C API based on HTWK Implementation
// https://github.com/NaoHTWK/HTWKVision/

static void error_reporter(void* /*user_data*/, const char* format, va_list args) {
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

struct TfLiteInterpreter;
struct TfLiteDelegate;

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

class TFLiteModelNaoTH : public AbstractCNNFinder
{
public:
    TFLiteModelNaoTH(std::string file, bool has_radius = false, bool has_center = false, bool has_confidence = true) :
      name(file),
      has_radius(has_radius),
      has_center(has_center),
      has_confidence(has_confidence)
    {
        TfLiteModel* model = TfLiteModelCreateFromFileWithErrorReporter(("Config/" + file).c_str(), error_reporter, nullptr);
        MY_ASSERT_NE(model, nullptr);

        #ifndef WIN32
        TfLiteXNNPackDelegateOptions xnnPackDelegateOption = TfLiteXNNPackDelegateOptionsDefault();
        xnnPackDelegateOption.num_threads = numThreads;
        delegate = TfLiteXNNPackDelegateCreate(&xnnPackDelegateOption);
        MY_ASSERT_NE(delegate, nullptr);
        #endif // undef WIN32

        TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
        MY_ASSERT_NE(options, nullptr);
        TfLiteInterpreterOptionsSetNumThreads(options, numThreads);

        #ifndef WIN32
        TfLiteInterpreterOptionsAddDelegate(options, delegate);
        #endif // undef WIN32

        interpreter = TfLiteInterpreterCreate(model, options);
        MY_ASSERT_NE(interpreter, nullptr);

        inputTensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
				MY_ASSERT_NE(inputTensor, nullptr);
        MY_ASSERT_EQ(TfLiteTensorType(inputTensor), kTfLiteFloat32);

        // Free options and model now that the interpreter is created
        TfLiteInterpreterOptionsDelete(options);
        TfLiteModelDelete(model);

        // Resize input tensor and allocate tensors
        MY_ASSERT_EQ(TfLiteInterpreterResizeInputTensor(interpreter, 0, input_dims.data(), input_dims.size()), kTfLiteOk);
        MY_ASSERT_EQ(TfLiteInterpreterAllocateTensors(interpreter), kTfLiteOk);

        MY_ASSERT_NE(inputTensor, nullptr);
        MY_ASSERT_EQ(TfLiteTensorType(inputTensor), kTfLiteFloat32);

        const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
        MY_ASSERT_NE(outputTensor, nullptr);
        MY_ASSERT_EQ(TfLiteTensorType(outputTensor), kTfLiteFloat32);
    }

    virtual ~TFLiteModelNaoTH() {
        if (interpreter) {
            TfLiteInterpreterDelete(interpreter);
        }
        #ifndef WIN32
        if (delegate) {
            TfLiteXNNPackDelegateDelete(delegate);
        }
        #endif
    }

    virtual void predict(const BallCandidates::PatchYUVClassified& p, double meanBrightness);
    /** This is actually the "no ball" confidence */
    virtual double getRadius() const;
    virtual Vector2d getCenter() const;
    virtual double getBallConfidence() const;

private:
    const std::string name;
    bool has_radius;
    bool has_center;
    bool has_confidence;
    int numThreads = 2;
    std::vector<int> input_dims = {1, 16, 16, 1};
    TfLiteTensor* inputTensor;
    std::vector<float> result;
    TfLiteInterpreter* interpreter = nullptr;
    TfLiteDelegate* delegate = nullptr;
};

#endif
