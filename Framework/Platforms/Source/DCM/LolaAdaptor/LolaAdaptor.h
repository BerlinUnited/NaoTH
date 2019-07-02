

#ifndef _LolaAdaptor_h_
#define _LolaAdaptor_h_

#include "Tools/Lola.h"
#include "Tools/IPCData.h"

#include <Tools/ThreadUtil.h>
#include "Tools/SharedMemoryIO.h"
#include "Tools/BasicMotion.h"

#include "Tools/NaoTime.h"

#include <chrono>
#include <thread>
#include <unistd.h>

namespace naoth 
{

class LolaAdaptor
{
  
public:

  LolaAdaptor() 
    : 
    running(false),
    exiting(false),
    waitingForLola(true),
    shutdown_requested(false),
    state(DISCONNECTED),
    sem(SEM_FAILED)
  {
    // open semaphore
    if((sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
    {
      perror("[LolaAdaptor] sem_open");
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

    // save the body ID
    std::string theBodyID = "ALDT_lola"; //theDCMHandler.getBodyID();
    std::cout << "[LolaAdaptor] bodyID: "<< theBodyID << std::endl;
    
    // save the nick name
    std::string theBodyNickName = "nao_lola"; //theDCMHandler.getBodyNickName();
    std::cout << "[LolaAdaptor] nickName: "<< theBodyNickName << std::endl;

    // save the value to file
    // FIXME: fixed path "Config/nao.info"
    {
      std::string staticMemberPath("Config/nao.info");
      std::ofstream os(staticMemberPath.c_str());
      ASSERT(os.good());
      os << theBodyID << "\n" << theBodyNickName << std::endl;
      os.close();
    }

  }
  
  ~LolaAdaptor()
  {
    // stop the thread before anything else
    stop();
    
    // close semaphore
    if(sem != SEM_FAILED)
    {
      sem_close(sem);
      sem = SEM_FAILED;
    }
  }
  
  static bool fileExists (const std::string& filename) {
    struct stat buffer;   
    return (stat (filename.c_str(), &buffer) == 0); 
  }

  void start() 
  {
    exiting = false;
    lolaThread = std::thread([this] {this->run();});
    //ThreadUtil::setPriority(lolaThread, ThreadUtil::Priority::highest);
    ThreadUtil::setName(lolaThread, "LOLA");

    // NOTE: SCHED_FIFO is a real time sheduler
    //       max priority for a FIFO thread is 99
    sched_param param;
    param.sched_priority = 50;
    if(pthread_setschedparam(lolaThread.native_handle(), SCHED_FIFO, &param)) {
      std::cerr << "[LolaAdaptor] error setting thread priority" << std::endl;
      assert(false);
    }

    int tryCount = 0;
    while(waitingForLola)
    {
      fprintf(stderr, "[LolaAdaptor] Waiting for LoLA socket.\n");
      std::this_thread::sleep_for(std::chrono::milliseconds(125));
      if(tryCount > 40 )
      {
        fprintf(stderr, "[LolaAdaptor] Waiting for LoLA socket failed after %d ms.\n", tryCount * 125);
        waitingForLola = false;
        assert(false);
      }
      tryCount++;
    }
    fprintf(stderr, "[LolaAdaptor] LoLA socket connection established.\n");
  }
  
  void stop() 
  {
    std::cout << "[LolaAdaptor] stop wait" << std::endl;
    
    // request the thread to stop
    exiting = true;
    
    if(lolaThread.joinable()) {
      lolaThread.join();
    }
    std::cout << "[LolaAdaptor] stop done" << std::endl;
  }
  
  bool isRunning() const {
    return running;
  }
  
private:

  void run() 
  {
    int tryCount = 0;

    // end the thread if lola could not connect to the socket
    while(!fileExists("/tmp/robocup")) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      if(tryCount > 20 )
      {
        fprintf(stderr, "[LolaAdaptor] Waiting for LoLa %d ms.\n", tryCount * 250);
        assert(false);
      }
      tryCount++;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    Lola lola;
    lola.connectSocket();
    if(lola.hasError())
    {
      fprintf(stderr, "[LolaAdaptor] Error while attemting to connect to LoLa socket.\n");
      assert(false);
    }

    waitingForLola = false;
    running = true;
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

      // check if chest button was pressed as a request to shutdown
      // each cycle needs 10ms so if the button was pressed for 30 seconds
      // these are 300 frames
      sensorData->get(theButtonData);
      if(!shutdown_requested && theButtonData.numOfFramesPressed[ButtonData::Chest] > 300)
      {
        shutdown_requested = true;
        //exit(-1);
        // break;
        shutdownCallbackThread = std::thread([this]{this->shutdownCallback();});
      }

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
    
    running = false;
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


  void shutdownCallback()
  {
    // play a sound that the user knows we recognized his shutdown request
    system("/usr/bin/paplay /usr/share/naoqi/wav/bip_gentle.wav");

    // // stop the user program
    // std::cout << "[LolaAdaptor] stopping naoth" << std::endl;
    // system("naoth stop");

    // sleep(5);

    // we are the child process, do a blocking call to shutdown
    system("/sbin/shutdown -h now");
    std::cout << "[LolaAdaptor] System shutdown requested" << std::endl;

    // await termination
    while(true) {
      sleep(100);
    }

  }//end shutdownCallback
  

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
    sensorsValue[theAccelerometerDataIndex + 1] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 2] = sensorData.Accelerometer.z;

    sensorsValue[theAccelerometerDataIndex + 3] = sensorData.Accelerometer.x;
    sensorsValue[theAccelerometerDataIndex + 4] = -sensorData.Accelerometer.y; // y-axis of v6 robots is mirrored compared to v5 and earlier
    sensorsValue[theAccelerometerDataIndex + 5] = sensorData.Accelerometer.z;
    }

    { // GyrometerData
    sensorsValue[theGyrometerDataIndex + 0] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 1] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 2] = -sensorData.Gyroscope.z;// z-axis of v6 robots is mirrored compared to v5 and earlier

    sensorsValue[theGyrometerDataIndex + 3] = sensorData.Gyroscope.x;
    sensorsValue[theGyrometerDataIndex + 4] = sensorData.Gyroscope.y;
    sensorsValue[theGyrometerDataIndex + 5] = -sensorData.Gyroscope.z;// z-axis of v6 robots is mirrored compared to v5 and earlier

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
    for(size_t i = 0; i <= LEDData::EarRight324; ++i) {
      actuators.REar[i] = (float)ledData.theMonoLED[LEDData::EarRight324 - i];
    }

    // LEar
    for(size_t i = 0; i <= LEDData::EarRight324; ++i) {
      actuators.LEar[i] = (float)ledData.theMonoLED[LEDData::EarLeft0 + i];
    }

    // Chest
    actuators.Chest[0] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::RED];
    actuators.Chest[1] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::GREEN];
    actuators.Chest[2] = (float)ledData.theMultiLED[LEDData::ChestButton][LEDData::BLUE];

    // LEye
    {
    actuators.LEye[0] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::RED]);
    actuators.LEye[1] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::RED]);
    actuators.LEye[2] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::RED]);
    actuators.LEye[3] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::RED]);
    actuators.LEye[4] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::RED]);
    actuators.LEye[5] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::RED]);
    actuators.LEye[6] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::RED]);
    actuators.LEye[7] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::RED]);

    actuators.LEye[8] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::GREEN]);
    actuators.LEye[9] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::GREEN]);
    actuators.LEye[10] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::GREEN]);
    actuators.LEye[11] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::GREEN]);
    actuators.LEye[12] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::GREEN]);
    actuators.LEye[13] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::GREEN]);
    actuators.LEye[14] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::GREEN]);
    actuators.LEye[15] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::GREEN]);

    actuators.LEye[16] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::BLUE]);
    actuators.LEye[17] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::BLUE]);
    actuators.LEye[18] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::BLUE]);
    actuators.LEye[19] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::BLUE]);
    actuators.LEye[20] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::BLUE]);
    actuators.LEye[21] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::BLUE]);
    actuators.LEye[22] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft135][LEDData::BLUE]);
    actuators.LEye[23] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::BLUE]);
    }

    // REye
    {
    actuators.LEye[0] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::RED]);
    actuators.LEye[1] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::RED]);
    actuators.LEye[2] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::RED]);
    actuators.LEye[3] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::RED]);
    actuators.LEye[4] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::RED]);
    actuators.LEye[5] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::RED]);
    actuators.LEye[6] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::RED]);
    actuators.LEye[7] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::RED]);

    actuators.LEye[8] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::GREEN]);
    actuators.LEye[9] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::GREEN]);
    actuators.LEye[10] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::GREEN]);
    actuators.LEye[11] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::GREEN]);
    actuators.LEye[12] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::GREEN]);
    actuators.LEye[13] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::GREEN]);
    actuators.LEye[14] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::GREEN]);
    actuators.LEye[15] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::GREEN]);

    actuators.LEye[16] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft0][LEDData::BLUE]);
    actuators.LEye[17] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft45][LEDData::BLUE]);
    actuators.LEye[18] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft90][LEDData::BLUE]);
    actuators.LEye[19] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceRight135][LEDData::BLUE]);
    actuators.LEye[20] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft180][LEDData::BLUE]);
    actuators.LEye[21] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft225][LEDData::BLUE]);
    actuators.LEye[22] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft270][LEDData::BLUE]);
    actuators.LEye[23] = static_cast<float>(ledData.theMultiLED[LEDData::MultiLEDID::FaceLeft315][LEDData::BLUE]);
    }

    // Skull
    actuators.Skull[ 0] = (float)ledData.theMonoLED[LEDData::HeadFrontLeft1];
    actuators.Skull[ 1] = (float)ledData.theMonoLED[LEDData::HeadFrontLeft0];
    actuators.Skull[ 2] = (float)ledData.theMonoLED[LEDData::HeadMiddleLeft0];
    actuators.Skull[ 3] = (float)ledData.theMonoLED[LEDData::HeadRearLeft0];
    actuators.Skull[ 4] = (float)ledData.theMonoLED[LEDData::HeadRearLeft1];
    actuators.Skull[ 5] = (float)ledData.theMonoLED[LEDData::HeadRearLeft2];
    actuators.Skull[ 6] = (float)ledData.theMonoLED[LEDData::HeadRearRight2];
    actuators.Skull[ 7] = (float)ledData.theMonoLED[LEDData::HeadRearRight1];
    actuators.Skull[ 8] = (float)ledData.theMonoLED[LEDData::HeadRearRight0];
    actuators.Skull[ 9] = (float)ledData.theMonoLED[LEDData::HeadMiddleRight0];
    actuators.Skull[10] = (float)ledData.theMonoLED[LEDData::HeadFrontRight0];
    actuators.Skull[11] = (float)ledData.theMonoLED[LEDData::HeadFrontRight1];

    // LFoot
    actuators.LFoot[0] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::RED];
    actuators.LFoot[1] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::GREEN];
    actuators.LFoot[2] = (float)ledData.theMultiLED[LEDData::FootLeft][LEDData::BLUE];

    // RFoot
    actuators.LFoot[0] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::RED];
    actuators.LFoot[1] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::GREEN];
    actuators.LFoot[2] = (float)ledData.theMultiLED[LEDData::FootRight][LEDData::BLUE];
  }

private:
  bool running;
  bool exiting;
  bool waitingForLola;
  std::thread lolaThread;

  // these things are necessary to handle shutdown
  ButtonData theButtonData;
  bool shutdown_requested;
  std::thread shutdownCallbackThread;

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