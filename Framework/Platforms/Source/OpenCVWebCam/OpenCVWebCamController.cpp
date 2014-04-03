
#include "OpenCVWebCamController.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;


OpenCVWebCamController::OpenCVWebCamController()
:
  PlatformInterface("OpenCVWebCam",10),
  frameNumber(0),
  time(0),
  windowName("OpenCVWebCam"),
  frameLossCounter(0)
{
  registerInput<FrameInfo>(*this);
  registerInput<Image>(*this);
  registerInput<SensorJointData>(*this);

  registerOutput<const MotorJointData>(*this);

  // debug
  registerInput<DebugMessageIn>(*this);
  registerOutput<DebugMessageOut>(*this);


  theGameData.loadFromCfg(Platform::getInstance().theConfiguration);
  theDebugServer.start(5401, true);
}

void OpenCVWebCamController::init()
{
  // check whether we have initialized the cam
  capture.open(0);
  if(!capture.isOpened())  // check if we succeeded
  {
    std::cerr << "Can't open camera 0" << std::endl;
    exit(-1);
  }//end if(!capture.isOpened())

//   cvInitSystem(0, NULL);
}//end init

void OpenCVWebCamController::main()
{
  while(waitKey(15) != 'q')
  {
    executeFrame();
    makeStep();
  }
}

void OpenCVWebCamController::get(Image& data)
{
  copyImage(data);
}//end get

void OpenCVWebCamController::get(FrameInfo& data)
{
  data.setTime(time);
}

void OpenCVWebCamController::get(SensorJointData& data)
{
  double dt = getBasicTimeStep() * 0.1;
  for (int i = 0; i < JointData::numOfJoint; i++) 
  {
    double p = motorJointData.position[i];;
    double dp = (p - data.position[i]) / dt;
    data.ddp[i] = (dp - data.dp[i]) / dt;
    data.dp[i] = dp;
    data.position[i] = p;    
    data.stiffness[i] = 1.0;
  }//end for
}

void OpenCVWebCamController::set(const MotorJointData& data)
{
  MotorJointData tmpData = motorJointData;
  motorJointData = data;
  double dt = getBasicTimeStep() * 0.1;
  for (int i = 0; i < JointData::numOfJoint; i++) 
  {
    double p = tmpData.position[i];
    double dp = (data.position[i] - p) * dt;
    motorJointData.position[i] = tmpData.position[i] + dp;
  }//end for
}


void OpenCVWebCamController::executeFrame()
{
  capture >> frameNative;

  if(frameNative.data != NULL)
  {
    // opencv release libs have some issues with highgui imshow() window's names,
    // so we use the old C functions: cvShowImage,
    // requests ImpImage and NOT Mat, so we must convert Mat to IplImage
    IplImage ipl_img = frameNative;
    cvShowImage(windowName.c_str(), &ipl_img);
    frameLossCounter = 0;
  }
}

void OpenCVWebCamController::copyImage(Image& image)
{
  Size size;
  size.width = image.width();
  size.height = image.height();
  Pixel pixel;

  if(frameNative.data != NULL)
  {
    resize(frameNative, frame, size, 0, 0, INTER_CUBIC);

    // Kirill
    cvtColor(frame, yCbCr, CV_BGR2YCrCb);

    // Kirill
    for(int x = 0; x < size.width; x++)
    {
      for(int y = 0; y < size.height; y++)
      {
        int index = 3 * (x + y * size.width);
        // YCrCb = YVU and NOT YUV, so we must
        // rotate channels
        pixel.y = yCbCr.data[index + 0];
        pixel.u = yCbCr.data[index + 2];
        pixel.v = yCbCr.data[index + 1];
        image.set(x, y, pixel);
      }//end for
    }//end for
  }
  else
  {
    frameLossCounter++;
  }
  if(frameLossCounter > 50)
  {
    std::cerr << "lost continuously  50 frames, exiting" << std::endl;
    exit(-1);
  }
}

void OpenCVWebCamController::makeStep()
{
  time += getBasicTimeStep();
  runCognition();
  runMotion();
}


MessageQueue* OpenCVWebCamController::createMessageQueue(const std::string& name)
{
  // for single thread
  return new MessageQueue();
}



OpenCVWebCamController::~OpenCVWebCamController()
{
  capture.release();
  destroyWindow(windowName);
}


