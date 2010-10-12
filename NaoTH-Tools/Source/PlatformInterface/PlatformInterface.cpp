/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Krause, Thomas</a>
 */

#include "PlatformInterface/PlatformInterface.h"

using namespace naoth;

PlatformDataInterface::PlatformDataInterface()
  :
  motionCallback(NULL),
  cognitionCallback(NULL)
{
}

PlatformDataInterface::~PlatformDataInterface()
{
  cout<<"NaoTH clean..."<<endl;
  delete_action_list(motionInput);
  delete_action_list(motionOutput);
  delete_action_list(cognitionInput);
  delete_action_list(cognitionOutput);
}


void PlatformDataInterface::callCognition()
{
  // TODO: assert?
  if(cognitionCallback != NULL)
  {
    getCognitionInput();
    cognitionCallback->call();
    setCognitionOutput();
  }
}//end callCognition

void PlatformDataInterface::getCognitionInput()
{
  execute(cognitionInput);
}

void PlatformDataInterface::setCognitionOutput()
{
  execute(cognitionOutput);
}

void PlatformDataInterface::callMotion()
{
  // TODO: assert?
  if(motionCallback != NULL)
  {
    getMotionInput();
    motionCallback->call();
    setMotionOutput();
  }
}//callMotion 

void PlatformDataInterface::getMotionInput()
{
  execute(motionInput);
}

void PlatformDataInterface::setMotionOutput()
{
  execute(motionOutput);
}

void PlatformDataInterface::delete_action_list(ActionList& actionList)
{
  for(ActionList::iterator iter = actionList.begin(); iter != actionList.end(); iter++)
  {
    delete (*iter);
  }//end for
}//end execute

void PlatformDataInterface::execute(ActionList& actionList) const
{
  for(ActionList::iterator iter = actionList.begin(); iter != actionList.end(); iter++)
  {
    (*iter)->execute();
  }//end for
}//end execute
