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


NaothModule::NaothModule(ALPtr<ALBroker> pB, const std::string& pName )
  : 
  ALModule(pB, pName),
  state(IDLE),
  pBroker(pB),
  dcmTime(0),
  timeOffset(0),
  sem(SEM_FAILED)
{
  theModule = this;

  // Describe the module here
  setModuleDescription( "Naoth-controlunit of the robot" );
  
  // Define callable methods with there description
  functionName( "init", "NaothModule",  "Initialize Controller" );
  BIND_METHOD( NaothModule::init );
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


  // init shared memory
  //const std::string naoCommandDataPath = "/nao_command_data";
  const std::string naoSensorDataPath = "/nao_sensor_data";
  //std::cout << "Opening Shared Memory: "<<naoCommandDataPath<<std::endl;
  //naoCommandData.open(naoCommandDataPath);
  std::cout<< "Opening Shared Memory: "<<naoSensorDataPath<<std::endl;
  naoSensorData.open(naoSensorDataPath);

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


  // open semaphore
  if((sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("libnaoth: sem_open");
  }

  // connect to DCM
  fDCMPreProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(motion_wrapper_pre);
  fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(motion_wrapper_post);
  
  cout << "NaothModule:init finished!" << endl;
}//end init


void NaothModule::motionCallbackPre()
{
  // update the dcm time
  dcmTime = NaoTime::getNaoTimeInMilliSeconds() + timeOffset;

  // we are at the moment shortly before the DCM commands are send to the
  // USB bus, so put the motion execute stuff here
  static int drop_count = 1000;


  bool stiffness_set = false;

  // get the MotorJointData from the shared memory and put them to the DCM
  if ( naoCommandMotorJointData.swapReading() )
  {
    const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
    
    theDCMHandler.setAllPositionData(commandData->get(), dcmTime);
    stiffness_set = theDCMHandler.setAllHardnessDataSmart(commandData->get(), dcmTime);

    drop_count = 0;
  }
  else
  {
    if(drop_count == 0)
      fprintf(stderr, "libnaoth: dropped comand data.\n");
    else if(drop_count == 10)
      fprintf(stderr, "libnaoth: I think the core is dead.\n");
    else if(drop_count > 10)
      setWarningLED();

    // don't count more than 11
    drop_count += (drop_count < 11);
  }//end else

  bool us_set = false;

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

  // get the LEDData from the shared memory and put them to the DCM
  // don't set too many things at once
  if(naoCommandLEDData.swapReading())
  {
    const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
    theDCMHandler.setSingleLED(commandData->get(), dcmTime);
  }//end if
}//end motionCallbackPre


void NaothModule::motionCallbackPost()
{
  static int drop_count = 1000;

  NaoSensorData* sensorData = naoSensorData.writing();

  sensorData->timeStamp = dcmTime;

  // read the sensory data from DCM to the shared memory
  theDCMHandler.readSensorData(sensorData->sensorsValue);
  
  // 
  naoSensorData.swapWriting();


  // raise the semaphore: triggers core
  if(sem != SEM_FAILED)
  {
    int sval;
    if(sem_getvalue(sem, &sval) == 0)
    {
      if(sval < 1)
      {
        sem_post(sem);
        drop_count = 0;
      }
      else
      {
        if(drop_count == 0)
          fprintf(stderr, "libnaoth: dropped sensor data.\n");
        else if(drop_count == 10)
          fprintf(stderr, "libnaoth: I think the core is dead.\n");
    
        // don't count more than 11
        drop_count += (drop_count < 11);
      }//end if
    }//end if
  }//end if

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

  // Remove the call back connection
  fDCMPreProcessConnection.disconnect();
  fDCMPostProcessConnection.disconnect();

  cout << "NaoTH exit is finished" << endl;
}//end exit


void NaothModule::setWarningLED()
{
  static naoth::LEDData theLEDData;
  static int count = 0;
  
  theLEDData.change = true;

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

