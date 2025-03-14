#include "tfliteexecuter.h"

#include "tensorflow/lite/c/c_api.h"
#include "tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h"

#include <cstdio>
#include <cstdlib>

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



TFLiteExecuter::~TFLiteExecuter() {
    TfLiteInterpreterDelete(interpreter);
    TfLiteXNNPackDelegateDelete(delegate);
}

static void error_reporter(void* user_data, const char* format, va_list args) {
    fprintf(stderr, format, args);
}

void TFLiteExecuter::loadModelFromFile(std::string file, std::vector<int> inputDims, int numThreads) {
    TfLiteModel* model = TfLiteModelCreateFromFileWithErrorReporter(file.c_str(), error_reporter, nullptr);
    if (model == nullptr) {
        fprintf(stderr, "%s:%d  - Couldn't load file: %s\n", __FILE__, __LINE__, file.c_str());
        fprintf(stderr,
                "%s:%d - You can the location of files via the environment variable 'NAO_TFLITE_PATH' see "
                "TFLiteExecuter::getTFliteModelPath() for details.\n",
                __FILE__, __LINE__);
        fflush(stderr);
        exit(1);
    }

    TfLiteXNNPackDelegateOptions xnnPackDelegateOption = TfLiteXNNPackDelegateOptionsDefault();
    xnnPackDelegateOption.num_threads = numThreads;
    delegate = TfLiteXNNPackDelegateCreate(&xnnPackDelegateOption);
    MY_ASSERT_NE(delegate, nullptr);

    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    MY_ASSERT_NE(options, nullptr);
    TfLiteInterpreterOptionsSetNumThreads(options, numThreads);
    TfLiteInterpreterOptionsAddDelegate(options, delegate);

    interpreter = TfLiteInterpreterCreate(model, options);
    MY_ASSERT_NE(interpreter, nullptr);

    TfLiteInterpreterOptionsDelete(options);
    TfLiteModelDelete(model);

    MY_ASSERT_EQ(TfLiteInterpreterAllocateTensors(interpreter), kTfLiteOk);
    MY_ASSERT_EQ(TfLiteInterpreterGetInputTensorCount(interpreter), 1);
    MY_ASSERT_EQ(TfLiteInterpreterGetOutputTensorCount(interpreter), 1);

    MY_ASSERT_EQ(TfLiteInterpreterResizeInputTensor(interpreter, 0, inputDims.data(), inputDims.size()), kTfLiteOk);
    MY_ASSERT_EQ(TfLiteInterpreterAllocateTensors(interpreter), kTfLiteOk);

    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
    MY_ASSERT_NE(inputTensor, nullptr);
    MY_ASSERT_EQ(TfLiteTensorType(inputTensor), kTfLiteFloat32);

    const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
    MY_ASSERT_NE(outputTensor, nullptr);
    MY_ASSERT_EQ(TfLiteTensorType(outputTensor), kTfLiteFloat32);
}


float* TFLiteExecuter::getInputTensor() {
    return TfLiteInterpreterGetInputTensor(interpreter, 0)->data.f;
}

const float* TFLiteExecuter::getOutputTensor() {
    return TfLiteInterpreterGetOutputTensor(interpreter, 0)->data.f;
}

size_t TFLiteExecuter::getElementsInputTensor() {
    return TfLiteTensorByteSize(TfLiteInterpreterGetInputTensor(interpreter, 0)) / sizeof(float);
}

size_t TFLiteExecuter::getElementsOutputTensor() {
    return TfLiteTensorByteSize(TfLiteInterpreterGetOutputTensor(interpreter, 0)) / sizeof(float);
}

void TFLiteExecuter::execute() {
    MY_ASSERT_EQ(TfLiteInterpreterInvoke(interpreter), kTfLiteOk);
}

