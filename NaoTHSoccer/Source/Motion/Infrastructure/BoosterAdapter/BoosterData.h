/**
* @file BoosterData.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
*
* Data for communication with Booster K1. Data is serialized using MessagePack.
*
* More information regarding MessagePack for C++ can be found here:
* https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_adaptor
*
*/

#ifndef BOOSTER_DATA_H_
#define BOOSTER_DATA_H_

#include <msgpack.hpp>
#include <msgpack/fbuffer.hpp>
#include <msgpack/unpack.hpp>

#include <array>

/*
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
*/

// TODO: work in progress
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


/*
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
  } Sonar = {true, true}; // enable by default
  
  MSGPACK_DEFINE_MAP(Position, Stiffness, REar, LEar, Chest, LEye, REye, LFoot, RFoot, Skull, Sonar);
};
*/

namespace Booster {

struct ActuatorData 
{
  struct {
    float yaw;
    float pitch;

    MSGPACK_DEFINE_MAP(yaw, pitch);
  } headPose;

  struct {
    float x;
    float y;
    float rotation;

    MSGPACK_DEFINE_MAP(x, y, rotation);
  } walkVelocity;
  
  MSGPACK_DEFINE_MAP(headPose, walkVelocity);
};

};

#endif // BOOSTER_DATA_H_
