#include "PoseDetector.h"

PoseDetector::PoseDetector()
{
  getDebugParameterList().add(&params);
  TfLiteModel* model = TfLiteModelCreateFromFileWithErrorReporter(("Config/" + params.tflite_model_file).c_str(), error_reporter, nullptr);
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

PoseDetector::~PoseDetector() {
  getDebugParameterList().remove(&params);

  if (interpreter) {
      TfLiteInterpreterDelete(interpreter);
  }
  #ifndef WIN32
  if (delegate) {
      TfLiteXNNPackDelegateDelete(delegate);
  }
  #endif
}

void PoseDetector::execute(){
  Pixel p;
  //TODO expect input as 1,353,257,3 rgb for posenet or 1x192x192x3 rgb for movenet
  //int y_value = ((int)getImageTop().getY_direct(0,0));
  //cv::Mat img = cv::Mat::eye(192,192,CV_32FC3);
  cv::Mat image(192, 192, CV_32FC3, cv::Scalar(0.0f, 0.0f, 0.0f)); // Create a 192x192 image with 3 channels (RGB) initialized to black
  for (unsigned int y = 0; y < 192; y+=1) {
      for (unsigned int x = 0; x < 192; x+=1) {
          getImageTop().get(x,y,p); 

          naoth::ColorModelConversions::fromYCbCrToRGB(
          p.y, p.u, p.v,
          p.y, p.u, p.v); 
          //inputTensor->data.f[320 * (y/2) + (x/2)] = y_value / 255.0f;
          //std::cout << 240 * (y/2) + (x/2) << std::endl;
          image.at<cv::Vec3f>(y, x) = cv::Vec3f(p.v / 255.0, p.u / 255.0, p.y / 255.0);     
      }
  }
  cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
  float* inputImg_ptr = image.ptr<float>(0);
  memcpy(inputTensor->data.f, image.ptr<float>(0), 192 * 192 * 3 * sizeof(float));
   
  std::cout << image.at<cv::Vec3f>(0, 0) << std::endl;
  std::cout << inputTensor->data.f[0] << std::endl;

  STOPWATCH_START("PoseDetector:predict");
  MY_ASSERT_EQ(TfLiteInterpreterInvoke(interpreter), kTfLiteOk);
  STOPWATCH_STOP("PoseDetector:predict");

}