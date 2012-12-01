/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Krause, Thomas</a>
 */

#include "PlatformInterface/PlatformInterface.h"

using namespace naoth;
using namespace std;

PlatformBase::~PlatformBase()
{
}


PlatformDataInterface::PlatformDataInterface()
  :
  motionCallback(NULL),
  cognitionCallback(NULL)
{
}

MessageQueue* PlatformDataInterface::getMessageQueue(const std::string& name)
{
  if (theMessageQueue.count(name) == 0)
  {
    // create a new one
    theMessageQueue[name] = this->createMessageQueue(name);
  }
  
  return theMessageQueue[name];
}

PlatformDataInterface::~PlatformDataInterface()
{
  cout<<"NaoTH clean..."<<endl;
  delete_action_list(motionInput);
  delete_action_list(motionOutput);
  delete_action_list(cognitionInput);
  delete_action_list(cognitionOutput);

  for(std::map<std::string, MessageQueue*>::iterator iter = theMessageQueue.begin(); iter!=theMessageQueue.end(); ++iter)
  {
    delete iter->second;
  }
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
  for(ActionList::iterator iter = actionList.begin(); iter != actionList.end(); ++iter)
  {
    delete (*iter);
  }//end for
  actionList.clear();
}//end execute

void PlatformDataInterface::execute(ActionList& actionList) const
{
  for(ActionList::iterator iter = actionList.begin(); iter != actionList.end(); ++iter)
  {
    (*iter)->execute();
  }//end for
}//end execute
