
#include <iostream>
#include <DCM/Tools/Lola.h>

#ifdef NAO
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
#endif

void test_head(GLola& lola) 
{
  ActuatorData actuators;
  SensorData data;

  float step = 0.02f;

  while (true) 
  {
    lola.readSensors(data);

    actuators.Position[0] = data.Position[0] + step;

    if(std::fabs(actuators.Position[0]) > 1.5) {
      step *= -1.0f;
    }

    lola.writeActuators(actuators);
  }
}


int main()
{
  g_type_init();

#ifdef NAO
  Lola lola;
  lola.connectSocket();
#else
  GLola lola;
  lola.connect("localhost", 7776);
#endif

  // basic test read
  SensorData data;
  lola.readSensors(data);
  
  // basic write
  ActuatorData actuators;
  lola.writeActuators(actuators);

#ifdef NAO
  test_eyes(lola);
#else
  test_head(lola);
#endif
}
