/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformInterface_h_
#define _PlatformInterface_h_

#include "Callable.h"

#include "PlatformBase.h"
#include "Process.h"
#include "ProcessInterface.h"
#include "ActionList.h"
#include "PlatformDataInterface.h"
#include "MessageQueueHandler.h"
#include "ChannelActionCreator.h"

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

// EXPERIMENTAL
class Cognition;
class Motion;
extern Cognition* createCognition();
extern Motion* createMotion();
extern void deleteCognition(Cognition* cognition);
extern void deleteMotion(Motion* motion);

namespace naoth
{
class PlatformInterface;

// those are defined externally and linked later
extern void init_agent(PlatformInterface& platform);
}//


namespace naoth
{

/*
 * the platform interface responses for 4 kinds of functionalities:
 * - get sensor data
 * - set action data
 * - initialize the cognition module and motion module
 * - main loop to call cognition and motion
 */
class PlatformInterface:
  public PlatformBase,          // basic information about the platform
  public MessageQueueHandler,   //
  public PlatformDataInterface  // 
{
private:
  ProcessEnvironment environment;

  //
  Process cognitionProcess;
  Process motionProcess;
  Process soundProcess;

public:
  PlatformInterface(const std::string& name, unsigned int basicTimeStep)
    : 
    PlatformBase(name, basicTimeStep),
    PlatformDataInterface(environment)
  {
    PRINT_DEBUG("[PlatformInterface] NaoTH " << getName() << " starting...");
      
    //
    environment.channelActionCreator.setMessageQueueHandler(this);
  }

  virtual ~PlatformInterface() {
    PRINT_DEBUG("[PlatformInterface] destruct PlatformInterface");
  }

  /**
    * Register callback objects for this platform interface.
    * @param cognition The callback object for the cognition cycle or NULL if not active.
    */
  void registerCognition(Callable* cognition)
  {
    ProcessInterface processInterface(cognitionProcess, environment);
    cognitionProcess.callback = cognition;

    if(cognition != NULL) {
      PRINT_DEBUG("[PlatformInterface] register COGNITION callback");
      cognition->init(processInterface, *this);
    } else {
      std::cerr << "[PlatformInterface] COGNITION callback is NULL" << std::endl;
    }
  }//end registerCognition

  /**
    * Register callback objects for this platform interface.
    * @param sound The callback object for the sound cycle or NULL if not active.
    */
  void registerSound(Callable* sound)
  {
    ProcessInterface processInterface(soundProcess, environment);
    soundProcess.callback = sound;

    if(sound != NULL) {
      PRINT_DEBUG("[PlatformInterface] register SOUND callback");
      sound->init(processInterface, *this);
    } else {
      std::cerr << "[PlatformInterface] SOUND callback is NULL" << std::endl;
    }
  }//end registerSound


  /**
    * Register callback objects for this platform interface.
    * @param motion The callback object for the motion cycle or NULL if not active.
    */
  void registerMotion(Callable* motion)
  {
    ProcessInterface processInterface(motionProcess, environment);
    motionProcess.callback = motion;

    if(motion != NULL) {
      PRINT_DEBUG("[PlatformInterface] register MOTION callback");
      motion->init(processInterface, *this);
    } else {
      std::cerr << "[PlatformInterface] MOTION callback is NULL" << std::endl;
    }
  }//end registerMotion

  bool cognitionRegistered() {
    return cognitionProcess.callback != NULL;
  }

  virtual void runCognition()
  {
    if(cognitionRegistered())
    {
      getCognitionInput();
      callCognition();
      setCognitionOutput();
    }
  }//end callCognition

  virtual void callCognition() {
    if(cognitionRegistered()) {
      cognitionProcess.callback->call();
    }
  }

  virtual void getCognitionInput() {
    cognitionProcess.preActions.execute();
  }

  virtual void setCognitionOutput() {
    cognitionProcess.postActions.execute();
  }

  bool motionRegistered() {
    return motionProcess.callback != NULL;
  }

  virtual void runMotion()
  {
    if(motionRegistered())
    {
      getMotionInput();
      callMotion();
      setMotionOutput();
    }
  }//runMotion

  virtual void callMotion() {
    if(motionRegistered()) {
      motionProcess.callback->call();
    }
  }

  virtual void getMotionInput() {
    motionProcess.preActions.execute();
  }

  virtual void setMotionOutput() {
    motionProcess.postActions.execute();
  }
};//end class PlatformInterface

}//end namespace naoth

#endif  // _PlatformInterface_h_

