
#include "OpenCVWebCamController.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;


OpenCVWebCamController::OpenCVWebCamController()
:
PlatformInterface("OpenCVWebCam",10),
frameNumber(0),
frameLossCounter(0),
time(0),
windowName("OpenCVWebCam")
{
  registerInput<FrameInfo>(*this);
  registerInput<Image>(*this);

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
  copyImage(data, yCbCr);
}//end get

void OpenCVWebCamController::get(FrameInfo& data)
{
  data.setTime(time);
}


void OpenCVWebCamController::executeFrame()
{
  capture >> frameNative;

  if(frameNative.data != NULL)
  {
    Size size;
    size.width = 320;
    size.height = 240;
    resize(frameNative, frame, size, 0, 0, INTER_CUBIC);

    // Kirill
    cvtColor(frame, yCbCr, CV_BGR2YCrCb);
    // opencv release libs have some issues with highgui imshow() window's names,
    // so we use the old C functions: cvShowImage,
    // requests ImpImage and NOT Mat, so we must convert Mat to IplImage
    IplImage ipl_img = frame;
    cvShowImage(windowName.c_str(), &ipl_img);
    frameLossCounter = 0;
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

void OpenCVWebCamController::copyImage(Image& image, Mat& capturedImage)
{
  int currentWidth = 320;
  int currentHeight = 240;
  Pixel pixel;

  // Kirill
  for(int x = 0; x < currentWidth; x++)
  {
    for(int y = 0; y < currentHeight; y++)
    {
      int index = 3 * (x + y * currentWidth);
      // YCrCb = YVU and NOT YUV, so we must
      // rotate channels
      pixel.y = capturedImage.data[index + 0];
      pixel.u = capturedImage.data[index + 2];
      pixel.v = capturedImage.data[index + 1];
      image.set(x, y, pixel);
    }//end for
  }//end for
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


