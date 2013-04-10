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


// EXPERIMENTAL
class Cognition;
class Motion;
extern Cognition* createCognition();
extern Motion* createMotion();

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
  ProsessEnvironment environment;

  //
  Prosess cognitionProsess;
  Prosess motionProsess;

public:
  PlatformInterface(const std::string& name, unsigned int basicTimeStep)
    : 
    PlatformBase(name, basicTimeStep),
    PlatformDataInterface(environment)
  {
    std::cout << "NaoTH " << getName() << " starting..." << std::endl;
      
    //
    environment.channelActionCreator.setMessageQueueHandler(this);
  }

  virtual ~PlatformInterface() {
    std::cout << "destruct PlatformInterface" << std::endl;
  }

  /**
    * Register callback objects for this platform interface.
    * @param cognition The callback object for the cognition cycle or NULL if not active.
    */
  void registerCognition(Callable* cognition)
  {
    ProcessInterface processInterface(cognitionProsess, environment);
    cognitionProsess.callback = cognition;

    if(cognition != NULL) {
      std::cout << "register COGNITION callback" << std::endl;
      cognition->init(processInterface, *this);
    } else {
      std::cerr << "COGNITION callback is NULL" << std::endl;
    }
  }//end registerCognition


  /**
    * Register callback objects for this platform interface.
    * @param motion The callback object for the motion cycle or NULL if not active.
    */
  void registerMotion(Callable* motion)
  {
    ProcessInterface processInterface(motionProsess, environment);
    motionProsess.callback = motion;

    if(motion != NULL) {
      std::cerr << "register MOTION callback" << std::endl;
      motion->init(processInterface, *this);
    } else {
      std::cerr << "MOTION callback is NULL" << std::endl;
    }
  }//end registerMotion

  bool cognitionRegistered() {
    return cognitionProsess.callback != NULL;
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
      cognitionProsess.callback->call();
    }
  }

  virtual void getCognitionInput() {
    cognitionProsess.preActions.execute();
  }

  virtual void setCognitionOutput() {
    cognitionProsess.postActions.execute();
  }

  bool motionRegistered() {
    return motionProsess.callback != NULL;
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
      motionProsess.callback->call();
    }
  }

  virtual void getMotionInput() {
    motionProsess.preActions.execute();
  }

  virtual void setMotionOutput() {
    motionProsess.postActions.execute();
  }
};//end class PlatformInterface

}//end namespace naoth

#endif  // _PlatformInterface_h_

