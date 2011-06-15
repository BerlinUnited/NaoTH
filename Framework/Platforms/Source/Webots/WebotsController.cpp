/* 
 * File:   WebotsController.cpp
 * Author: Oliver Welter
 *
 * Created on 11. Dezember 2008, 16:06
 */

#include <cmath>


#include "WebotsController.h"
#include "PlatformInterface/Platform.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"

using namespace naoth;

WebotsController::WebotsController()
  : 
    PlatformInterface<WebotsController>("Webots", (int)wb_robot_get_basic_time_step()/*40*/),
    key(0),
    currentStiffness()
{
  wb_robot_init();
  get_Devices();
  cout << "Constructor finished" << endl;

  // register input
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<BumperData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const SoundData>(*this);
  registerOutput<const MotorJointData>(*this);
}

WebotsController::~WebotsController()
{
  wb_robot_cleanup();
}

//get all devices of the robot and enable them with the basic update frequency
void WebotsController::get_Devices()
{
  
  camera = wb_robot_get_device("camera");
  wb_camera_enable(camera,getBasicTimeStep());

  cameraSelect = wb_robot_get_device("CameraSelect");
  wb_servo_enable_position(cameraSelect,getBasicTimeStep());

  for(int i=0;i<FSRData::numOfFSR;i++)
  {
    fsr[(FSRData::FSRID) i] = wb_robot_get_device(FSRData::getFSRName((FSRData::FSRID) i).c_str());
    wb_touch_sensor_enable(fsr[i],getBasicTimeStep());
  }//end for

  for(int i=0;i<JointData::numOfJoint;i++)
  {  
    joint[(JointData::JointID) i] = wb_robot_get_device(JointData::getJointName((JointData::JointID)i).c_str());
    wb_servo_enable_position(joint[i], getBasicTimeStep());
  }//end for

  accelerometer = wb_robot_get_device("accelerometer");
  wb_accelerometer_enable(accelerometer, getBasicTimeStep());

  gyrometer = wb_robot_get_device("gyro");
  wb_gyro_enable(gyrometer, getBasicTimeStep());

  gps = wb_robot_get_device("gps");
  wb_gps_enable(gps, getBasicTimeStep());

  leds[EarLeft] = wb_robot_get_device("Ears/Led/Left");
  leds[EarRight] = wb_robot_get_device("Ears/Led/Right");
  leds[FaceLeft] = wb_robot_get_device("Face/Led/Left");
  leds[FaceRight] = wb_robot_get_device("Face/Led/Right");
  leds[ChestButton] = wb_robot_get_device("ChestBoard/Led");
  leds[FootRight] = wb_robot_get_device("RFoot/Led");
  leds[FootLeft] = wb_robot_get_device("LFoot/Led");

  bumper[BumperData::LeftBumperLeft] = wb_robot_get_device("LFoot/Bumper/Left");
  bumper[BumperData::LeftBumperRight] = wb_robot_get_device("LFoot/Bumper/Right");
  bumper[BumperData::RightBumperLeft] = wb_robot_get_device("RFoot/Bumper/Left");
  bumper[BumperData::RightBumperRight] = wb_robot_get_device("RFoot/Bumper/Right");
  
  for(int i=0;i<BumperData::numOfBumper;i++)
  {
    wb_touch_sensor_enable(bumper[i], getBasicTimeStep());
  }

  wb_robot_keyboard_enable(getBasicTimeStep());

}//end get_Devices

void WebotsController::init()
{
  std::cout << "Init WebotsController" << std::endl;
//  PlayerInfo::TeamColor teamColor = PlayerInfo::red;
  bool isBlue = false;
  unsigned char playerNumber = 0;


  // get player-info from webots
  std::string robotName(wb_robot_get_name());

  if(robotName.substr(0,3) == "blue")
    isBlue = true;

  if(isBlue)
  {
    if(robotName != "blue goal keeper")
    {
      int n = 1;
      // TODO: don't use sscanf
#ifdef WIN32
      sscanf_s(robotName.c_str(), "blue player %d", &n);
#else
      sscanf(robotName.c_str(), "blue player %d", &n);
#endif
      playerNumber = n;
    }
  }
  else
  {
    if(robotName != "red goal keeper")
    {
      int n = 1;
      // TODO: don't use sscanf
#ifdef WIN32
      sscanf_s(robotName.c_str(), "red player %d", &n);
#else
      sscanf(robotName.c_str(), "red player %d", &n);
#endif
      playerNumber = n;
    }
  }

  // calculate port
  unsigned short port = 5401;
  if(isBlue)
  {
    port = 5500 + playerNumber;
  }
  else
  {
    port = 5400 + playerNumber;
  }

  Platform::getInstance().init(this);
}

void WebotsController::main()
{
  cout << "Run WebotsController" << endl;

  while(wb_robot_step(getBasicTimeStep()) != -1)
  {
    //cout << "Step WebotsController" << endl;
    key = wb_robot_keyboard_get_key();
    if(key >= '0' && key <='9')
    {
      key-='0';
    }

    callCognition();
    callMotion();
  }//end while

  cout << "Run finished" << endl;
}//end main

void WebotsController::get(FrameInfo& data)
{
  data.frameNumber++;
  data.time += getBasicTimeStep();
  data.basicTimeStep = getBasicTimeStep();
}

void WebotsController::get(SensorJointData& data)
{
  double dt = getBasicTimeStep() / 1000.0;
  for (int i = 0; i < JointData::numOfJoint; i++) {
    double p = wb_servo_get_position(joint[i]);
    double dp = (p - data.position[i]) / dt;
    data.ddp[i] = (dp - data.dp[i]) / dt;
    data.dp[i] = dp;
    data.position[i] = p;
    
    data.stiffness[i] = currentStiffness[i];
  }//end for
}

void WebotsController::get(AccelerometerData& data)
{
  const double *webots_accelerometer = wb_accelerometer_get_values(accelerometer);
  data.data[AccelerometerData::X] = webots_accelerometer[0];
  data.data[AccelerometerData::Y] = webots_accelerometer[1];
  data.data[AccelerometerData::Z] = webots_accelerometer[2];
}

/**
 * the original webots image is organized as following
 *    image[3*(y*width+x)]   = red
 *    image[3*(y*width+x)+1] = green
 *    image[3*(y*width+x)+2] = blue
 *
 */
void WebotsController::get(Image& data)
{
  data.setCameraInfo(Platform::getInstance().theCameraInfo);
  const unsigned char *webotsImage = wb_camera_get_image(camera);

  //unsigned char *yuv422Image = transformRGBtoYUV422(webotsImage);

  // assume the Image to be 320x240
  if(data.cameraInfo.resolutionWidth == 320)
  {
    copyImage(data, webotsImage);
  }
  
  double t = wb_servo_get_position(cameraSelect);
  data.cameraInfo.cameraID = (t > 0)?CameraInfo::Bottom:CameraInfo::Top;
}

// upsampling of 160x120 -> 320x240 image
// using linear interpolation
void WebotsController::copyImage(Image& image, const unsigned char *woriginal_image)
{
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  int factorW = image.cameraInfo.resolutionWidth/width;
  int factorH = image.cameraInfo.resolutionHeight/height;

  unsigned char original_image[320*240*3];

  for(int x = 0; x < width; x++)
  {
    int indexWebots_first = 3*x;
    int indexWebots1_first = (x == width-1)?indexWebots_first:3*(x+1);

    // convert the first line
    ColorModelConversions::fromRGBToYCbCr(
        woriginal_image[indexWebots_first],
        woriginal_image[indexWebots_first+1],
        woriginal_image[indexWebots_first+2],
        original_image[indexWebots_first],
        original_image[indexWebots_first+1],
        original_image[indexWebots_first+2]);

      ColorModelConversions::fromRGBToYCbCr(
        woriginal_image[indexWebots1_first],
        woriginal_image[indexWebots1_first+1],
        woriginal_image[indexWebots1_first+2],
        original_image[indexWebots1_first],
        original_image[indexWebots1_first+1],
        original_image[indexWebots1_first+2]);


    for(int y = 0; y < height; y++)
    {
      int xImage = x*factorW;
      int yImage = y*factorH;

      int indexWebots = 3*(x + y*width);
      int indexWebots1 = (x == width-1)?indexWebots:3*((x+1) + y*width);
      int indexWebots2 = (x == width-1 || y == height-1)?indexWebots:3*((x+1) + (y+1)*width);
      int indexWebots3 = (y == height-1)?indexWebots:3*(x + (y+1)*width);

      ColorModelConversions::fromRGBToYCbCr(
        woriginal_image[indexWebots2],
        woriginal_image[indexWebots2+1],
        woriginal_image[indexWebots2+2],
        original_image[indexWebots2],
        original_image[indexWebots2+1],
        original_image[indexWebots2+2]);

      ColorModelConversions::fromRGBToYCbCr(
        woriginal_image[indexWebots3],
        woriginal_image[indexWebots3+1],
        woriginal_image[indexWebots3+2],
        original_image[indexWebots3],
        original_image[indexWebots3+1],
        original_image[indexWebots3+2]);

      Pixel pixels[4];

      for(int k = 0; k < 3; k++)
      {

        pixels[0].channels[k]     = original_image[indexWebots+k];
        pixels[1].channels[k]   = (original_image[indexWebots+k]+original_image[indexWebots1+k])/2;
        pixels[2].channels[k] = (original_image[indexWebots+k]+original_image[indexWebots1+k]+
                                         original_image[indexWebots2+k]+original_image[indexWebots3+k])/4;
        pixels[3].channels[k]   = (original_image[indexWebots+k]+original_image[indexWebots3+k])/2;

        /*
        image.image[yImage][xImage].channels[k]     = original_image[indexWebots+k];
        image.image[yImage][xImage+1].channels[k]   = (original_image[indexWebots+k]+original_image[indexWebots1+k])/2;
        image.image[yImage+1][xImage+1].channels[k] = (original_image[indexWebots+k]+original_image[indexWebots1+k]+
                                         original_image[indexWebots2+k]+original_image[indexWebots3+k])/4;
        image.image[yImage+1][xImage].channels[k]   = (original_image[indexWebots+k]+original_image[indexWebots3+k])/2;
         */
      }//end for

      image.set(xImage,yImage, pixels[0]);
      image.set(xImage+1,yImage, pixels[1]);
      image.set(xImage+1,yImage+1, pixels[2]);
      image.set(xImage,yImage+1, pixels[3]);
    }//end for
  }//end for

/*
//GAUSS Filter

    double filter[3][3]=
      {{1.0/16.0, 2.0/16.0, 1.0/16.0},
       {2.0/16.0, 4.0/16.0, 2.0/16.0},
       {1.0/16.0, 2.0/16.0, 1.0/16.0}};

  for(int x = 1; x < theImage.cameraInfo.resolutionWidth-1; x++)
    for(int y = 1; y < theImage.cameraInfo.resolutionHeight-1; y++)
    {
      double valueY = 0;
      double valueU = 0;
      double valueV = 0;

      for(int i = -1; i < 2; i++)
      {
        for(int j = -1; j < 2; j++)
        {
          valueY += filter[i+1][j+1]*theImage.get_channel(x+i,y+j,Image::Y);
          valueU += filter[i+1][j+1]*theImage.get_channel(x+i,y+j,Image::U);
          valueV += filter[i+1][j+1]*theImage.get_channel(x+i,y+j,Image::V);
        }//end for
      }//end for

      int index = 3*(x + y*theImage.cameraInfo.resolutionWidth);
      theImage.image[index] = (unsigned char)valueY; // RED
      theImage.image[index+1] = (unsigned char)valueU; // GREEN
      theImage.image[index+2] = (unsigned char)valueV; // BLUE
    }//end for
    */
}//end copyImage

void WebotsController::get(GyrometerData& data)
{
  const double *webots_gyro = wb_gyro_get_values(gyrometer);
  data.data[GyrometerData::X] = webots_gyro[0];
  data.data[GyrometerData::Y] = webots_gyro[1];
}

void WebotsController::get(FSRData& data)
{
  for (int i = 0; i < FSRData::numOfFSR; i++) {
    data.data[i] = wb_touch_sensor_get_value(fsr[i]);
    data.force[i] = data.data[i]; // the data is force in webots
  }
}

void WebotsController::get(InertialSensorData& data)
{
  // calculate the intertial sensor from GPS data
  // it can be enabled for debuging
//  const float *gpsmatrix = gps_get_matrix(gps);
//  data.data[InertialSensorData::X] = asin(-gpsmatrix[6]);
//  data.data[InertialSensorData::Y] = -atan2(gpsmatrix[4], gpsmatrix[5]);
//  return;

  // calculate inertial sensor data by gyrometer
  const double *webots_gyrometer = wb_gyro_get_values(gyrometer);
  double time = getBasicTimeStep() * 1e-3;
  data.data[InertialSensorData::X] += (webots_gyrometer[0] * time);
  data.data[InertialSensorData::Y] += (webots_gyrometer[1] * time);

  // calibrate the Interial Sensor while the robot is static
  if ( abs(webots_gyrometer[0]) < 0.01 && abs(webots_gyrometer[1]) < 0.01  )
  {
    // calculate intertial sensor data by accelerometer
    const double *webots_acc = wb_accelerometer_get_values(accelerometer);
    double len = sqrt(Math::sqr(webots_acc[0])+Math::sqr(webots_acc[1])+Math::sqr(webots_acc[2]));
    if(len > 1)
    {
      data.data[InertialSensorData::X] = asin(webots_acc[1]/len);
      double cx = cos(data.data[InertialSensorData::X]);
      if(fabs(cx) > 0)
      {
        data.data[InertialSensorData::Y] = -atan2(webots_acc[0]/cx, webots_acc[2]/cx);
      }
    }
//    else
//    {
//      cerr<<" can not update InertialSensorData"<<endl;
//    }
  }
}

void WebotsController::get(BumperData& data)
{
  for (int i = 0; i < BumperData::numOfBumper; i++) {
    data.data[i] = (wb_touch_sensor_get_value(bumper[i]) > 0);
  }
}

void WebotsController::get(IRReceiveData& data)
{
  data.data[IRReceiveData::RightRCByte2] = key;
}

void WebotsController::set(const MotorJointData& data)
{
  const double* stiffness = data.stiffness;
  const double* jointData = data.position;
  for (int i = 0; i < JointData::numOfJoint; i++) {
    if (stiffness[i] > 0) {
      wb_servo_set_position(joint[i], jointData[i]);
      currentStiffness[i] = stiffness[i];
    }
  }//end for
}

void WebotsController::set(const LEDData& data)
{
  int value = 0;
  //EarLeft
  for (int i = LEDData::EarLeft0; i <= LEDData::EarLeft324; i++) {
    if (data.theMonoLED[i] > 0.0) {
      value = (int) (255 * data.theMonoLED[i]);
      wb_led_set(leds[WebotsController::EarLeft], 0xff);
      break;
    }
  }

  //EarRight
  for (int i = LEDData::EarRight0; i <= LEDData::EarRight324; i++) {
    if (data.theMonoLED[i] > 0.0) {
      value = (int) (255 * data.theMonoLED[i]);
      wb_led_set(leds[WebotsController::EarRight], 0xff);
      break;
    }
  }

  //ChestButton
  if (data.theMultiLED[LEDData::ChestButton][LEDData::RED] > 0.0) {
    wb_led_set(leds[WebotsController::ChestButton], 0xff0000);
  } else if (data.theMultiLED[LEDData::ChestButton][LEDData::GREEN] > 0.0) {
    wb_led_set(leds[WebotsController::ChestButton], 0x00ff00);
  } else if (data.theMultiLED[LEDData::ChestButton][LEDData::BLUE] > 0.0) {
    wb_led_set(leds[WebotsController::ChestButton], 0x0000ff);
  }

  //LeftEye
  for (int i = LEDData::FaceLeft0; i <= LEDData::FaceLeft315; i++) {
    if (data.theMultiLED[i][LEDData::RED] > 0.0) {
      wb_led_set(leds[WebotsController::FaceLeft], 0xff0000);
      break;
    } else if (data.theMultiLED[i][LEDData::GREEN] > 0.0) {
      wb_led_set(leds[WebotsController::FaceLeft], 0x00ff00);
      break;
    } else if (data.theMultiLED[i][LEDData::BLUE] > 0.0) {
      wb_led_set(leds[WebotsController::FaceLeft], 0x0000ff);
      break;
    }
  }

  //RightEye
  for (int i = LEDData::FaceRight0; i <= LEDData::FaceRight315; i++) {
    if (data.theMultiLED[i][LEDData::RED] > 0.0) {
      wb_led_set(leds[WebotsController::FaceRight], 0xff0000);
      break;
    } else if (data.theMultiLED[i][LEDData::GREEN] > 0.0) {
      wb_led_set(leds[WebotsController::FaceRight], 0x00ff00);
      break;
    } else if (data.theMultiLED[i][LEDData::BLUE] > 0.0) {
      wb_led_set(leds[WebotsController::FaceRight], 0x0000ff);
      break;
    }
  }
}

void WebotsController::set(const CameraSettingsRequest& data)
{
  // select the camera
  wb_servo_set_position(cameraSelect, data.data[CameraSettings::CameraSelection]);
}


void WebotsController::get(ButtonData& /*data*/)
{
  // not support yet
}

//void WebotsController::get(GPSData& data)
//{
////    const double *webots_gps = wb_gps_get_values(theWebotsController.gps);
////    gps.translation.x = webots_gps[0] * 1000;
////    gps.translation.y = webots_gps[2] * 1000;
////    gps.translation.z = webots_gps[1] * 1000;
//
//    // the deprecated method, but it provides rotation matrix
//    const float *gpsmatrix = gps_get_matrix(gps);
//    Vector3<double>& pos = data.data.translation;
//    pos.x = -gpsmatrix[12] * 1000;
//    pos.y = gpsmatrix[14] * 1000;
//    pos.z = gpsmatrix[13] * 1000;
//    double angZ = atan2(gpsmatrix[2], -gpsmatrix[0]) - Math::pi_2;
//    data.data.rotation = RotationMatrix::getRotationZ(angZ);
//}

void WebotsController::get(CurrentCameraSettings& data)
{
  // get the camera position
  double p = wb_servo_get_position(cameraSelect);
  data.data[CameraSettings::CameraSelection] = p>0.1?CameraInfo::Bottom:CameraInfo::Top;
}

  void WebotsController::set(const IRSendData& /*data*/)
  {
    // unsupported yet
  }

  void WebotsController::set(const UltraSoundSendData& /*data*/)
  {
    // unsupported yet
  }

  void WebotsController::set(const SoundData& /*data*/)
  {
    // unsupported yet
  }

void WebotsController::getCognitionInput()
{
  PlatformInterface<WebotsController>::getCognitionInput();
  //get(theGPSDataProvider.theGPSData);
}

MessageQueue* WebotsController::createMessageQueue(const std::string& name)
{
  // for single thread
  return new MessageQueue();
}
