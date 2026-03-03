#ifndef Agent_h
#define Agent_h

#include <PlatformInterface/PlatformInterface.h>
#include "Cognition/Cognition.h"
#include "Motion/Motion.h"
#include "Motion/Motion2026.h"

// this is the entry point (!)
void naoth::init_agent(naoth::PlatformInterface& platform)
{
  // create and register processes
  platform.registerCognition(new Cognition());
  platform.registerMotion(new Motion());
}


Motion* createMotion()
{
  return new Motion();
}

Motion2026* createMotion2026()
{
  return new Motion2026();
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

ModuleManager* getModuleManager(Motion2026* m)
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

void deleteMotion2026(Motion2026* motion)
{
  delete motion;
}


#endif // Agent_h
