#ifndef Agent_h
#define Agent_h

#include <PlatformInterface/PlatformInterface.h>
#include "Cognition/Cognition.h"
#include "Motion/Motion.h"

// this is the entry point (!)
void naoth::init_agent(naoth::PlatformInterface& platform)
{
  // create and register processes
  platform.registerCognition(new Cognition());
  platform.registerMotion(new Motion());
}// end init_agent


Motion* createMotion()
{
  return new Motion();
}

Cognition* createCognition()
{
  return new Cognition();
}

ModuleManager* getModuleManager(Cognition* c)
{
  return dynamic_cast<ModuleManager*>(c);
}

ModuleManager* getModuleManager(Motion* m)
{
  return dynamic_cast<ModuleManager*>(m);
}

void deleteCognition(Cognition* cognition)
{
  delete cognition;
}

void deleteMotion(Motion* motion)
{
  delete motion;
}


#endif // Agent_h
