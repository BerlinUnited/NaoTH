/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

#include <PlatformInterface/Platform.h>

/////////////////////////////////////
// Modules
/////////////////////////////////////
//Infrastructure
#include "Modules/Infrastructure/IO/Sensor.h"
#include "Modules/Infrastructure/IO/Actuator.h"
#include "Modules/Infrastructure/LEDSetter/LEDSetter.h"

using namespace std;

Cognition::Cognition()
{
  registerModule<LEDSetter>("LEDSetter");
}

Cognition::~Cognition()
{
}

void Cognition::init(naoth::PlatformDataInterface& platformInterface)
{
  g_debug("test configuration: %s", 
    Platform::getInstance().theConfiguration.getString("test","test").c_str());

  g_message("Cognition register start");
  
  // input
  ModuleCreator<Sensor>* sensor = registerModule<Sensor>("Sensor");
  sensor->setEnabled(true);
  sensor->getModuleT()->init(platformInterface);
  
  // output
  ModuleCreator<Actuator>* actuator = registerModule<Actuator>("Actuator");
  actuator->setEnabled(true);
  actuator->getModuleT()->init(platformInterface);

  g_message("Cognition register end");
}//end init


void Cognition::call()
{
  cout << "Cognition run" << endl;
}//end call


