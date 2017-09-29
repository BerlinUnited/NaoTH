

#include "DCM_led.h"


using namespace naoth;
using namespace std;
using namespace AL;


DCM_led::DCM_led()
{
  // reset last ledData
  // set all requests to invalide values
  // this will force the function setSingleLED to set all 
  // values at the first call
  for(int i=0;i<LEDData::numOfMonoLED;i++) {
    lastLEDData.theMonoLED[i] = -1.0;
  }

  for(int i=0;i<LEDData::numOfMultiLED;i++) {
    for(int k=0;k<LEDData::numOfLEDColor;k++) {
      lastLEDData.theMultiLED[i][k] = -1.0;
    }
  }


  // Generate all paths for interaction
  // ear left
  path_MonoLED[LEDData::EarLeft0]           = "Ears/Led/Left/0Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft36]          = "Ears/Led/Left/36Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft72]          = "Ears/Led/Left/72Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft108]         = "Ears/Led/Left/108Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft144]         = "Ears/Led/Left/144Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft180]         = "Ears/Led/Left/180Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft216]         = "Ears/Led/Left/216Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft252]         = "Ears/Led/Left/252Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft288]         = "Ears/Led/Left/288Deg/Actuator/Value";
  path_MonoLED[LEDData::EarLeft324]         = "Ears/Led/Left/324Deg/Actuator/Value";

  // ear right
  path_MonoLED[LEDData::EarRight0]          = "Ears/Led/Right/0Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight36]         = "Ears/Led/Right/36Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight72]         = "Ears/Led/Right/72Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight108]        = "Ears/Led/Right/108Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight144]        = "Ears/Led/Right/144Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight180]        = "Ears/Led/Right/180Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight216]        = "Ears/Led/Right/216Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight252]        = "Ears/Led/Right/252Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight288]        = "Ears/Led/Right/288Deg/Actuator/Value";
  path_MonoLED[LEDData::EarRight324]        = "Ears/Led/Right/324Deg/Actuator/Value";

  // head
  path_MonoLED[LEDData::HeadFrontLeft0]     = "Head/Led/Front/Left/0/Actuator/Value";
  path_MonoLED[LEDData::HeadFrontLeft1]     = "Head/Led/Front/Left/1/Actuator/Value";
  path_MonoLED[LEDData::HeadFrontRight0]    = "Head/Led/Front/Right/0/Actuator/Value";
  path_MonoLED[LEDData::HeadFrontRight1]    = "Head/Led/Front/Right/1/Actuator/Value";
  path_MonoLED[LEDData::HeadMiddleLeft0]    = "Head/Led/Middle/Left/0/Actuator/Value";
  path_MonoLED[LEDData::HeadMiddleRight0]   = "Head/Led/Middle/Right/0/Actuator/Value";
  path_MonoLED[LEDData::HeadRearLeft0]      = "Head/Led/Rear/Left/0/Actuator/Value";
  path_MonoLED[LEDData::HeadRearLeft1]      = "Head/Led/Rear/Left/1/Actuator/Value";
  path_MonoLED[LEDData::HeadRearLeft2]      = "Head/Led/Rear/Left/2/Actuator/Value";
  path_MonoLED[LEDData::HeadRearRight0]     = "Head/Led/Rear/Right/0/Actuator/Value";
  path_MonoLED[LEDData::HeadRearRight1]     = "Head/Led/Rear/Right/1/Actuator/Value";
  path_MonoLED[LEDData::HeadRearRight2]     = "Head/Led/Rear/Right/2/Actuator/Value";

  // eye left
  path_MultiLED[LEDData::FaceLeft0][LEDData::RED]      = "Face/Led/Red/Left/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft0][LEDData::GREEN]    = "Face/Led/Green/Left/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft0][LEDData::BLUE]     = "Face/Led/Blue/Left/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft45][LEDData::RED]     = "Face/Led/Red/Left/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft45][LEDData::GREEN]   = "Face/Led/Green/Left/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft45][LEDData::BLUE]    = "Face/Led/Blue/Left/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft90][LEDData::RED]     = "Face/Led/Red/Left/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft90][LEDData::GREEN]   = "Face/Led/Green/Left/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft90][LEDData::BLUE]    = "Face/Led/Blue/Left/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft135][LEDData::RED]    = "Face/Led/Red/Left/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft135][LEDData::GREEN]  = "Face/Led/Green/Left/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft135][LEDData::BLUE]   = "Face/Led/Blue/Left/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft180][LEDData::RED]    = "Face/Led/Red/Left/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft180][LEDData::GREEN]  = "Face/Led/Green/Left/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft180][LEDData::BLUE]   = "Face/Led/Blue/Left/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft225][LEDData::RED]    = "Face/Led/Red/Left/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft225][LEDData::GREEN]  = "Face/Led/Green/Left/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft225][LEDData::BLUE]   = "Face/Led/Blue/Left/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft270][LEDData::RED]    = "Face/Led/Red/Left/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft270][LEDData::GREEN]  = "Face/Led/Green/Left/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft270][LEDData::BLUE]   = "Face/Led/Blue/Left/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft315][LEDData::RED]    = "Face/Led/Red/Left/315Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft315][LEDData::GREEN]  = "Face/Led/Green/Left/315Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceLeft315][LEDData::BLUE]   = "Face/Led/Blue/Left/315Deg/Actuator/Value";

  // eye right
  path_MultiLED[LEDData::FaceRight0][LEDData::RED]     = "Face/Led/Red/Right/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight0][LEDData::GREEN]   = "Face/Led/Green/Right/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight0][LEDData::BLUE]    = "Face/Led/Blue/Right/0Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight45][LEDData::RED]    = "Face/Led/Red/Right/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight45][LEDData::GREEN]  = "Face/Led/Green/Right/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight45][LEDData::BLUE]   = "Face/Led/Blue/Right/45Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight90][LEDData::RED]    = "Face/Led/Red/Right/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight90][LEDData::GREEN]  = "Face/Led/Green/Right/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight90][LEDData::BLUE]   = "Face/Led/Blue/Right/90Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight135][LEDData::RED]   = "Face/Led/Red/Right/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight135][LEDData::GREEN] = "Face/Led/Green/Right/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight135][LEDData::BLUE]  = "Face/Led/Blue/Right/135Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight180][LEDData::RED]   = "Face/Led/Red/Right/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight180][LEDData::GREEN] = "Face/Led/Green/Right/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight180][LEDData::BLUE]  = "Face/Led/Blue/Right/180Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight225][LEDData::RED]   = "Face/Led/Red/Right/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight225][LEDData::GREEN] = "Face/Led/Green/Right/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight225][LEDData::BLUE]  = "Face/Led/Blue/Right/225Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight270][LEDData::RED]   = "Face/Led/Red/Right/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight270][LEDData::GREEN] = "Face/Led/Green/Right/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight270][LEDData::BLUE]  = "Face/Led/Blue/Right/270Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight315][LEDData::RED]   = "Face/Led/Red/Right/315Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight315][LEDData::GREEN] = "Face/Led/Green/Right/315Deg/Actuator/Value";
  path_MultiLED[LEDData::FaceRight315][LEDData::BLUE]  = "Face/Led/Blue/Right/315Deg/Actuator/Value";

  // feet
  path_MultiLED[LEDData::FootLeft][LEDData::RED]       = "LFoot/Led/Red/Actuator/Value";
  path_MultiLED[LEDData::FootLeft][LEDData::GREEN]     = "LFoot/Led/Green/Actuator/Value";
  path_MultiLED[LEDData::FootLeft][LEDData::BLUE]      = "LFoot/Led/Blue/Actuator/Value";
  path_MultiLED[LEDData::FootRight][LEDData::RED]      = "RFoot/Led/Red/Actuator/Value";
  path_MultiLED[LEDData::FootRight][LEDData::GREEN]    = "RFoot/Led/Green/Actuator/Value";
  path_MultiLED[LEDData::FootRight][LEDData::BLUE]     = "RFoot/Led/Blue/Actuator/Value";

  // chest
  path_MultiLED[LEDData::ChestButton][LEDData::RED]    = "ChestBoard/Led/Red/Actuator/Value";
  path_MultiLED[LEDData::ChestButton][LEDData::GREEN]  = "ChestBoard/Led/Green/Actuator/Value";
  path_MultiLED[LEDData::ChestButton][LEDData::BLUE]   = "ChestBoard/Led/Blue/Actuator/Value";
}

DCM_led::~DCM_led()
{}

void DCM_led::init(boost::shared_ptr<ALBroker> pB)
{
  //connect to DCM
  try {
    dcm = boost::shared_ptr<AL::DCMProxy>(new DCMProxy(pB));
  } catch(ALError& e) {
    std::cerr << "Failed to init DCMHandler: " << e.what() << endl;
    return; // TODO: assert
  }
  
  // create alias to set all leds at once
  try
  {
    ALValue aliasCommand;
    aliasCommand.arraySetSize(2);

    aliasCommand[0] = string("AllLED");

    aliasCommand[1].arraySetSize(LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED);
    
    for(int i=0; i<LEDData::numOfMonoLED; ++i) {
      aliasCommand[1][i] = path_MonoLED[i];
    }
    for(int i=0; i<LEDData::numOfMultiLED; i++) {
      aliasCommand[1][LEDData::numOfMonoLED + 3*i  ] = path_MultiLED[i][LEDData::RED];
      aliasCommand[1][LEDData::numOfMonoLED + 3*i+1] = path_MultiLED[i][LEDData::GREEN];
      aliasCommand[1][LEDData::numOfMonoLED + 3*i+2] = path_MultiLED[i][LEDData::BLUE];
    }

    dcm->createAlias(aliasCommand);
  }
  catch(ALError& e)
  {
    std::cerr << "[NaoSMAL] Failed to create LED-Alias: " << e.what() << endl;
  }


  // create a command to set all leds
  allLedCommand.arraySetSize(6);
  allLedCommand[0] = string("AllLED");
  allLedCommand[1] = string("ClearAll");
  allLedCommand[2] = string("time-separate");
  allLedCommand[3] = 0;
  allLedCommand[4].arraySetSize(1);
  allLedCommand[4][0] = 0;

  allLedCommand[5].arraySetSize(LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED);
  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    allLedCommand[5][i].arraySetSize(1);
    allLedCommand[5][i][0] = 0.0;
  }
  for(int i=LEDData::numOfMonoLED;i<LEDData::numOfMonoLED + 3*LEDData::numOfMultiLED;i+=3)
  {
    allLedCommand[5][i].arraySetSize(1);
    allLedCommand[5][i][0] = 0.0;
    allLedCommand[5][i+1].arraySetSize(1);
    allLedCommand[5][i+1][0] = 0.0;
    allLedCommand[5][i+2].arraySetSize(1);
    allLedCommand[5][i+2][0] = 0.0;
  }


  // create a command to set a single led (for experiments)
  singleLedCommand.arraySetSize(3);
  singleLedCommand[1] = string("ClearAll");
  singleLedCommand[2].arraySetSize(1);
  singleLedCommand[2][0].arraySetSize(2);
  singleLedCommand[2][0][1] = 0;

}//end initLED


bool DCM_led::setLEDSmart(const LEDData& data, int dcmTime)
{
  for(int i=0;i<LEDData::numOfMonoLED;i++)
  {
    if(lastLEDData.theMonoLED[i] != data.theMonoLED[i])
    {
      // copy all
      lastLEDData = data;
      setAllLED(data, dcmTime);
      return true;
    }
  }

  for(int i=0;i<LEDData::numOfMultiLED;i++)
  {
    if( lastLEDData.theMultiLED[i][LEDData::RED]   != data.theMultiLED[i][LEDData::RED] ||
        lastLEDData.theMultiLED[i][LEDData::GREEN] != data.theMultiLED[i][LEDData::GREEN] ||
        lastLEDData.theMultiLED[i][LEDData::BLUE]  != data.theMultiLED[i][LEDData::BLUE])
    {
      // copy all
      lastLEDData = data;
      setAllLED(data, dcmTime);
      return true;
    }
  }//end for

  return false;
}//end setLEDSmart

bool DCM_led::setSingleLED(const LEDData& data, int dcmTime)
{
  bool result = false;
  try
  {
    singleLedCommand[2][0][1] = dcmTime;

    for(int i=0;i<LEDData::numOfMonoLED;i++)
    {
      if(lastLEDData.theMonoLED[i] != data.theMonoLED[i])
      {
        result = true;
        singleLedCommand[0] = path_MonoLED[i];
        singleLedCommand[2][0][0] = lastLEDData.theMonoLED[i] = data.theMonoLED[i];
        dcm->set(singleLedCommand);
      }
    }


    for(int i=0;i<LEDData::numOfMultiLED;i++)
    {
      for(int k=0;k<LEDData::numOfLEDColor;k++)
      {
        if(lastLEDData.theMultiLED[i][k] != data.theMultiLED[i][k])
        {
          result = true;
          singleLedCommand[0] = path_MultiLED[i][k];
          singleLedCommand[2][0][0] = lastLEDData.theMultiLED[i][k] = data.theMultiLED[i][k];
          dcm->set(singleLedCommand);
        }
      }
    }

  } catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to set LEDs: " << e.what() << endl;
  }
  return result;
}//end setSingleLED


void DCM_led::setAllLED(const LEDData& data, int dcmTime)
{
  allLedCommand[4][0] = dcmTime;

  for(int i=0;i<LEDData::numOfMonoLED;i++) {
    allLedCommand[5][i][0] = static_cast<float>(data.theMonoLED[i]);
  }
  for(int i=0; i<LEDData::numOfMultiLED; ++i) {
    allLedCommand[5][LEDData::numOfMonoLED + 3*i  ][0] = static_cast<float>(data.theMultiLED[i][LEDData::RED]);
    allLedCommand[5][LEDData::numOfMonoLED + 3*i+1][0] = static_cast<float>(data.theMultiLED[i][LEDData::GREEN]);
    allLedCommand[5][LEDData::numOfMonoLED + 3*i+2][0] = static_cast<float>(data.theMultiLED[i][LEDData::BLUE]);
  }

  try {
    dcm->setAlias(allLedCommand);
  } catch(ALError& e) {
    std::cerr << "[NaoSMAL] Failed to set LEDs: " << e.what() << endl;
  }
}//end setAllLED

