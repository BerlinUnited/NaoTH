

#ifndef _LolaAdaptor_h_
#define _LolaAdaptor_h_

#include "Tools/Lola.h"
#include "Tools/IPCData.h"

#include <Tools/ThreadUtil.h>
#include "Tools/SharedMemoryIO.h"
#include "Tools/BasicMotion.h"

#include "Tools/NaoTime.h"

#include <thread>

namespace naoth 
{

class LolaAdaptor
{
  
public:

  LolaAdaptor() 
    : 
    exiting(false),
    state(DISCONNECTED),
    sem(SEM_FAILED)
  {
    // open semaphore
    if((sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
    {
      perror("libnaoth: sem_open");
      ::exit(-1);
    }
    
    const std::string naoSensorDataPath = "/nao_sensor_data";
    std::cout<< "[LolaAdaptor] Opening Shared Memory: "<<naoSensorDataPath<<std::endl;
    naoSensorData.open(naoSensorDataPath);
    
    const std::string naoCommandMotorJointDataPath = "/nao_command.MotorJointData";
    const std::string naoCommandUltraSoundSendDataPath = "/nao_command.UltraSoundSendData";
    const std::string naoCommandIRSendDataPath = "/nao_command.IRSendData";
    const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandMotorJointDataPath << std::endl;
    naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandUltraSoundSendDataPath << std::endl;
    naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandIRSendDataPath << std::endl;
    naoCommandIRSendData.open(naoCommandIRSendDataPath);
    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandLEDDataPath << std::endl;
    naoCommandLEDData.open(naoCommandLEDDataPath);
  }
  
  ~LolaAdaptor()
  {
    // close semaphore
    if(sem != SEM_FAILED)
    {
      sem_close(sem);
      sem = SEM_FAILED;
    }
  }
  
  void start() 
  {
    exiting = false;
    lolaThread = std::thread([this] {this->run();});
    ThreadUtil::setPriority(lolaThread, ThreadUtil::Priority::highest);
    ThreadUtil::setName(lolaThread, "LOLA");
  }
  
  void stop() 
  {
    exiting = true;
    if(lolaThread.joinable()) {
      lolaThread.join();
    }
  }
  
private:

  void run() 
  {
    Lola lola;
  
    SensorData sensors;
    ActuatorData actuators;
    
    //long long start = NaoTime::getSystemTimeInMicroSeconds();

    while(!exiting) 
    {

      //long long stop = NaoTime::getSystemTimeInMicroSeconds();
      //std::cout << (int)(stop - start) << std::endl;
      //start = NaoTime::getSystemTimeInMicroSeconds();

      lola.readSensors(sensors);
      
      NaoSensorData* sensorData = naoSensorData.writing();
      
      // current system time (System time, not nao time (!))
      sensorData->timeStamp = NaoTime::getSystemTimeInMilliSeconds();

      // copy sensor data to shared memory 
      readSensorData(sensors, sensorData->sensorsValue);

      // push the data to shared memory
      naoSensorData.swapWriting();

      // notify cognition
      notify();


      // get the MotorJointData from the shared memory and put them to the DCM
      if ( naoCommandMotorJointData.swapReading() )
      {
        const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
        const MotorJointData& motorData = commandData->get();
        
        // SensorJointData
        for(size_t i = 0; i < lolaJointIdx.size(); ++i) 
        {
          actuators.Position[i] = (float)motorData.position[lolaJointIdx[i]];
        }

        for(size_t i = 0; i < lolaJointIdx.size(); ++i) 
        {
          actuators.Stiffness[i] = (float)motorData.stiffness[lolaJointIdx[i]];
        }

        //drop_count = 0;
        //command_data_available = true;
      }

      if(naoCommandLEDData.swapReading())
      {
        const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
        writeLEDData(commandData->get(), actuators);
      }
      
      lola.writeActuators(actuators);
    }
  }
  
  void notify() 
  {  
    static int drop_count = 10;
    
    // raise the semaphore: triggers core
    if(sem != SEM_FAILED)
    {
      int sval;
      if(sem_getvalue(sem, &sval) == 0)
      {
        if(sval < 1)
        {
          sem_post(sem);

          if(state == DISCONNECTED) {
            fprintf(stderr, "[LolaAdaptor] I think the core is alive.\n");
          }

          drop_count = 0;
          state = CONNECTED;
        }
        else
        {
          if(drop_count == 0) {
            fprintf(stderr, "[LolaAdaptor] dropped sensor data.\n");
          } else if(drop_count == 10) {
            fprintf(stderr, "[LolaAdaptor] I think the core is dead.\n");
            state = DISCONNECTED;
          }

          // don't count more than 11
          drop_count += (drop_count < 11);
        }//end if
      }
      else
      {
        fprintf(stderr, "[LolaAdaptor] I couldn't get value by sem_getvalue.\n");
      }
    }//end if SEM_FAILED
  }
  
  std::array<int,25> lolaJointIdx {{
    JointData::HeadYaw,
    JointData::HeadPitch,

    JointData::LShoulderPitch,
    JointData::LShoulderRoll,
    JointData::LElbowYaw,
    JointData::LElbowRoll,
    JointData::LWristYaw,

    JointData::LHipYawPitch,

    JointData::LHipRoll,
    JointData::LHipPitch,
    JointData::LKneePitch,
    JointData::LAnklePitch,
    JointData::LAnkleRoll,

    JointData::RHipRoll,
    JointData::RHipPitch,
    JointData::RKneePitch,
    JointData::RAnklePitch,
    JointData::RAnkleRoll,

    JointData::RShoulderPitch,
    JointData::RShoulderRoll,
    JointData::RElbowYaw,
    JointData::RElbowRoll,
    JointData::RWristYaw,

    JointData::LHand,
    JointData::RHand
  }};

  
  
  void readSensorData(const SensorData& sensorData, float* sensorsValue) 
  {
    // SensorJointData
    for(size_t i = 0; i < lolaJointIdx.size(); ++i) 
    {
      //NOTE: ignore the JointData::RHipYawPitch
      size_t j = theSensorJointDataIndex + ((lolaJointIdx[i] >= JointData::RHipYawPitch)?lolaJointIdx[i]-1:lolaJointIdx[i])*4;
      sensorsValue[j  ] = sensorData.Current[i];
      sensorsValue[j+1] = sensorData.Temperature[i];
      sensorsValue[j+2] = sensorData.Position[i];
      sensorsValue[j+3] = sensorData.Stiffness[i];
    }

    
    { // FSRData
    unsigned int currentIndex = theFSRDataIndex;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.FrontLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.FrontRight;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.RearLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.LFoot.RearRight;

    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.FrontLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.FrontRight;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.RearLeft;
    sensorsValue[currentIndex++] = sensorData.FSR.RFoot.RearRight;
    }

    { // AccelerometerData
    sensorsValue[theAccelerometerDataIndex + 0] = sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 1] = sensorData.Accelerometer.y;
    sensorsValue[theAccelerometerDataIndex + 2] = sensorData.Accelerometer.z;

    sensorsValue[theAccelerometerDataIndex + 3] = sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 4] = sensorData.Accelerometer.y;
    sensorsValue[theAccelerometerDataIndex + 5] = sensorData.Accelerometer.z;
    }

    { // GyrometerData
    sensorsValue[theGyrometerDataIndex + 0] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 1] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 2] = sensorData.Gyroscope.z;

    sensorsValue[theGyrometerDataIndex + 3] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 4] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 5] = sensorData.Gyroscope.z;

    //sensorsValue[theGyrometerDataIndex + 6] = 0;
    }

    { // InertialSensorData
    sensorsValue[theInertialSensorDataIndex    ] = sensorData.Angles.x;
    sensorsValue[theInertialSensorDataIndex + 1] = sensorData.Angles.y;
    }

    // IRReceiveData
    //for (int i = 0; i < IRReceiveData::numOfIRReceive; i++) {
    //  sensorsValue[theIRReceiveDataIndex + i] = 0;
    //}

    { // ButtonData
    unsigned int currentIndex = theButtonDataIndex;
    sensorsValue[currentIndex++] = sensorData.Touch.ChestBoard.Button;

    sensorsValue[currentIndex++] = sensorData.Touch.LFoot.Bumper.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.LFoot.Bumper.Right;
    sensorsValue[currentIndex++] = sensorData.Touch.RFoot.Bumper.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.RFoot.Bumper.Right;

    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Front;
    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Middle;
    sensorsValue[currentIndex++] = sensorData.Touch.Head.Touch.Rear;

    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Back;
    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.LHand.Touch.Right;

    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Back;
    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Left;
    sensorsValue[currentIndex++] = sensorData.Touch.RHand.Touch.Right;
    }

    { // UltraSoundReceiveData
    //sensorsValue[theUltraSoundReceiveDataIndex    ] = 0;
    sensorsValue[theUltraSoundReceiveDataIndex + 1] = sensorData.Sonar.Left;
    sensorsValue[theUltraSoundReceiveDataIndex + 2] = sensorData.Sonar.Right;
    }

    { // BatteryData
    sensorsValue[theBatteryDataIdex    ] = sensorData.Battery.Charge;
    sensorsValue[theBatteryDataIdex + 1] = sensorData.Battery.Current;
    sensorsValue[theBatteryDataIdex + 2] = sensorData.Battery.Temperature;
    }
  }


  void writeLEDData(const LEDData& ledData, ActuatorData& actuators) 
  {
    // REar
    for(size_t i = 0; i < LEDData::EarLeft0; ++i) {
      actuators.REar[LEDData::EarLeft0-i] = (float)ledData.theMonoLED[i];
    }

    // LEar
    for(size_t i = LEDData::EarLeft0; i < LEDData::numOfMonoLED; ++i) {
      actuators.LEar[i-LEDData::EarLeft0] = (float)ledData.theMonoLED[i];
    }

    // Chest
    actuators.Chest[0] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::RED];
    actuators.Chest[1] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::GREEN];
    actuators.Chest[2] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::BLUE];

    // LEye
    {
    size_t idx = 2;
    for(size_t c = 0; c < LEDData::numOfLEDColor; ++c) {
      for(size_t i = LEDData::FaceLeft0; i < LEDData::FootLeft; ++i) {
        actuators.REar[(8-idx) % 8] = (float)ledData.theMultiLED[i][c];
        idx++;
      }
    }
    }

    // REye
    {
    size_t idx = 0;
    for(size_t c = 0; c < LEDData::numOfLEDColor; ++c) {
      for(size_t i = LEDData::FaceRight0; i < LEDData::FaceLeft0; ++i) {
        actuators.REar[idx++] = (float)ledData.theMultiLED[i][c];
      }
    }
    }
  }

private:
  bool exiting;
  std::thread lolaThread;

  enum State
  {
    DISCONNECTED,
    CONNECTED
  } state;

  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  //SharedMemory<NaoCommandData> naoCommandData;
  SharedMemory<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemory<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemory<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemory<Accessor<LEDData> > naoCommandLEDData;

  // syncronize with NaoController
  sem_t* sem;
};

}

#endif //_LolaAdaptor_h_