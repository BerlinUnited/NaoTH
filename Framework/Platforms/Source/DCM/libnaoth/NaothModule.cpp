/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Implementation of NaothModule
*/
#include "NaothModule.h"
#include "libnaoth.h"
#include "Tools/NaoTime.h"
#include <glib.h>
#include <glib-object.h>

using namespace naoth;


static NaothModule* theModule = NULL;

inline static void motion_wrapper_pre()
{
  if (theModule != NULL)
    theModule->motionCallbackPre();
}

inline static void motion_wrapper_post()
{
  if (theModule != NULL)
    theModule->motionCallbackPost();
}


// some low level debugging stuff
int current_line = 0;
int time_motionCallbackPre = 0;
int time_motionCallbackPost = 0;

//#define LNT current_line=__LINE__
#define LNT (void)0

void* debug_wrepper(void* ref)
{
  while(true)
  {
    int c = time_motionCallbackPre;
    if(c > 500) // longer than 1ms
      std::cout << "time_motionCallbackPre: " << c << std::endl;

    c = time_motionCallbackPost;
    if(c > 500) // longer than 1ms
      std::cout << "time_motionCallbackPost: " << c << std::endl;

    usleep(1000000);
  }
  return NULL;
}//end debug_wrepper


NaothModule::NaothModule(ALPtr<ALBroker> pB, const std::string& pName )
  :
  ALModule(pB, pName),
  state(DISCONNECTED),
  pBroker(pB),
  dcmTime(0),
  timeOffset(0),
  sem(SEM_FAILED),
  command_data_available(false),
  sensor_data_available(false),
  initialMotion(NULL)
{
  theModule = this;

  // Describe the module here
  setModuleDescription( "Naoth-controlunit of the robot" );
  
  // Define callable methods with there description
  functionName( "init", "NaothModule",  "Initialize Controller" );
  BIND_METHOD( NaothModule::init );

  GError* err = NULL;
  g_thread_create(debug_wrepper, 0, true, &err);
  if(err)
  {
    g_warning("Could not create cognition thread: %s", err->message);
  }
}

NaothModule::~NaothModule()
{
}

bool NaothModule::innerTest() 
{
  bool result = true;
  // put here codes dedicaced to autotest this module.
  // return false if fail, success otherwise
  return result;
}

std::string NaothModule::version()
{
  //return ALTOOLS_VERSION( NAOTH );
  return std::string(NAOTH_VERSION);
}

void NaothModule::init()
{
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pBroker);

  unsigned int delta = 0;
  dcmTime = theDCMHandler.getTime(delta);
  timeOffset = dcmTime - NaoTime::getNaoTimeInMilliSeconds();
  
  // save the body ID
  string theBodyID = theDCMHandler.getBodyID();
  std::cout << "bodyID: "<< theBodyID << endl;
  
  // save the nick name
  string theBodyNickName = theDCMHandler.getBodyNickName();
  std::cout << "nickName: "<< theBodyNickName << endl;

  // save the value to file
  // FIXME: fixed path "Config/nao.info"
  string staticMemberPath("Config/nao.info");
  ofstream os(staticMemberPath.c_str());
  ASSERT(os.good());
  os<<theBodyID<<"\n"<<theBodyNickName<<endl;
  os.close();

  // open semaphore
  if((sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("libnaoth: sem_open");
    ::exit(-1);
  }


  // init shared memory
  //const std::string naoCommandDataPath = "/nao_command_data";
  const std::string naoSensorDataPath = "/nao_sensor_data";
  //std::cout << "Opening Shared Memory: "<<naoCommandDataPath<<std::endl;
  //naoCommandData.open(naoCommandDataPath);
  std::cout<< "Opening Shared Memory: "<<naoSensorDataPath<<std::endl;
  naoSensorData.open(naoSensorDataPath);

  debugSM.open("/debug_data");

  const std::string naoCommandMotorJointDataPath = "/nao_command.MotorJointData";
  const std::string naoCommandUltraSoundSendDataPath = "/nao_command.UltraSoundSendData";
  const std::string naoCommandIRSendDataPath = "/nao_command.IRSendData";
  const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

  std::cout << "Opening Shared Memory: " << naoCommandMotorJointDataPath << std::endl;
  naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
  std::cout << "Opening Shared Memory: " << naoCommandUltraSoundSendDataPath << std::endl;
  naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
  std::cout << "Opening Shared Memory: " << naoCommandIRSendDataPath << std::endl;
  naoCommandIRSendData.open(naoCommandIRSendDataPath);
  std::cout << "Opening Shared Memory: " << naoCommandLEDDataPath << std::endl;
  naoCommandLEDData.open(naoCommandLEDDataPath);


  // connect to DCM
  fDCMPreProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(motion_wrapper_pre);
  fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(motion_wrapper_post);
  
  cout << "NaothModule:init finished!" << endl;
}//end init


void NaothModule::motionCallbackPre()
{
  long long start = NaoTime::getSystemTimeInMicroSeconds();
  LNT;

  // update the dcm time
  dcmTime = NaoTime::getNaoTimeInMilliSeconds() + timeOffset;

  // we are at the moment shortly before the DCM commands are send to the
  // USB bus, so put the motion execute stuff here
  static int drop_count = 10;

  LNT;
  // what to do when the Controller is dead?
  if(runningEmergencyMotion())
  {
    return;
  }
  LNT;

  bool stiffness_set = false;

  // get the MotorJointData from the shared memory and put them to the DCM
  if ( naoCommandMotorJointData.swapReading() )
  {
    LNT;
    const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
    
    theDCMHandler.setAllPositionData(commandData->get(), dcmTime);
    stiffness_set = theDCMHandler.setAllHardnessDataSmart(commandData->get(), dcmTime);

    drop_count = 0;
    command_data_available = true;
  }
  else
  {
    LNT;
    if(drop_count == 0)
      fprintf(stderr, "libnaoth: dropped comand data.\n");

    // don't count more than 11
    drop_count += (drop_count < 11);
  }//end else

  bool us_set = false;

  LNT;
  // get the UltraSoundSendData from the shared memory and put them to the DCM
  if ( naoCommandUltraSoundSendData.swapReading() )
  {
    const Accessor<UltraSoundSendData>* commandData = naoCommandUltraSoundSendData.reading();
    us_set = theDCMHandler.setUltraSoundSendSmart(commandData->get(), dcmTime);
  }//end if
  
  /*
  if ( naoCommandIRSendData.swapReading() )
  {
    const Accessor<IRSendData>* commandData = naoCommandIRSendData.reading();
    theDCMHandler.setIRSendData(commandData->get(), dcmTime);
  }//end if
  */
  LNT;
  // get the LEDData from the shared memory and put them to the DCM
  // don't set too many things at once
  if(!stiffness_set && !us_set && naoCommandLEDData.swapReading())
  {
    const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
    theDCMHandler.setSingleLED(commandData->get(), dcmTime);
  }//end if


  long long stop = NaoTime::getSystemTimeInMicroSeconds();
  time_motionCallbackPre = (int)(stop - start);
}//end motionCallbackPre


void NaothModule::motionCallbackPost()
{
  long long start = NaoTime::getSystemTimeInMicroSeconds();
  LNT;
  static int drop_count = 10;

  NaoSensorData* sensorData = naoSensorData.writing();

  // current system time (System time, not nao time (!))
  sensorData->timeStamp = NaoTime::getSystemTimeInMilliSeconds();

  // read the sensory data from DCM to the shared memory
  theDCMHandler.readSensorData(sensorData->sensorsValue);

  // save the data for the emergency case
  if(state == DISCONNECTED)
  {
    sensorData->get(theInertialSensorData);
    sensor_data_available = true;
  }
  else
  {
    sensor_data_available = false;
  }

  LNT;
  // 
  naoSensorData.swapWriting();

  LNT;
  // raise the semaphore: triggers core
  if(sem != SEM_FAILED)
  {
    LNT;
    int sval;
    if(sem_getvalue(sem, &sval) == 0)
    {
      LNT;
      if(sval < 1)
      {
        LNT;
        sem_post(sem);

        LNT;
        if(state == DISCONNECTED)
          fprintf(stderr, "libnaoth: I think the core is alive.\n");

        drop_count = 0;
        state = CONNECTED;
      }
      else
      {
        LNT;
        if(drop_count == 0)
          fprintf(stderr, "libnaoth: dropped sensor data.\n");
        else if(drop_count == 10)
        {
          fprintf(stderr, "libnaoth: I think the core is dead.\n");
          state = DISCONNECTED;
        }

        // don't count more than 11
        drop_count += (drop_count < 11);
      }//end if
    }
    else
    {
      fprintf(stderr, "libnaoth: I couldn't get value by sem_getvalue.\n");
    }
  }//end if

  long long stop = NaoTime::getSystemTimeInMicroSeconds();
  time_motionCallbackPost = (int)(stop - start);
}//end motionCallbackPost


void NaothModule::exit()
{
  cout << "NaoTH is exiting ..."<<endl;

  // close semaphore
  if(sem != SEM_FAILED)
  {
    sem_close(sem);
    sem = SEM_FAILED;
  }

  // close the shared memory
  naoSensorData.close();
  naoCommandMotorJointData.close();
  naoCommandUltraSoundSendData.close();
  naoCommandIRSendData.close();
  naoCommandLEDData.close();

  // set all stiffness to 0
  theDCMHandler.setAllHardnessData(0.0, dcmTime);

  // Remove the call back connection
  fDCMPreProcessConnection.disconnect();
  fDCMPostProcessConnection.disconnect();

  cout << "NaoTH exit is finished" << endl;
}//end exit


void NaothModule::setWarningLED()
{
  static naoth::LEDData theLEDData;
  static int count = 0;
  
  int begin = ((++count)/10)%10;
  theLEDData.theMonoLED[LEDData::EarRight0 + begin] = 0;
  theLEDData.theMonoLED[LEDData::EarLeft0 + begin] = 0;
  int end = (begin+2)%10;
  theLEDData.theMonoLED[LEDData::EarRight0 + end] = 1;
  theLEDData.theMonoLED[LEDData::EarLeft0 + end] = 1;

  for(int i=0; i<LEDData::numOfMultiLED; i++)
  {
    theLEDData.theMultiLED[i][LEDData::RED] = 0;
    theLEDData.theMultiLED[i][LEDData::GREEN] = 0;
    theLEDData.theMultiLED[i][LEDData::BLUE] = 1;
  }//end for

  theDCMHandler.setSingleLED(theLEDData, dcmTime);
}//end checkWarningState


bool NaothModule::runningEmergencyMotion()
{
  if(state == DISCONNECTED)
  {
    LNT;
    if(initialMotion == NULL && command_data_available && sensor_data_available)
    {
      // take the last command data
      const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
      initialMotion = new BasicMotion(theMotorJointData, commandData->get(), theInertialSensorData);
    }//end if

    setWarningLED();
  }//end if


  // after reconnect: wait until the init motion is finished
  if(initialMotion != NULL)
  {
    LNT;
    if(state == CONNECTED && initialMotion->isFinish())
    {
      delete initialMotion;
      initialMotion = NULL;
    }
    else
    {
      initialMotion->execute();
      theDCMHandler.setAllPositionData(theMotorJointData, dcmTime);
      theDCMHandler.setAllHardnessDataSmart(theMotorJointData, dcmTime);
    }
  }//end if
  
  return initialMotion != NULL;
}//end runningEmergencyMotion


