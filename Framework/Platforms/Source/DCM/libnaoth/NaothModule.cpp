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
  const std::string naoCommandDataPath = "/nao_command_data";
  const std::string naoSensorDataPath = "/nao_sensor_data";
  std::cout << "Opening Shared Memory: "<<naoCommandDataPath<<std::endl;
  naoCommandData.open(naoCommandDataPath);
  std::cout<< "Opening Shared Memory: "<<naoSensorDataPath<<std::endl;
  naoSensorData.open(naoSensorDataPath);


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
  // we are at the moment shortly before the DCM commands are send to the
  // USB bus, so put the motion execute stuff here
  static int drop_count = 1000;

  // get the data from the shared memory and put them to the DCM
  if ( naoCommandData.swapReading() )
  {
    const NaoCommandData* commandData = naoCommandData.reading();
    // update the dcm time
    dcmTime = NaoTime::getNaoTimeInMilliSeconds() + timeOffset;

    theDCMHandler.setAllPositionData(commandData->motorJointData(), dcmTime);
    theDCMHandler.setAllHardnessData(commandData->motorJointData(), dcmTime);

    theDCMHandler.setLED(commandData->lEDData(), dcmTime);
    theDCMHandler.setIRSend(commandData->iRSendData(), dcmTime);
    theDCMHandler.setUltraSoundSend(commandData->ultraSoundSendData(), dcmTime);

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
  }
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
  naoCommandData.close();

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

  theDCMHandler.setLED(theLEDData, dcmTime);
}//end checkWarningState

