/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformInterface_h_
#define _PlatformInterface_h_

#include "Callable.h"


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
  * the PlatformBase holds (and provides access to) 
  * some basic information about the platform
  */
class PlatformBase
{
public:
  PlatformBase(const std::string& platformName, unsigned int basicTimeStep)
    :
    platformName(platformName),
    theBasicTimeStep(basicTimeStep)
  {}

  virtual ~PlatformBase();

  /////////////////////// get ///////////////////////
  virtual std::string getBodyID() const = 0;
  virtual std::string getBodyNickName() const = 0;
  virtual std::string getHeadNickName() const = 0;

  inline const std::string& getName() const { return platformName; }
  inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

private:
  std::string platformName;
  unsigned int theBasicTimeStep;
};//end class PlatformBase


/*
 * the platform interface responses for 4 kinds of functionalities:
 * - get sensor data
 * - set action data
 * - initialize the cognition module and motion module
 * - main loop to call cognition and motion
 */
class PlatformInterface:
  public PlatformBase,          // basic information about the platform
  public PlatformDataInterface, // 
  public MessageQueueHandler    //
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

  /**
    * Register callback objects for this platform interface.
    * @param cognition The callback object for the cognition cycle or NULL if not active.
    */
  void registerCognition(Callable* cognition)
  {
    ProcessInterface processInterface(cognitionProsess, environment);
    cognitionProsess.callback = cognition;

    if(cognition != NULL)
    {
      std::cout << "register COGNITION callback" << std::endl;
      cognition->init(processInterface, *this);
    }
    else
    {
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

    if(motion != NULL)
    {
      std::cerr << "register MOTION callback" << std::endl;
      motion->init(processInterface, *this);
    }
    else
    {
      std::cerr << "MOTION callback is NULL" << std::endl;
    }
  }//end registerMotion


  virtual void callCognition()
  {  
    // TODO: assert?
    if(cognitionProsess.callback != NULL)
    {
      getCognitionInput();
      cognitionProsess.callback->call();
      setCognitionOutput();
    }
  }//end callCognition

  virtual void getCognitionInput()
  {
    cognitionProsess.preActions.execute();
  }

  virtual void setCognitionOutput()
  {
    cognitionProsess.postActions.execute();
  }

  virtual void callMotion()
  {
    // TODO: assert?
    if(motionProsess.callback != NULL)
    {
      getMotionInput();
      motionProsess.callback->call();
      setMotionOutput();
    }
  }//callMotion 

  virtual void getMotionInput()
  {
    motionProsess.preActions.execute();
  }

  virtual void setMotionOutput()
  {
    motionProsess.postActions.execute();
  }
};//end class PlatformInterface

}//end namespace naoth

#endif  // _PlatformInterface_h_

