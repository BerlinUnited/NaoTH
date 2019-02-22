
// https://github.com/troydhanson/network/blob/master/unixdomain/01.basic/cli.c

#include <iostream>

//#define MSGPACK_USE_DEFINE_MAP 
#include <msgpack.hpp>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sstream>



/*
{
  "RobotConfig":["P0000073A03S83I00011","6.0.0","P0000074A03S84F00006","6.0.0"],
  "Accelerometer":[8.66997,0.287402,-5.48939],"Angles":[-0.0239685,0.990951],
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
struct MyData 
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
  
  // motors
  // note we don't unpack the individual values for the joints, 
  // because they are adressed by 
  std::vector<float> Position;
  std::vector<float> Stiffness;
  std::vector<float> Current;
  std::vector<float> Temperature;
  std::vector<float> Status;
  
  MSGPACK_DEFINE_MAP(RobotConfig, Accelerometer, Battery, Current, FSR, Gyroscope, Position, Sonar, Stiffness, Temperature, Touch, Status);
};


int main()
{

  int fd;
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
  
  
  char buf[896];
  int bytes = read(fd, buf, sizeof(buf));
  std::cout << "test: " << bytes << std::endl;
  
  msgpack::object_handle oh;
  msgpack::unpack(oh, buf, 896);

  //std::tuple<std::map<std::string,std::vector<std::string>>, std::map<std::string, std::vector<float>>> t;
  MyData data;
  oh.get().convert(data);
  

  /*
  std::stringstream ss;
  msgpack::pack(ss, data);
  
  msgpack::object_handle oh2 =
      msgpack::unpack(ss.str().data(), ss.str().size());
  msgpack::object obj = oh2.get();
  */
  std::cout << data.RobotConfig.Head.Version << std::endl;
  
  /*
  for(auto& a: s) {
    std::cout << a.first << std::endl;
  }
  */
  
  //
  //close(s);
}
