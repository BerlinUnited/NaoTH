/**
* @file SMALModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Implementation of SMALModule
*/
#include "SMALModule.h"

#include "Tools/NaoTime.h"
#include <fstream>
#include <cstdlib>

using namespace naoth;
using namespace std;
using namespace AL;

// some low level debugging stuff
#ifdef DEBUG_SMAL
#define LNT current_line=__LINE__
int current_line = 0;
#else
#define LNT (void)0
#endif


// this stuff is necessary to register at the DCM
static SMALModule* theModule = NULL;

static void motion_wrapper_pre()
{
  assert(theModule != NULL);
  theModule->motionCallbackPre();
}

static void motion_wrapper_post()
{
  assert(theModule != NULL);
  theModule->motionCallbackPost();
}

void* slowDCMCycle(void* ref)
{
  while(true) 
  {
    SMALModule* smalModule = (SMALModule*)ref;
    smalModule->slowDcmUpdate();

    pthread_yield();
    usleep(1000);
  }
  
  return NULL;
}


SMALModule::SMALModule(boost::shared_ptr<ALBroker> pBroker, const std::string& pName )
  :
  ALModule(pBroker, pName),
  pBroker(pBroker),
  state(DISCONNECTED),
  dcmTime(0),
  timeOffset(0),
  sem(SEM_FAILED),
  slowDCM(0),
  slowDCMupdateCanRun(false),
  command_data_available(false),
  sensor_data_available(false),
  shutdown_requested(false),
  initialMotion(NULL)
{

  // there should be only one instance of SMALModule
  assert(theModule == NULL);
  theModule = this;

  // 
  setModuleDescription( 
  "Nao Shared Memory Abstraction Layer (NaoSMAL)" 
  "provides access to the HAL functionality of naoqi through shared memory" 
  );
}

SMALModule::~SMALModule()
{
  if(shutdownCallbackThread.joinable()) {
    shutdownCallbackThread.join();
  }
}

std::string SMALModule::version()
{
  return std::string(SMAL_VERSION);
}

void SMALModule::init()
{
  std::cout << "Init DCMHandler" << endl;

  std::cout << "resetting working directory to /home/nao/" << std::endl;
  int retChDir = chdir("/home/nao");
  if(retChDir != 0)
  {
    std::cerr << "Could not change working directory" << std::endl;
  }

  theDCMHandler.init(pBroker);

  // calculate the difference between the NaoTime and the DcmTime
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
  {
    string staticMemberPath("Config/nao.info");
    ofstream os(staticMemberPath.c_str());
    ASSERT(os.good());
    os<<theBodyID<<"\n"<<theBodyNickName<<endl;
    os.close();
  }

  // NOTE: read the joint limits from the dcm and write it to the config
  theDCMHandler.getJointPositionLimits(theMotorJointData);
  {
    ofstream os("Config/private/joints.cfg");
    ASSERT(os.good());
    os << "[joints]" << endl << endl;
    os << "# auto generated joint limits for " << theBodyNickName << endl << endl;
    for(int i=0;i<JointData::numOfJoint;i++)
    {
      double joint_min = theMotorJointData.min[i];
      double joint_max = theMotorJointData.max[i];

      // NOTE: we convert all the joint limits to degrees to be stored in the config
      //       except for the hand, because those limits are just {0,1} ~ {close,open}
      double joint_max_config = (i == JointData::LHand || i == JointData::RHand)?joint_max:Math::toDegrees(joint_max);
      double joint_min_config = (i == JointData::LHand || i == JointData::RHand)?joint_min:Math::toDegrees(joint_min);

      os << JointData::getJointName((JointData::JointID) i) << "Min = " << joint_min_config << ";" << endl;
      os << JointData::getJointName((JointData::JointID) i) << "Max = " << joint_max_config << ";" << endl;
      os << endl;
    }
    os.close();
  }

  //key_t semkey = ftok("/dev/null",1);

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
  
  cout << "SMALModule:init finished!" << endl;

  pthread_create(&slowDCM, 0, slowDCMCycle, (void*)this);
}//end init

void SMALModule::shutdownCallback()
{
  // play a sound that the user knows we recognized his shutdown request
  system("/usr/bin/paplay /usr/share/naoqi/wav/bip_gentle.wav");

  // stop the user program
  std::cout << "stopping naoth" << std::endl;
  system("naoth stop");

  sleep(5);

  // we are the child process, do a blocking call to shutdown
  system("/sbin/shutdown -h now");
  std::cout << "System shutdown requested" << std::endl;

  // await termination
  while(true) {
    sleep(100);
  }

}//end soundThreadCallback

void SMALModule::slowDcmUpdate()
{
  if(!slowDCMupdateCanRun) {
    return;
  }

  int dcmTime = theDCMHandler.getTime(0);

  if(naoCommandLEDData.swapReading())
  {
    const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
    //theDCMHandler.setSingleLED(commandData->get(), dcmTime);
    theDCMHandler.setLED(commandData->get(), dcmTime);
  }

  // get the UltraSoundSendData from the shared memory and put them to the DCM
  if (naoCommandUltraSoundSendData.swapReading() )
  {
    const Accessor<UltraSoundSendData>* commandData = naoCommandUltraSoundSendData.reading();
    theDCMHandler.setUltraSoundSend(commandData->get(), dcmTime);
  }

  slowDCMupdateCanRun = false;
}

// we are at the moment shortly before the DCM commands are send to the
// USB bus, so put the motion execute stuff here
void SMALModule::motionCallbackPre()
{
#ifdef DEBUG_SMAL
  long long start = NaoTime::getSystemTimeInMicroSeconds();
  counterPre++;
  if(counterPre == 100) {
    long long currentPreTime = NaoTime::getSystemTimeInMicroSeconds();
    long long delta_cycle = (currentPreTime - lastPreTime)/counterPre;
    long long delta_execTime = time_motionCallbackPreSum/counterPre;
    std::cout << "[pre]\t" << delta_cycle <<"\t"<< delta_execTime <<std::endl;
    lastPreTime = NaoTime::getSystemTimeInMicroSeconds();
    counterPre = 0;
    time_motionCallbackPreSum = 0;
  }
#endif

  static int drop_count = 10;

  // update the dcm time: NaoTime + (offset to DCM time)
  dcmTime = NaoTime::getNaoTimeInMilliSeconds() + timeOffset;

  // what to do when the Controller is dead?
  if(runningEmergencyMotion())
  {
#ifdef DEBUG_SMAL
    long long stop = NaoTime::getSystemTimeInMicroSeconds();
    time_motionCallbackPreSum = (int)(stop - start);
#endif
    return;
  }

  //bool stiffness_set = false;

  // get the MotorJointData from the shared memory and put them to the DCM
  if ( naoCommandMotorJointData.swapReading() )
  {
    const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
    
    theDCMHandler.setAllPositionData(commandData->get(), dcmTime);
    //stiffness_set = 
    theDCMHandler.setAllHardnessDataSmart(commandData->get(), dcmTime);

    drop_count = 0;
    command_data_available = true;
  }
  else
  {
    if(drop_count == 0) {
      fprintf(stderr, "libnaoth: dropped comand data.\n");
    }

    // don't count more than 11
    drop_count += (drop_count < 11);
  }//end else
  
  /*
  if ( naoCommandIRSendData.swapReading() )
  {
    const Accessor<IRSendData>* commandData = naoCommandIRSendData.reading();
    theDCMHandler.setIRSendData(commandData->get(), dcmTime);
  }//end if
  */

  // NOTE: the LEDs are only set if stiffness was not set in this cycle
  // get the LEDData from the shared memory and put them to the DCM
  // !stiffness_set && 

  /*
  if(naoCommandLEDData.swapReading())
  {
    const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
    //theDCMHandler.setSingleLED(commandData->get(), dcmTime);
    //theDCMHandler.setLED(commandData->get(), dcmTime);
    theDCMHandler.lastLEDData = commandData->get();
  }
  theDCMHandler.setLED(theDCMHandler.lastLEDData, dcmTime);
  
  // get the UltraSoundSendData from the shared memory and put them to the DCM
  if (naoCommandUltraSoundSendData.swapReading() )
  {
    const Accessor<UltraSoundSendData>* commandData = naoCommandUltraSoundSendData.reading();
    theDCMHandler.setUltraSoundSend(commandData->get(), dcmTime);
  }
  */
  

#ifdef DEBUG_SMAL
  long long stop = NaoTime::getSystemTimeInMicroSeconds();
  time_motionCallbackPreSum += (int)(stop - start);
#endif
}//end motionCallbackPre


void SMALModule::motionCallbackPost()
{
#ifdef DEBUG_SMAL
  long long start = NaoTime::getSystemTimeInMicroSeconds();
  counterPost++;
  if(counterPost == 100) {
    long long currentPostTime = NaoTime::getSystemTimeInMicroSeconds();
    long long delta_cycle = (currentPostTime - lastPostTime)/counterPost;
    long long delta_execTime = time_motionCallbackPostSum/counterPost;
    std::cout << "[post]\t" << delta_cycle <<"\t"<< delta_execTime <<std::endl;
    lastPostTime = NaoTime::getSystemTimeInMicroSeconds();
    counterPost = 0;
    time_motionCallbackPostSum = 0;
  }
#endif

  static int drop_count = 10;

  NaoSensorData* sensorData = naoSensorData.writing();

  // current system time (System time, not nao time (!))
  sensorData->timeStamp = NaoTime::getSystemTimeInMilliSeconds();

  // read the sensory data from DCM to the shared memory
  theDCMHandler.readSensorData(sensorData->sensorsValue);

  // check if chest button was pressed as a request to shutdown
  // each cycle needs 10ms so if the button was pressed for 30 seconds
  // these are 300 frames
  sensorData->get(theButtonData);
  if(!shutdown_requested && theButtonData.numOfFramesPressed[ButtonData::Chest] > 300)
  {
    shutdown_requested = true;

    shutdownCallbackThread = std::thread([this]{this->shutdownCallback();});
  }

  // save the data for the emergency case
  if(state == DISCONNECTED) {
    sensorData->get(theInertialSensorData);
    sensor_data_available = true;
  } else {
    sensor_data_available = false;
  }

  // push the data to shared memory
  naoSensorData.swapWriting();

  // allow the slow thread to run after all sensor data has been fetched
  slowDCMupdateCanRun = true;
  
  // raise the semaphore: triggers core
  if(sem != SEM_FAILED)
  {
    int sval;
    if(sem_getvalue(sem, &sval) == 0)
    {
      if(sval < 1)
      {
        sem_post(sem);

        if(state == DISCONNECTED) {
          fprintf(stderr, "libnaoth: I think the core is alive.\n");
    }

        drop_count = 0;
        state = CONNECTED;
      }
      else
      {
        if(drop_count == 0) {
          fprintf(stderr, "libnaoth: dropped sensor data.\n");
    } else if(drop_count == 10) {
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
  }//end if SEM_FAILED

#ifdef DEBUG_SMAL
  long long stop = NaoTime::getSystemTimeInMicroSeconds();
  time_motionCallbackPostSum += (int)(stop - start);
#endif
}//end motionCallbackPost


void SMALModule::exit()
{
  cout << "NaoTH is exiting ..." << endl;

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


void SMALModule::setWarningLED(bool red)
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
    theLEDData.theMultiLED[i][LEDData::RED] = red ? 1 : 0;
    theLEDData.theMultiLED[i][LEDData::GREEN] = 0;
    theLEDData.theMultiLED[i][LEDData::BLUE] = red ? 0 : 1;
  }

  theDCMHandler.setSingleLED(theLEDData, dcmTime);
}//end checkWarningState


bool SMALModule::runningEmergencyMotion()
{
  if(state == DISCONNECTED)
  {
    LNT;
    if(initialMotion == NULL && command_data_available && sensor_data_available)
    {
      // take the last command data
      const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
      initialMotion = new BasicMotion(theMotorJointData, commandData->get(), theInertialSensorData);
    }

    setWarningLED(shutdown_requested);
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


