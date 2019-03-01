
// https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c

//#define MSGPACK_USE_DEFINE_MAP 
#include <msgpack.hpp>
#include <msgpack/fbuffer.hpp>
#include <msgpack/unpack.hpp>
#include <ext/stdio_filebuf.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include <array>

/*
// example of received sensor data
{
  "RobotConfig":["P0000073A03S83I00011","6.0.0","P0000074A03S84F00006","6.0.0"],
  "Accelerometer":[8.66997,0.287402,-5.48939],
  "Angles":[-0.0239685,0.990951],
  "Battery":[0.99,-32552,0.016,3.6],
  "Current":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
  "FSR":[0.190524,1.52253,0.000270986,0.654938,1.86128,0.130825,0.692107,0.000510382],
  "Gyroscope":[0.00186421,-0.00559264,0],
  "Position":[0.00302601,0.54913,0.544528,0.0674541,-1.45734,-0.0122299,-0.122762,-0.513848,-0.30369,-1.44038,2.13375,-1.21497,0.024586,0.222472,-1.43126,2.1231,-1.20568,0.0322559,0.503194,-0.0276539,1.58305,0.024586,-0.0997519,0.0188,0.036],
  "Sonar":[0.2,0.2],
  "Stiffness":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
  "Temperature":[38,40,38,38,38,38,38,33,27,27,27,27,27,27,27,27,27,27,38,38,38,38,38,38,38],
  "Touch":[0,0,0,0,0,0,0,0,0,0,0,0,0,0],
  "Status":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
}
*/

// https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_adaptor
struct SensorData 
{
  struct {
    struct {
      std::string BodyId;
      std::string Version;
    } Body;
    
    struct {
      std::string FullHeadId;
      std::string Version;
    } Head;
    
    MSGPACK_DEFINE_ARRAY(Body.BodyId, Body.Version, Head.FullHeadId, Head.Version);
  } RobotConfig;
  
  struct {
    float Charge;
    float Current;
    float Status;
    float Temperature;
    
    MSGPACK_DEFINE_ARRAY(Charge, Current, Status, Temperature);
  } Battery;
  
  struct {
    float x;
    float y;
    float z;
    
    MSGPACK_DEFINE_ARRAY(x,y,z);
  } Accelerometer;
  
  struct {
    float x;
    float y;
    float z;
    
    MSGPACK_DEFINE_ARRAY(x,y,z);
  } Gyroscope;
  
  struct {
    float x;
    float y;
  } Angles;
  
  struct FootFSR {
    float FrontLeft;
    float FrontRight;
    float RearLeft;
    float RearRight;
  };
  
  struct {
    FootFSR LFoot;
    FootFSR RFoot;
    MSGPACK_DEFINE_ARRAY(LFoot.FrontLeft, LFoot.FrontRight, LFoot.RearLeft, LFoot.RearRight, RFoot.FrontLeft, RFoot.FrontRight, RFoot.RearLeft, RFoot.RearRight);
  } FSR;
  
  struct {
    float Left;
    float Right;
    MSGPACK_DEFINE_ARRAY(Left, Right);
  } Sonar;
  
  
  struct {
    struct {
      float Button;
    } ChestBoard;
   
    struct {
      struct {
        float Front;
        float Middle;
        float Rear;
      } Touch;
    } Head;
   
    struct BumperType {
      float Left;
      float Right;
    };
    
    struct TouchType {
      float Back;
      float Left;
      float Right;
    };
    
    struct {
      BumperType Bumper;
    } LFoot;
   
    struct {
      TouchType Touch;
    } LHand;

    struct {
      BumperType Bumper;
    } RFoot;

    struct {
      TouchType Touch;
    } RHand;
    
    MSGPACK_DEFINE_ARRAY(
      ChestBoard.Button,
      Head.Touch.Front, Head.Touch.Middle, Head.Touch.Rear,
      LFoot.Bumper.Left, LFoot.Bumper.Right, 
      LHand.Touch.Back, LHand.Touch.Left, LHand.Touch.Right,
      RFoot.Bumper.Left, RFoot.Bumper.Right, 
      RHand.Touch.Back, RHand.Touch.Left, RHand.Touch.Right
    );
  } Touch;

  /*
  struct Joints {

    Joints() : RHipYawPitch(LHipYawPitch) {}

    float HeadPitch;
    float HeadYaw;

    float RShoulderRoll;
    float LShoulderRoll;
    float RShoulderPitch;
    float LShoulderPitch;

    float RElbowRoll;
    float LElbowRoll;
    float RElbowYaw;
    float LElbowYaw;

    float& RHipYawPitch; // doesn't exist on Nao
    float LHipYawPitch;
    float RHipPitch;
    float LHipPitch;
    float RHipRoll;
    float LHipRoll;
    float RKneePitch;
    float LKneePitch;
    float RAnklePitch;
    float LAnklePitch;
    float RAnkleRoll;
    float LAnkleRoll;

    // NOTE: those values don't exist on the old V3.2/V3.3 robots
    //       so; we pu them at the end for easier support for the old format
    float LWristYaw;
    float RWristYaw;
    float LHand;
    float RHand;

    MSGPACK_DEFINE_ARRAY(
      HeadYaw,
      HeadPitch,
      LShoulderPitch,
      LShoulderRoll,
      LElbowYaw,
      LElbowRoll,
      LWristYaw,
      LHipYawPitch,
      LHipRoll,
      LHipPitch,
      LKneePitch,
      LAnklePitch,
      LAnkleRoll,
      RHipRoll,
      RHipPitch,
      RKneePitch,
      RAnklePitch,
      RAnkleRoll,
      RShoulderPitch,
      RShoulderRoll,
      RElbowYaw,
      RElbowRoll,
      RWristYaw,
      LHand,
      RHand
    );
  };
  */

  // motors
  // note we don't unpack the individual values for the joints, 
  // because they are adressed by index
  std::array<float,25> Position;
  std::array<float,25> Stiffness;
  std::array<float,25> Current;
  std::array<float,25> Temperature;
  std::array<int,25> Status;
  
  MSGPACK_DEFINE_MAP(RobotConfig, Accelerometer, Battery, Current, FSR, Gyroscope, Position, Sonar, Stiffness, Temperature, Touch, Status);
};

struct ActuatorData 
{
  std::array<float,25> Position  { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
  std::array<float,25> Stiffness { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
  
  std::vector<float> REar = std::vector<float>(10,0);
  std::vector<float> LEar = std::vector<float>(10,0);
  std::vector<float> Chest = std::vector<float>(3, 0);
  std::vector<float> LEye = std::vector<float>(8*3,0);
  std::vector<float> REye = std::vector<float>(8*3,0);
  std::vector<float> LFoot = std::vector<float>(3,0);
  std::vector<float> RFoot = std::vector<float>(3,0);
  std::vector<float> Skull = std::vector<float>(12,0);
  
  struct {
    bool Left;
    bool Right;
    
    MSGPACK_DEFINE_ARRAY(Left, Right);
  } Sonar;
  
  MSGPACK_DEFINE_MAP(Position, Stiffness, REar, LEar, Chest, LEye, REye, LFoot, RFoot, Skull, Sonar);
};


class Lola 
{
  int fd;
  FILE* fp;
  
  static const int PACKET_ZIZE = 896;
  msgpack::unpacker m_pac;
  
  public:
    Lola()
    {
      if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
      }
      
      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, "/tmp/robocup", sizeof(addr.sun_path)-1);
      
      if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
      }
      
      fp = fdopen(fd, "w");
      m_pac.reserve_buffer(PACKET_ZIZE);
    }  
  
    void writeActuators(const ActuatorData& data) {
      msgpack::fbuffer fbuf(fp);
      msgpack::pack(fbuf, data);
      fflush(fp);
    }
    
    void readSensors(SensorData& data) 
    {
      m_pac.reserve_buffer(PACKET_ZIZE);
      int bytes = read(fd, m_pac.buffer(), m_pac.buffer_capacity());
      
      if(bytes != PACKET_ZIZE) {
        std::cout << "wrong message size: " << bytes << " expected " << PACKET_ZIZE << std::endl;
        exit(-1);
      }
      m_pac.buffer_consumed(bytes);
      
      msgpack::object_handle oh;
      m_pac.next(oh);
      oh.get().convert(data);

      //std::cout << oh.get() << std::endl;
    }
};
