
#include <iostream>
#include "Lola.h"

void test_eyes(Lola& lola) 
{
  ActuatorData actuators;
  SensorData data;
  
  int pos = 0;
  int step = 0;
  while (step < 83*10) 
  {
    lola.readSensors(data);
    
    if(step % 10 == 0) {
      pos = (pos + 1) % (8*3);
      actuators.LEye[pos] = 1 - actuators.LEye[pos];
    }
    
    lola.writeActuators(actuators);
    
    step++;
  }
}

int main()
{
  Lola lola;
  
  SensorData data;
  lola.readSensors(data);
  
  ActuatorData actuators;
  lola.writeActuators(actuators);

  test_eyes(lola);
}
