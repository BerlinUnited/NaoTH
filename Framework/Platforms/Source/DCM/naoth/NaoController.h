/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#ifndef _NAO_CONTROLLER_BASE_H_
#define _NAO_CONTROLLER_BASE_H_

#include <glib.h>

#include "PlatformInterface/PlatformInterface.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Process.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
#include "Tools/Debug/Stopwatch.h"


//
#include "V4lCameraHandler.h"
#include "SoundPlayer.h"
#include "SoundControl.h"
#include "Tools/Communication/Broadcast/BroadCaster.h"
#include "Tools/Communication/Broadcast/BroadCastListener.h"
#include "SPLGameController.h"
#include "DebugCommunication/DebugServer.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"

// local tools
#include "Tools/IPCData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemory.h"

#define TEAMCOMM_MAX_MSG_SIZE 4096

namespace naoth
{
class NaoController : public PlatformInterface<NaoController>
{
public:
  NaoController()
    :
    PlatformInterface<NaoController>("Nao", 10),
    sensorDataReading(NULL),
    m_naoSensorData(NULL),
    m_naoCommandMotorJointData(NULL),
    m_naoCommandUltraSoundSendData(NULL),
    m_naoCommandIRSendData(NULL),
    m_naoCommandLEDData(NULL),
    theSoundHandler(NULL),
    theBroadCaster(NULL),
    theBroadCastListener(NULL),
    theDebugServer(NULL)
  {
    staticMemberPath = Platform::getInstance().theConfigDirectory+"nao.info";

    // init shared memory
    const std::string naoSensorDataPath = "/nao_sensor_data";
    //const std::string naoCommandDataPath = "/nao_command_data";
    const std::string naoCommandMotorJointDataPath = "/nao_command.MotorJointData";
    const std::string naoCommandUltraSoundSendDataPath = "/nao_command.UltraSoundSendData";
    const std::string naoCommandIRSendDataPath = "/nao_command.IRSendData";
    const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

    //std::cout << "Opening Shared Memory: " << naoCommandDataPath << std::endl;
    //naoCommandData.open(naoCommandDataPath);
    std::cout << "Opening Shared Memory: " << naoCommandMotorJointDataPath << std::endl;
    naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
    std::cout << "Opening Shared Memory: " << naoCommandUltraSoundSendDataPath << std::endl;
    naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
    std::cout << "Opening Shared Memory: " << naoCommandIRSendDataPath << std::endl;
    naoCommandIRSendData.open(naoCommandIRSendDataPath);
    std::cout << "Opening Shared Memory: " << naoCommandLEDDataPath << std::endl;
    naoCommandLEDData.open(naoCommandLEDDataPath);

    std::cout << "Opening Shared Memory: " << naoSensorDataPath << std::endl;
    naoSensorData.open(naoSensorDataPath);

    // read the values from file
    ifstream is(staticMemberPath.c_str());
    //ASSERT(is.good());
    if(!is.good()) {
        THROW("is.good() failed. No Configs found. Do you call ../bin/naoth from ~/naoqi?");
    }

    is >> theBodyID >> theBodyNickName;
    cout << "bodyID: " << theBodyID << endl;
    cout << "bodyNickName: " << theBodyNickName << endl;


    /*  REGISTER IO  */
    // camera
    registerInput<Image>(*this);
    registerInput<CurrentCameraSettings>(*this);
    registerOutput<const CameraSettingsRequest>(*this);
    
    // sound
    registerOutput<const SoundPlayData>(*this);

    // gamecontroller
    registerInput<GameData>(*this);
    registerOutput<const GameReturnData>(*this);
    
    // teamcomm
    registerInput<TeamMessageDataIn>(*this);
    registerOutput<const TeamMessageDataOut>(*this);

    // debug comm
    registerInput<DebugMessageIn>(*this);
    registerOutput<const DebugMessageOut>(*this);

    // time
    registerInput<FrameInfo>(*this);

    // register sensor input
    registerInput<AccelerometerData>(*this);
    registerInput<SensorJointData>(*this);
    registerInput<FSRData>(*this);
    registerInput<GyrometerData>(*this);
    registerInput<InertialSensorData>(*this);
    registerInput<IRReceiveData>(*this);
    registerInput<ButtonData>(*this);
    registerInput<BatteryData>(*this);
    registerInput<UltraSoundReceiveData>(*this);

    // register command output
    registerOutput<const MotorJointData>(*this);
    registerOutput<const LEDData>(*this);
    registerOutput<const IRSendData>(*this);
    registerOutput<const UltraSoundSendData>(*this);


    /*  INIT DEVICES  */
    std::cout << "Init Platform" << endl;
    Platform::getInstance().init(this);

    std::cout << "Init CameraHandler" << endl;
    theCameraHandler.init("/dev/video");
  
    std::cout << "Init SoundHandler" <<endl;
    //theSoundPlayer.play("penalized");
    theSoundHandler = new SoundControl();

    std::cout<< "Init TeamComm"<<endl;
    naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
    string interfaceName = "wlan0";
    if(config.hasKey("teamcomm", "interface"))
    {
      interfaceName = config.getString("teamcomm", "interface");
    }

    unsigned int port = 10700;
    if(config.hasKey("teamcomm", "port"))
    {
      port = config.getInt("teamcomm", "port");
    }

    theBroadCaster = new BroadCaster(interfaceName, port);
    theBroadCastListener = new BroadCastListener(port, TEAMCOMM_MAX_MSG_SIZE);

    // start the debug server at the default debug port
    std::cout<< "Init DebugServer"<<endl;
    theDebugServer = new DebugServer();
    theDebugServer->start(5401, true);

    std::cout<< "Init SPLGameController"<<endl;
    theGameController = new SPLGameController();


    m_naoSensorData = g_mutex_new();
    m_naoCommandMotorJointData = g_mutex_new();
    m_naoCommandUltraSoundSendData = g_mutex_new();
    m_naoCommandIRSendData = g_mutex_new();
    m_naoCommandLEDData = g_mutex_new();
  }

  ~NaoController()
  {
//  delete theSoundHandler;
    delete theBroadCaster;
    delete theBroadCastListener;
    delete theGameController;
    delete theDebugServer;

    g_mutex_free(m_naoSensorData);
    g_mutex_free(m_naoCommandMotorJointData);
    g_mutex_free(m_naoCommandUltraSoundSendData);
    g_mutex_free(m_naoCommandIRSendData);
    g_mutex_free(m_naoCommandLEDData);

    // close the shared memory
    naoSensorData.close();
    naoCommandMotorJointData.close();
    naoCommandUltraSoundSendData.close();
    naoCommandIRSendData.close();
    naoCommandLEDData.close();
  }

  virtual string getBodyID() const { return theBodyID; }
  virtual string getBodyNickName() const { return theBodyNickName; }
  

  // camera stuff
  void get(Image& data) { theCameraHandler.get(data); }
  void get(CurrentCameraSettings& data) { theCameraHandler.getCameraSettings(data); }
  void set(const CameraSettingsRequest& data) { theCameraHandler.setCameraSettings(data, data.queryCameraSettings); }

  // sound
  void set(const SoundPlayData& data) 
  { 
    //if(data.soundFile.size() > 0) theSoundPlayer.play(data.soundFile); 
    theSoundHandler->setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theBroadCastListener->receive(data.data); }
  void set(const TeamMessageDataOut& data)
  {
    if ( data.data.size() <= TEAMCOMM_MAX_MSG_SIZE )
      theBroadCaster->send(data.data);
    else
      cerr << "NaoController: TeamMessageDataOut is too big " << data.data.size() << endl;
  }//end set TeamMessageDataOut

  // gamecontroller stuff
  void get(GameData& data)
  {
    theGameController->get(data, NaoTime::getNaoTimeInMilliSeconds());
    //data.frameNumber = theFrameInfo.getFrameNumber();
  }//end set GameData
  void set(const GameReturnData& data) { theGameController->setReturnData(data); }

  // debug comm
  void get(DebugMessageIn& data) { theDebugServer->getDebugMessageIn(data); }
  void set(const DebugMessageOut& data) { theDebugServer->setDebugMessageOut(data); }

  // time
  void get(FrameInfo& data)
  {
    data.setTime(NaoTime::getNaoTimeInMilliSeconds()); 
    data.setFrameNumber(data.getFrameNumber()+1);
  }

  void get(unsigned int& timestamp) { if (sensorDataReading!=NULL) timestamp = sensorDataReading->timeStamp; }

  

  // read directly from the shared memory
#define GET_FROM_SHARED_MEMORY(TYPE) \
  void get(TYPE& data)\
  { \
    g_mutex_lock(m_naoSensorData); \
    if (sensorDataReading!=NULL) \
      sensorDataReading->get(data); \
    g_mutex_unlock(m_naoSensorData); \
  }

  GET_FROM_SHARED_MEMORY(SensorJointData)
  GET_FROM_SHARED_MEMORY(AccelerometerData)
  GET_FROM_SHARED_MEMORY(GyrometerData)
  GET_FROM_SHARED_MEMORY(FSRData)
  GET_FROM_SHARED_MEMORY(InertialSensorData)
  GET_FROM_SHARED_MEMORY(IRReceiveData)
  GET_FROM_SHARED_MEMORY(ButtonData)
  GET_FROM_SHARED_MEMORY(BatteryData)
  GET_FROM_SHARED_MEMORY(UltraSoundReceiveData)

  // write directly to the shared memory
#define SET_TO_SHARED_MEMORY(TYPE) \
  void set(const TYPE& data)\
  { \
    g_mutex_lock(m_naoCommand##TYPE); \
    naoCommand##TYPE.writing()->set(data); \
    naoCommand##TYPE.swapWriting(); \
    g_mutex_unlock(m_naoCommand##TYPE); \
  }
  SET_TO_SHARED_MEMORY(MotorJointData)
  SET_TO_SHARED_MEMORY(LEDData)
  SET_TO_SHARED_MEMORY(IRSendData)
  SET_TO_SHARED_MEMORY(UltraSoundSendData)



  virtual void getMotionInput()
  {
    STOPWATCH_START("getMotionInput");
    // try to get some data from the DCM
    if ( naoSensorData.swapReading() )
    {
      sensorDataReading = naoSensorData.reading();
    }
    else // didn't get new sensor data
    {
      std::cerr << "didn't get new sensor data" << std::endl;
      sensorDataReading = NULL;
    }

    PlatformInterface<NaoController>::getMotionInput();
    STOPWATCH_STOP("getMotionInput");
  }//end getMotionInput


  virtual void setMotionOutput()
  {
    STOPWATCH_START("setMotionOutput");
    PlatformInterface<NaoController>::setMotionOutput();
    STOPWATCH_STOP("setMotionOutput");
    
    // send the data to DCM
    //naoCommandData.swapWriting();
  }//end setMotionOutput


  virtual void getCognitionInput()
  {
    STOPWATCH_START("getCognitionInput");
    PlatformInterface<NaoController>::getCognitionInput();
    STOPWATCH_STOP("getCognitionInput");
  }//end getCognitionInput
  

  void setCognitionOutput()
  {
    STOPWATCH_START("setCognitionOutput");
    PlatformInterface<NaoController>::setCognitionOutput();
    STOPWATCH_STOP("setCognitionOutput");
  }//end setCognitionOutput


protected:
  virtual MessageQueue* createMessageQueue(const std::string& name)
  {
    return new MessageQueue4Threads();
    //return new MessageQueue4Process(name);
  }

protected:
  //FrameInfo theFrameInfo;

  std::string staticMemberPath;
  std::string theBodyID;
  std::string theBodyNickName;


  const NaoSensorData* sensorDataReading;
  GMutex* m_naoSensorData;
  GMutex* m_naoCommandMotorJointData;
  GMutex* m_naoCommandUltraSoundSendData;
  GMutex* m_naoCommandIRSendData;
  GMutex* m_naoCommandLEDData;
  
  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  //SharedMemory<NaoCommandData> naoCommandData;
  SharedMemory<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemory<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemory<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemory<Accessor<LEDData> > naoCommandLEDData;

  //
  V4lCameraHandler theCameraHandler;
  SoundPlayer theSoundPlayer;
  SoundControl *theSoundHandler;
  BroadCaster* theBroadCaster;
  BroadCastListener* theBroadCastListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
