#include "PoseDetector.h"
PoseDetector::PoseDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:PoseDetector:drawJoints", "draw detected human joints", false);
  getDebugParameterList().add(&params);
  //TfLiteModel* model = TfLiteModelCreateFromFileWithErrorReporter(("Config/" + params.tflite_model_file).c_str(), error_reporter, nullptr);
  TfLiteModel* model = TfLiteModelCreateFromFileWithErrorReporter("Config/movenet_lightning.tflite", error_reporter, nullptr);
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

  // Free options and model now that the interpreter is created
  TfLiteInterpreterOptionsDelete(options);
  TfLiteModelDelete(model);

  // Resize input tensor and allocate tensors
  MY_ASSERT_EQ(TfLiteInterpreterResizeInputTensor(interpreter, 0, input_dims.data(), static_cast<int32_t>(input_dims.size())), kTfLiteOk);
  MY_ASSERT_EQ(TfLiteInterpreterAllocateTensors(interpreter), kTfLiteOk);

  inputTensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
  MY_ASSERT_NE(inputTensor, nullptr);
  MY_ASSERT_EQ(TfLiteTensorType(inputTensor), kTfLiteUInt8);

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

  if (!getSoccerStrategy().run_pose_detection){
    return;
  }
  Pixel p;
  cv::Mat yuv422(480, 640, CV_8UC2, getImageTop().data());

  // Convert YUV422 to rgb
  cv::Mat rgb;
  cv::cvtColor(yuv422, rgb, cv::COLOR_YUV2RGB_YUYV);
  // new
  /*
  cv::Mat result = cv::Mat::zeros(getImageTop().height(), getImageTop().width()/2, CV_8UC(6));

  // wrap the original one in a way that uses 4 columns for two pixels
  cv::Mat wrappedYUV422((int) getImageTop().height(), (int) getImageTop().width()/2,
                        CV_8UC4, 
                        (void*) getImageTop().data());

  // Y1 U Y2 V will be converted to Y1 U V, Y2 U V
  // Y1=0 -> 0
  // U=1 -> 1,4
  // Y2=2 -> 3
  // V=3 -> 2,5
  const int fromTo[] = { 0,0, 1,1, 1,4, 2,3, 3,2, 3,5 };
  cv::mixChannels(&wrappedYUV422, 1, &result, 1, fromTo, 6);

  std::vector<cv::Mat> cool_channels;
  cv::split(wrappedYUV422, cool_channels);
  std::vector<cv::Mat> firstThreeChannels(cool_channels.begin(), cool_channels.begin() + 3);
  cv::Mat dst;
  cv::merge(firstThreeChannels, dst);

  // we now have a real matrix with correct color values for each pixel,
  // reshape to a true YUV-color model to make the life easier for users
  // of this matrix
  result.reshape(3);
  std::cout<< "channels after reshape " << result.channels() << std::endl;
  cv::Mat test = cv::Mat::zeros(getImageTop().height(), getImageTop().width()/2, CV_8UC(3));
  cv::cvtColor(dst, test, cv::COLOR_YUV2BGR );
  cv::Mat resized_down(192, 192, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0));
  cv::resize(result, resized_down, cv::Size(192, 192), cv::INTER_LINEAR);
  //cv::imwrite("test.png", resized_down);
  // old
  */

  cv::Mat resized_down(192, 192, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0));
  cv::resize(rgb, resized_down, cv::Size(192, 192), cv::INTER_LINEAR);
  

  uint8_t* inputImg_ptr = resized_down.ptr<uint8_t>(0);
  memcpy(inputTensor->data.uint8, resized_down.ptr<uint8_t>(0), 192 * 192 * 3 * sizeof(uint8_t));
   
  //std::cout << image.at<cv::Vec3f>(0, 0) << std::endl;
  //std::cout << inputTensor->data.f[0] << std::endl;

  STOPWATCH_START("PoseDetector:predict");
  MY_ASSERT_EQ(TfLiteInterpreterInvoke(interpreter), kTfLiteOk);
  STOPWATCH_STOP("PoseDetector:predict");

  const float* outputData = TfLiteInterpreterGetOutputTensor(interpreter, 0)->data.f;
  int numOutputElements = TfLiteTensorByteSize(TfLiteInterpreterGetOutputTensor(interpreter, 0)) / sizeof(float);
  model_result = std::vector<float>(outputData, outputData + numOutputElements);

  std::cout << TfLiteInterpreterGetOutputTensor(interpreter, 0)->dims->size << " " << std::endl;
  std::cout << TfLiteInterpreterGetOutputTensor(interpreter, 0)->dims->data[0] << " " << std::endl;
  std::cout << TfLiteInterpreterGetOutputTensor(interpreter, 0)->dims->data[1] << " " << std::endl;
  std::cout << TfLiteInterpreterGetOutputTensor(interpreter, 0)->dims->data[2] << " " << std::endl;
  std::cout << TfLiteInterpreterGetOutputTensor(interpreter, 0)->dims->data[3] << " " << std::endl;
  // dims are batch = 1, height = 1, width = 17, channels 3
  // channels: y,x,scores
  int batch_size = 1;
  int height = 1;
  int width = 17;
  int channels = 3;

  const float (&a)[1][1][17][3] = * static_cast<const float(*)[1][1][17][3]> (static_cast<void*>(&outputData));
  for (int i=0; i < 17; i++){ 
    std::cout << a[0][0][0][0] << " " << a[0][0][0][1] << " " << a[0][0][0][2] << std::endl;
    std::cout << std::endl;
  }


  // int index = b * (height * width * channels) + h * (width * channels) + w * channels;
  int nose_index = 0 * (height * width * channels) + 0 * (width * channels) + 0 * channels;
  int left_eye_index = 0 * (height * width * channels) + 0 * (width * channels) + 1 * channels;
  int right_eye_index = 0 * (height * width * channels) + 0 * (width * channels) + 2 * channels;
  int left_wrist_index = 0 * (height * width * channels) + 0 * (width * channels) + 9 * channels;
  int right_wrist_index = 0 * (height * width * channels) + 0 * (width * channels) + 10 * channels;
  //std::vector<float> a = std::vector<float>(outputData[nose_index],outputData[nose_index+1],outputData[nose_index+2]);

  //std::vector<float> nose_data {outputData[nose_index],outputData[nose_index+1],outputData[nose_index+2]};
  //std::vector<float> left_eye_data {outputData[left_eye_index],outputData[left_eye_index+1],outputData[left_eye_index+2]};
  //std::vector<float> right_eye_data {outputData[right_eye_index],outputData[right_eye_index+1],outputData[right_eye_index+2]};
  //std::vector<float> left_wrist_data {outputData[left_wrist_index],outputData[left_wrist_index+1],outputData[left_wrist_index+2]};
  //std::vector<float> right_wrist_data {outputData[right_wrist_index],outputData[right_wrist_index+1],outputData[right_wrist_index+2]};
  //std::cout << right_eye_data[0] << " " << right_eye_data[1] << " " << right_eye_data[2] << " " << std::endl;
  /*
  DEBUG_REQUEST("Vision:PoseDetector:drawJoints",

    getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::orange, (int)(nose_data[1] * 192*2.5), (int)(nose_data[0] * 192*3.3), 5);
    getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::orange, (int)(left_eye_data[1]* 192*2.5), (int)(left_eye_data[0] * 192*3.3), 5);
    getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::orange, (int)(right_eye_data[1]* 192*2.5), (int)(right_eye_data[0] * 192*3.3), 5);
    getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::red, (int)(left_wrist_data[1]* 192*2.5), (int)(left_wrist_data[0] * 192*3.3), 5);
    getDebugImageDrawingsTop().drawCircleToImage(ColorClasses::skyblue, (int)(right_wrist_data[1]* 192*2.5), (int)(right_wrist_data[0] * 192*3.3), 5);
  );
  */
}