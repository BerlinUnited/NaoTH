/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of NaothModule
*/
#include "NaothModule.h"
//#include "PlatformInterface/Platform.h"
#include "libnaoth.h"
//#include "Tools/DummpyCallable.h"

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

NaothModule::NaothModule(ALPtr<ALBroker> pB, const std::string& pName ): 
  ALModule(pB, pName ),
  pBroker(pB)//,
  //theContorller(NULL),
  //theMotion(NULL)
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
  //if (!g_thread_supported())
  //  g_thread_init(NULL);


  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pBroker);
  
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

  /*
  cout << "Initializing Controller" << endl;
  theContorller = new NaoMotionController();
  theContorller->init(pBroker);

  cout << "Creating Motion" << endl;
  theMotion = new Motion();
  
  cout << "Registering Motion" << endl;
  theContorller->registerCallbacks(theMotion,(DummyCallable*)NULL);
  */
  fDCMPreProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(motion_wrapper_pre);
  fDCMPostProcessConnection = getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(motion_wrapper_post);
  
  cout << "NaothModule:init finished!" << endl;
}

void NaothModule::motionCallbackPre()
{
  // we are at the moment shortly before the DCM commands are send to the
  // USB bus, so put the motion execute stuff here
  //theContorller->callMotion();
  //theMotion->call();
  //theContorller->setMotionOutput();

}

void NaothModule::motionCallbackPost()
{
  // right behind the sensor update from the DCM
  //theContorller->getMotionInput();

  LibNaothData* libNaothDataWriting = libNaothData.writing();
  theDCMHandler.readSensorData(libNaothDataWriting->timeStamp, libNaothDataWriting->sensorsValue);
  libNaothDataReading = libNaothDataWriting;
  libNaothData.swapWriting();
}

void NaothModule::exit()
{
  cout << "NaoTH is exiting ..."<<endl;
  
  // stop motion
  /*
  while ( !theMotion->exit() )
  {
    usleep(100000);
  }
  */

  // Remove the call back connection
  fDCMPreProcessConnection.disconnect();
  fDCMPostProcessConnection.disconnect();

  theModule = NULL;
  /*
  delete theMotion;
  delete theContorller;
  */
  cout << "NaoTH exit is finished" << endl;
}
