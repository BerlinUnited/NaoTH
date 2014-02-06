
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
  }

  capture.set(CV_CAP_PROP_FRAME_WIDTH, naoth::IMAGE_WIDTH);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, naoth::IMAGE_HEIGHT);
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
}

void OpenCVWebCamController::get(FrameInfo& data)
{
  data.setTime(time);
}


void OpenCVWebCamController::executeFrame()
{
  capture >> frameNative;

  if(frameNative.data != NULL)
  {
    //Size size;
    //size.width = naoth::IMAGE_WIDTH;
    //size.height = naoth::IMAGE_HEIGHT;
    //resize(frameNative, frame, size, 0, 0, INTER_CUBIC);

    // Kirill
    cvtColor(frameNative, yCbCr, CV_BGR2YCrCb);
    // opencv release libs have some issues with highgui imshow() window's names,
    // so we use the old C functions: cvShowImage,
    // requests ImpImage and NOT Mat, so we must convert Mat to IplImage
    IplImage ipl_img = frameNative;
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
  // variables for fast access (it's in particular faster in debug mode, when inlining is deactivated)
  unsigned char* data = image.data();
  unsigned int width = image.width();
  unsigned int height = image.height();

  for(unsigned int x = 0; x < width; x++) {
    for(unsigned int y = 0; y < height; y++)
    {
      int index = 3 * (x + y * width);
      int yOffset = Image::PIXEL_SIZE_YUV422 * (y * width + x);

      // YCrCb = YVU and NOT YUV, so we must
      // rotate channels
      //Pixel pixel;
      //pixel.y = capturedImage.data[index + 0];
      //pixel.u = capturedImage.data[index + 2];
      //pixel.v = capturedImage.data[index + 1];
      //image.set(x, y, pixel);
      
      data[yOffset] = capturedImage.data[index + 0]; // y
      // ((x & 1)<<1) = 2 if x is odd and 0 if it's even
      data[yOffset+1-((x & 1)<<1)] = capturedImage.data[index + 2]; // u
      data[yOffset+3-((x & 1)<<1)] = capturedImage.data[index + 1]; // v
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


