/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of NaothModule
*/
#include "NaothModule.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;

static NaothModule* theModule = NULL;

inline static void motion_wrapper_pre()
{
  theModule->motionCallbackPre();
}

inline static void motion_wrapper_post()
{
  theModule->motionCallbackPost();
}

NaothModule::NaothModule(ALPtr<ALBroker> pB, const std::string& pName ): 
  ALModule(pB, pName ),
  pBroker(pB),
  motionFrame(0)
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
  
  delete theCognition;
  delete theMotion;
 
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
  //int cognition_interval = 60000;
  int cognition_interval =   40000;

  cout << "Init OutputThread" << endl;
  
  cout << "Initializing Controller" << endl;
  NaoController::getInstance().init(pBroker);

  getParentBroker()->getProxy("DCM")->getModule()->atPreProcess(motion_wrapper_pre);
  getParentBroker()->getProxy("DCM")->getModule()->atPostProcess(motion_wrapper_post);

  cout << "Creating Cognition and Motion" << endl;
  theCognition = new Cognition();
  theMotion = new Motion();
  
  cout << "Registering Cognition and Motion" << endl;
  NaoController::getInstance().registerCallbacks(theMotion,theCognition);
  
  cout << "Creating Cognition-Thread" << endl;
  theCognitionThread = new CognitionThread();
  theCognitionThread->setPeriod(cognition_interval);
  //theCognitionThread->setRealtime(SCHED_RR,1);
  theCognitionThread->setRealtime(SCHED_OTHER,0);
  theCognitionThread->create();
  
  cout << "NaothModule:init finished!" << endl;
}

void NaothModule::motionCallbackPre()
{
  
  unsigned int timeStep = NaoController::getInstance().getBasicTimeStep();
  ASSERT(timeStep == 10 || timeStep == 20);

  if(timeStep == 10 || motionFrame % 2 == 0)
  {
    // we are at the moment shortly before the DCM commands are send to the
    // USB bus, so put the motion execute stuff here
    NaoController::getInstance().callMotion();
  }
}

void NaothModule::motionCallbackPost()
{

  unsigned int timeStep = NaoController::getInstance().getBasicTimeStep();
  ASSERT(timeStep == 10 || timeStep == 20);
  
  if(timeStep == 10 || motionFrame % 2 == 0)
  {
    // right behind the sensor update from the DCM
    // TODO: get stuff into internal buffers
  }
  motionFrame++;
}

void NaothModule::exit( )
{
  cout << "NaoTH is exiting ..."<<endl;
  
  theCognitionThread->stop();
  theCognitionThread->join();
  
  cout << "NaoTH exit is finished" << endl;
}
