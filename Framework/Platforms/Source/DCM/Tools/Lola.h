
// https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c

//#define MSGPACK_USE_DEFINE_MAP 
#include <msgpack.hpp>
#include <msgpack/fbuffer.hpp>
#include <msgpack/unpack.hpp>

#include <sstream>
#include <iostream>
#include <array>


// this is needed for communication with the UNIX socket on NAO
#ifdef NAO
  #include <ext/stdio_filebuf.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <unistd.h>
#endif


// example of received sensor data
/*
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
  
  // NOTE: The order for battery values in Softbank's documentation is wrong.
  struct {
    float Charge;
    float Status;
    float Current;
    float Temperature;
    
    MSGPACK_DEFINE_ARRAY(Charge, Status, Current, Temperature);
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

    MSGPACK_DEFINE_ARRAY(x,y);
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
  std::array<int,25>   Status;
  
  MSGPACK_DEFINE_MAP(RobotConfig, Accelerometer, Angles, Battery, Current, FSR, Gyroscope, Position, Sonar, Stiffness, Temperature, Touch, Status);
};

struct ActuatorData 
{
  std::array<float,25> Position  { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
  std::array<float,25> Stiffness { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
  
  std::vector<float> REar  = std::vector<float>( 10, 0);
  std::vector<float> LEar  = std::vector<float>( 10, 0);
  std::vector<float> Chest = std::vector<float>(  3, 0);
  std::vector<float> LEye  = std::vector<float>(8*3, 0);
  std::vector<float> REye  = std::vector<float>(8*3, 0);
  std::vector<float> LFoot = std::vector<float>(  3, 0);
  std::vector<float> RFoot = std::vector<float>(  3, 0);
  std::vector<float> Skull = std::vector<float>( 12, 0);
  
  struct {
    bool Left;
    bool Right;
    
    MSGPACK_DEFINE_ARRAY(Left, Right);
  } Sonar;
  
  MSGPACK_DEFINE_MAP(Position, Stiffness, REar, LEar, Chest, LEye, REye, LFoot, RFoot, Skull, Sonar);
};


/** 
  NAO client for the UNIX socket on the NAO robot
*/
#ifdef NAO
class Lola 
{
  // POSIX file descriptor for the UNIX Socket on NAO
  int fd;
  // C-style file pointer to fd used to write by msgpack::fbuffer
  FILE* fp;
  
  static const int PACKET_ZIZE = 896;
  msgpack::unpacker m_pac;

  // indicated that the LOLA client is in the error state
  bool error = false;
  
  public:
    Lola() {}  

    void connectSocket() 
    {
      if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        std::err << "[LOLA] socket error" << std::endl;
        //exit(-1);
        error = true;
        return;
      }

      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, "/tmp/robocup", sizeof(addr.sun_path)-1);

      if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::err << "[LOLA] connect error" << std::endl;
        //exit(-1);
        error = true;
        return ;
      }

      //open the file for writing
      fp = fdopen(fd, "w");
    }

    bool hasError() {
      return error;
    }
  
    void writeActuators(const ActuatorData& data) 
    {
      msgpack::fbuffer fbuf(fp);
      msgpack::pack(fbuf, data);
      fflush(fp);
    }
    
    void readSensors(SensorData& data) 
    {
      // make sure we have enough space
      m_pac.reserve_buffer(PACKET_ZIZE);

      // read from the soccet (POSIX style)
      int bytes = read(fd, m_pac.buffer(), m_pac.buffer_capacity());
      
      if(bytes != PACKET_ZIZE) {
        std::cerr << "[LOLA] wrong message size: " << bytes << " expected " << PACKET_ZIZE << std::endl;
      }
      m_pac.buffer_consumed(bytes);
      
      msgpack::object_handle oh;
      m_pac.next(oh);

      // deserialized object is valid during the msgpack::object_handle instance is alive.
      msgpack::object deserialized = oh.get();
      
      // print the decerialized content
      if(bytes != PACKET_ZIZE) {
        std::cout << deserialized << std::endl;
        //assert(false);
      }

      // TODO: is this faster?
      // get() - deserialized object is valid during the msgpack::object_handle instance is alive.
      // convert() - convert msgpack::object instance into the original type.
      //oh.get().convert(data);

      // convert msgpack::object instance into the original type.
      deserialized.convert(data);

      // DEBUG:
      //std::cout << oh.get() << std::endl;
    }
};
#endif




#include <glib.h>
#include <gio/gio.h>


// experimental Lola client based on glib
class GLola 
{
  GSocketClient * client;
  GSocketConnection * connection;

  static const int PACKET_ZIZE = 896;
  msgpack::unpacker m_pac;

public:

  void connect(const std::string& host, int port) 
  {
    GError * error = NULL;

    /* create a new connection */
    connection = NULL;
    client = g_socket_client_new();

    /* connect to the host */
    connection = g_socket_client_connect_to_host (client, (gchar*)host.c_str(), port, NULL, &error);
    //fd = g_socket_get_fd( g_socket_connection_get_socket (connection) );
  }

  void writeActuators(const ActuatorData& data) 
  {
    std::stringstream sbuf;
    msgpack::pack(sbuf, data);

    std::string s = sbuf.str();

    GError * error = NULL;
    GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));
    g_output_stream_write (ostream, s.c_str(), (int)s.size(), NULL, &error);
  }

  void readSensors(SensorData& data) 
  {
    m_pac.reserve_buffer(PACKET_ZIZE);

    GError * error = NULL;
    GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
    int bytes = g_input_stream_read (istream, m_pac.buffer(), m_pac.buffer_capacity(), NULL, &error);

    if(bytes != PACKET_ZIZE) {
      std::cout << "[LOLA] wrong message size: " << bytes << " expected " << PACKET_ZIZE << std::endl;
    }
    m_pac.buffer_consumed(bytes);

    msgpack::object_handle oh;
    m_pac.next(oh);

    // deserialized object is valid during the msgpack::object_handle instance is alive.
    msgpack::object deserialized = oh.get();

    // print the decerialized content
    if(bytes != PACKET_ZIZE) {
      std::cout << deserialized << std::endl;
      //assert(false);
    }

    // convert msgpack::object instance into the original type.
    deserialized.convert(data);

    // DEBUG:
    //std::cout << oh.get() << std::endl;
  }
};