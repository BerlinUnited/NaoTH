

#ifndef _LolaAdaptor_h_
#define _LolaAdaptor_h_

// data representations
#include "Tools/LolaData.h"
#include "Tools/LolaDataConverter.h"
#include "Tools/DCMData.h"

// lola unix socket client
#include "Tools/Lola.h"

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
    command_data_available(false),
    sensor_data_available(false),
    initialMotion(NULL),
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
    const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandMotorJointDataPath << std::endl;
    naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
    std::cout << "[LolaAdaptor] Opening Shared Memory: " << naoCommandUltraSoundSendDataPath << std::endl;
    naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
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

    // int tryCount = 0;
    while(waitingForLola && ! exiting)
    {
      fprintf(stderr, "[LolaAdaptor] Waiting for LoLA socket.\n");
      std::this_thread::sleep_for(std::chrono::milliseconds(125));
      // if(tryCount > 40 )
      // {
      //   fprintf(stderr, "[LolaAdaptor] Waiting for LoLA socket failed after %d ms.\n", tryCount * 125);
      //   waitingForLola = false;
      //   assert(false);
      // }
      // tryCount++;
    }
    fprintf(stderr, "[LolaAdaptor] LoLA socket connection established.\n");
    //HACK: pulseaudio is not initialized correctly, but after playing a sound as no it works?!?
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
    while(!fileExists("/tmp/robocup")) 
    {
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
    
    while(!exiting) 
    {
      lola.readSensors(sensors);
      
      // like old DCM motionCallbackPre
      if(!runEmergencyMotion(actuators))
      {
        // get the MotorJointData from the shared memory and put them to the DCM
        if ( naoCommandMotorJointData.swapReading() )
        {
          const Accessor<MotorJointData>* motorData = naoCommandMotorJointData.reading();
          
          // write MotorJointData to LolaActuatorData
          LolaDataConverter::set(actuators, motorData->get());

          //drop_count = 0;
          command_data_available = true;
        }

        if(naoCommandLEDData.swapReading())
        {
          const Accessor<LEDData>* commandData = naoCommandLEDData.reading();
          LolaDataConverter::set(actuators, commandData->get());
        }
      }
      lola.writeActuators(actuators);
      
      
      // like old DCM motionCallbackPost
      DCMSensorData* sensorData = naoSensorData.writing();
      
      // current system time (System time, not nao time (!))
      sensorData->timeStamp = NaoTime::getSystemTimeInMilliSeconds();

      // copy sensor data to shared memory 
      LolaDataConverter::readSensorData(sensors, *sensorData);

      // check if chest button was pressed as a request to shutdown
      // each cycle needs 12ms so if the button was pressed for 3 seconds
      // these are 250 frames
      sensorData->get(theButtonData);
      if(!shutdown_requested && theButtonData.numOfFramesPressed[ButtonData::Chest] > 250)
      {
        shutdown_requested = true;
        //exit(-1);
        // break;
        shutdownCallbackThread = std::thread([this]{this->shutdownCallback();});
      }

      // save the data for the emergency case
      if(state == DISCONNECTED) {
        std::cout << "get inertial sensor data" << std::endl;
        sensorData->get(theInertialSensorData);
        sensor_data_available = true;
      } else {
        sensor_data_available = false;
      }

      // push the data to shared memory
      naoSensorData.swapWriting();

      // notify cognition
      notify();
    }
    
    running = false;
  }
  
  void setWarningLED(ActuatorData& actuators, bool red)
  {
    static naoth::LEDData theLEDData;
    static int count = 0;
    
    int begin = ((++count)/10)%10;
    theLEDData.theMonoLED[LEDData::EarRight0 + begin] = 0;
    theLEDData.theMonoLED[LEDData::EarLeft0 + begin] = 0;
    int end = (begin+2)%10;
    theLEDData.theMonoLED[LEDData::EarRight0 + end] = 1;
    theLEDData.theMonoLED[LEDData::EarLeft0 + end] = 1;

    for(int i=0; i<LEDData::numOfMultiLED; i++)
    {
      theLEDData.theMultiLED[i][LEDData::RED] = red ? 1 : 0;
      theLEDData.theMultiLED[i][LEDData::GREEN] = 0;
      theLEDData.theMultiLED[i][LEDData::BLUE] = red ? 0 : 1;
    }
    LolaDataConverter::set(actuators, theLEDData);
  }//end setWarningLED

  bool runEmergencyMotion(ActuatorData& actuators)
  {
    if(state == DISCONNECTED)
    {
      if(initialMotion == NULL && command_data_available && sensor_data_available)
      {
         std::cout << "emerg: start init motion" << std::endl;
         // take the last command data
        const Accessor<MotorJointData>* commandData = naoCommandMotorJointData.reading();
        initialMotion = new BasicMotion(theMotorJointData, commandData->get(), theInertialSensorData);
      }

      setWarningLED(actuators, state == DISCONNECTED);
    }//end if

    // after reconnect: wait until the init motion is finished
    if(initialMotion != NULL)
    {
      if(state == CONNECTED && initialMotion->isFinish())
      {
        std::cout << "emerg: stop init motion" << std::endl;
        delete initialMotion;
        initialMotion = NULL;
      }
      else
      {
        // execute the emergency motion 
        // (the resulting joint commands are written to theMotorJointData)
        initialMotion->execute();
        
        // write MotorJointData to LolaActuatorData
        LolaDataConverter::set(actuators, theMotorJointData);
      }
    }//end if
   
    return initialMotion != NULL;
  }//end runEmergencyMotion

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
    system("paplay /opt/aldebaran/share/naoqi/wav/bip_power_off.wav");

    // stop the user program
    std::cout << "[LolaAdaptor] stopping naoth" << std::endl;
    // stop naoth to trigger emergency motion
    system("killall naoth");
    // in NaoSMAL it's like this
    //system("naoth stop");

    sleep(5);

    // we are the child process, do a blocking call to shutdown
    std::cout << "[LolaAdaptor] System shutdown requested" << std::endl;
    system("/sbin/shutdown -h now");

    // await termination
    while(true) {
      sleep(100);
    }

  }//end shutdownCallback
  

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

  // sitdown motion in case the Controller dies
  bool command_data_available;
  bool sensor_data_available;
  MotorJointData theMotorJointData;
  InertialSensorData theInertialSensorData;
  BasicMotion* initialMotion;

  // DCM --> NaoController
  SharedMemory<DCMSensorData> naoSensorData;

  // NaoController --> DCM
  //SharedMemory<NaoCommandData> naoCommandData;
  SharedMemory<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemory<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemory<Accessor<LEDData> > naoCommandLEDData;

  // syncronize with NaoController
  sem_t* sem;
};

}

#endif //_LolaAdaptor_h_