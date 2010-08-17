/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Krause, Thomas</a>
 */

#include "naorunner/PlatformInterface/PlatformInterface.h"

#include "naorunner/Tools/NaoTime.h"

using namespace naorunner;

PlatformInterface::PlatformInterface(const std::string& name, unsigned int basicTimeStep)
  :
  platformName(name),
  theBasicTimeStep(basicTimeStep),

  motionCallback(NULL),
  cognitionCallback(NULL)
{
  cout<<"NaoTH "<<platformName<<" starting..."<<endl;
}

PlatformInterface::~PlatformInterface()
{
}

void PlatformInterface::registerCallbacks(Callable* motionCallback, Callable* cognitionCallback)
{
  if(motionCallback != NULL)
  {
    std::cerr << "register MOTION callback" << std::endl;
    this->motionCallback = motionCallback;
    this->motionCallback->init(*this);
  }else
  {
    std::cerr << "could not register MOTION callback because it was NULL" << std::endl;
  }

  if(cognitionCallback != NULL)
  {
    std::cerr << "register COGNITION callback" << std::endl;
    this->cognitionCallback = cognitionCallback;
    this->cognitionCallback->init(*this);
  }else
  {
    std::cerr << "could not register COGNITION callback because it was NULL" << std::endl;
  }
}//end registerCallbacks


void PlatformInterface::callCognition()
{
  // TODO: assert?
  if(cognitionCallback != NULL)
  {
    getCognitionInput();
    cognitionCallback->call();
    setCognitionOutput();
  }
}//end callCognition

void PlatformInterface::getCognitionInput()
{
  execute(cognitionInput);
}

void PlatformInterface::setCognitionOutput()
{
  execute(cognitionOutput);
}


void PlatformInterface::callMotion()
{
  // TODO: assert?
  if(motionCallback != NULL)
  {
    getMotionInput();
    motionCallback->call();
    setMotionOutput();
  }
}//callMotion 

void PlatformInterface::getMotionInput()
{
  execute(motionInput);
}

void PlatformInterface::setMotionOutput()
{
  execute(motionOutput);
}