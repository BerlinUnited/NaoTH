#ifndef TFLITEEXECUTER_H
#define TFLITEEXECUTER_H

#include <cstdint>
#include <string>
#include <vector>

struct TfLiteInterpreter;
struct TfLiteDelegate;


class TFLiteExecuter
{
public:
    TFLiteExecuter() = default;
    ~TFLiteExecuter();

    TFLiteExecuter(TFLiteExecuter& h) = delete;
    TFLiteExecuter(TFLiteExecuter&& h) = delete;
    TFLiteExecuter& operator=(const TFLiteExecuter&) = delete;
    TFLiteExecuter& operator=(TFLiteExecuter&&) = delete;

    void loadModelFromFile(std::string file, std::vector<int> inputDims, int numThreads = 1);

    float* getInputTensor();
    const float *getOutputTensor();

    size_t getElementsInputTensor();
    size_t getElementsOutputTensor();

    void execute();
    float* getResult();


private:
    TfLiteInterpreter* interpreter = nullptr;
    TfLiteDelegate* delegate = nullptr;
};


#endif // TFLITEEXECUTER_H