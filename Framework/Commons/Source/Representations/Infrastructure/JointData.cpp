
#include "JointData.h"

#include "PlatformInterface/Platform.h"
#include "Tools/Debug/NaoTHAssert.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

double JointData::min[JointData::numOfJoint];
double JointData::max[JointData::numOfJoint];

JointData::JointData()
{
  // init the arrays
  for (int i = 0; i < numOfJoint; i++) 
  {
    position[i] = 0.0;
    dp[i] = 0.0;
    ddp[i] = 0.0;
    stiffness[i] = 0.0;
  }
}

void JointData::loadJointLimitsFromConfig()
{
  const Configuration& cfg = Platform::getInstance().theConfiguration;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    string jointName = JointData::getJointName((JointData::JointID)i);
    
    if (cfg.hasKey("joints", jointName + "Max")) {
      double v = cfg.getDouble("joints", jointName + "Max");
      max[i] = (i == LHand || i == RHand)?v:Math::fromDegrees(v);
    } else {
      THROW("JointData: can not get " + jointName + " max angle");
    }

    if (cfg.hasKey("joints", jointName + "Min")) {
      double v = cfg.getDouble("joints", jointName + "Min");
      min[i] = (i == LHand || i == RHand)?v:Math::fromDegrees(v);
    } else {
      THROW("JointData: can not get " + jointName + " min angle");
    }
  }//enf for
}//end init

string JointData::getJointName(JointID joint)
{
  switch (joint)
  {
    case HeadPitch: return string("HeadPitch");
    case HeadYaw: return string("HeadYaw");
    case RShoulderRoll: return string("RShoulderRoll");
    case LShoulderRoll: return string("LShoulderRoll");
    case RShoulderPitch: return string("RShoulderPitch");
    case LShoulderPitch: return string("LShoulderPitch");
    case RElbowRoll: return string("RElbowRoll");
    case LElbowRoll: return string("LElbowRoll");
    case RElbowYaw: return string("RElbowYaw");
    case LElbowYaw: return string("LElbowYaw");
    
    case LWristYaw: return string("LWristYaw");
    case RWristYaw: return string("RWristYaw");
    case LHand: return string("LHand");
    case RHand: return string("RHand");
    
    case LHipYawPitch: return string("LHipYawPitch");
    case RHipYawPitch: return string("RHipYawPitch");
    case RHipPitch: return string("RHipPitch");
    case LHipPitch: return string("LHipPitch");
    case RHipRoll: return string("RHipRoll");
    case LHipRoll: return string("LHipRoll");
    case RKneePitch: return string("RKneePitch");
    case LKneePitch: return string("LKneePitch");
    case RAnklePitch: return string("RAnklePitch");
    case LAnklePitch: return string("LAnklePitch");
    case RAnkleRoll: return string("RAnkleRoll");
    case LAnkleRoll: return string("LAnkleRoll");
    default: return string("Unknown Joint");
  }//end switch
}//end getJointName

double JointData::mirrorData(JointID joint) const
{
  switch (joint)
  {
      // Head (don't mirror)
    case HeadYaw: return -position[HeadYaw];
    case HeadPitch: return position[HeadPitch];

      // Arms
    case RShoulderPitch: return position[LShoulderPitch];
    case RShoulderRoll: return -position[LShoulderRoll];
    case RElbowYaw: return -position[LElbowYaw];
    case RElbowRoll: return -position[LElbowRoll];

    case LShoulderPitch: return position[RShoulderPitch];
    case LShoulderRoll: return -position[RShoulderRoll];
    case LElbowYaw: return -position[RElbowYaw];
    case LElbowRoll: return -position[RElbowRoll];

      // Legs
    case RHipYawPitch: return position[LHipYawPitch];
    case RHipPitch: return position[LHipPitch];
    case RHipRoll: return -position[LHipRoll];
    case RKneePitch: return position[LKneePitch];
    case RAnklePitch: return position[LAnklePitch];
    case RAnkleRoll: return -position[LAnkleRoll];


    case LHipYawPitch: return position[RHipYawPitch];
    case LHipPitch: return position[RHipPitch];
    case LHipRoll: return -position[RHipRoll];
    case LKneePitch: return position[RKneePitch];
    case LAnklePitch: return position[RAnklePitch];
    case LAnkleRoll: return -position[RAnkleRoll];
    default: return position[joint];
  }
}//end mirrorData

JointData::JointID JointData::jointIDFromName(std::string name)
{
  for (int i = 0; i < numOfJoint; i++) {
    if (name == getJointName((JointID) i)) return (JointID) i;
  }

  return numOfJoint;
}

void JointData::mirrorFrom(const JointData& jointData)
{
  for (int i = 0; i < numOfJoint; i++) {
    position[i] = jointData.mirrorData((JointID) i);
  }
}

void JointData::mirror()
{
  JointData tmp = *this;
  mirrorFrom(tmp);
}

void JointData::clamp(JointID id)
{
  position[id] = Math::clamp(position[id], min[id], max[id]);
}

void JointData::clamp()
{
  for( int i=0; i<numOfJoint; i++) {
    clamp((JointID)i);
  }
}

bool JointData::isInRange(JointData::JointID id, double ang) const
{
  return ang <= max[id] && ang >= min[id];
}

bool JointData::isInRange(JointData::JointID id) const
{
  return isInRange(id, position[id]);
}

void JointData::updateSpeed(const JointData& lastData, double dt)
{
  const double* lastPose = lastData.position;
  double idt = 1 / dt;
  for ( int i=0; i<JointData::numOfJoint; i++)
  {
    dp[i] = (position[i] - lastPose[i]) * idt;
  }
}

void JointData::updateAcceleration(const JointData& lastData, double dt)
{
  const double* lastSpeed = lastData.dp;
  double idt = 1 / dt;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    ddp[i] = (dp[i] - lastSpeed[i]) * idt;
  }
}

bool JointData::isLegStiffnessOn() const
{
  for ( int i = JointData::RHipYawPitch; i <= JointData::LAnkleRoll; i++)
  {
    if ( stiffness[i] < 0 ) { 
      return false;
    }
  }
  return true;
}

int JointData::checkStiffness() const
{
  for(int i=0; i < JointData::numOfJoint; i++)
  {
    double v = stiffness[i];
    // accepted only values are v \in [0, 1] or v == -1
    if ( (v < 0 && v != -1) || v > 1 ) {
      return i;
      //THROW("Get ILLEGAL Stiffness: "<<getJointName(JointData::JointID(i))<<" = "<<v);
    }
  }
  return -1;
}

SensorJointData::SensorJointData()
  : timestamp(0)
{
  for (int i = 0; i < numOfJoint; i++)
  {
    electricCurrent[i] = 0.0;
    temperature[i] = 0.0;
  }
}


void SensorJointData::print(ostream& stream) const
{
  stream << "Timestamp: " << timestamp << endl;

  // print the joint valies as a table
  stream << "Joint           pos    , pos(deg) , stiffness, temperature, current " << endl;
  stream << "-------------------------------------------------------------------" << endl;
  for (int i = 0; i < numOfJoint; i++) 
  {
    stream << std::left << std::setw(16) << getJointName((JointData::JointID) i);

    stream.precision(4);
    stream << std::right << std::setw(7) << fixed << position[i] << ", ";
    stream << std::right << std::setw(9) << fixed << Math::toDegrees(position[i]) << ", "; 
    stream << std::right << std::setw(9) << fixed << stiffness[i] << ", ";

    stream.precision(0);
    stream << std::right << std::setw(11) << temperature[i] << ", ";

    stream.precision(3);
    stream << std::right << std::setw(7) << electricCurrent[i] <<  endl;
  }
}//end print

SensorJointData::~SensorJointData()
{
}

MotorJointData::MotorJointData()
{
}


MotorJointData::~MotorJointData()
{
}

void MotorJointData::print(ostream& stream) const
{
  // print the joint valies as a table
  stream << "Joint           pos    , pos(deg) , stiffness " << endl;
  stream << "----------------------------------------------" << endl;
  for (int i = 0; i < numOfJoint; i++) 
  {
    stream << std::left << std::setw(16) << getJointName((JointData::JointID) i);

    stream.precision(4);
    stream << std::right << std::setw(7) << fixed << position[i] << ", ";
    stream << std::right << std::setw(9) << fixed << Math::toDegrees(position[i]) << ", "; 
    stream << std::right << std::setw(9) << fixed << stiffness[i] << ", ";
    stream << endl;
  }
}//end print

OffsetJointData::OffsetJointData()
{
}

OffsetJointData::~OffsetJointData()
{
}

void OffsetJointData::print(ostream& stream) const
{
  // print the joint valies as a table
  stream << "Joint           offset " << endl;
  stream << "-----------------------" << endl;
  for (int i = 0; i < numOfJoint; i++) 
  {
    stream << std::left << std::setw(16) << getJointName((JointData::JointID) i);

    stream.precision(4);
    stream << std::right << std::setw(7) << fixed << position[i] << endl;
  }
}//end print

void Serializer<SensorJointData>::serialize(const SensorJointData& representation, std::ostream& stream)
{
  naothmessages::SensorJointData message;
  
  for(int i=0; i < JointData::numOfJoint; i++)
  {
    message.add_electriccurrent(representation.electricCurrent[i]);
    message.add_temperature(representation.temperature[i]);

    message.mutable_jointdata()->add_position(representation.position[i]);
    message.mutable_jointdata()->add_stiffness(representation.stiffness[i]);
    message.mutable_jointdata()->add_dp(representation.dp[i]);
    message.mutable_jointdata()->add_ddp(representation.ddp[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<SensorJointData>::deserialize(std::istream& stream, SensorJointData& representation)
{
  naothmessages::SensorJointData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  // assure the integrity of the message
  ASSERT( message.electriccurrent().size() == JointData::numOfJoint || 
          message.electriccurrent().size() == JointData::numOfJoint - 4);
  ASSERT( message.temperature().size() == JointData::numOfJoint || 
          message.temperature().size() == JointData::numOfJoint - 4);
  ASSERT( message.jointdata().position().size() == JointData::numOfJoint || 
          message.jointdata().position().size() == JointData::numOfJoint - 4);
  ASSERT( message.jointdata().stiffness().size() == JointData::numOfJoint || 
          message.jointdata().stiffness().size() == JointData::numOfJoint - 4);
  ASSERT( message.jointdata().dp().size() == JointData::numOfJoint || 
          message.jointdata().dp().size() == JointData::numOfJoint - 4);
  ASSERT( message.jointdata().ddp().size() == JointData::numOfJoint || 
          message.jointdata().ddp().size() == JointData::numOfJoint - 4);

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    if(i < message.jointdata().position().size())
    {
      representation.electricCurrent[i] = message.electriccurrent(i);
      representation.temperature[i] = message.temperature(i);
      // joint data
      representation.position[i] = message.jointdata().position(i);
      representation.stiffness[i] = message.jointdata().stiffness(i);
      representation.dp[i] = message.jointdata().dp(i);
      representation.ddp[i] = message.jointdata().ddp(i);
    }
    else // LWristYaw, RWristYaw, LHand, RHand don't exist in old messages
    {
      representation.electricCurrent[i] = 0;
      representation.temperature[i] = 0;
      // joint data
      representation.position[i] = 0;
      representation.stiffness[i] = 0;
      representation.dp[i] = 0;
      representation.ddp[i] = 0;
    }
  }
}


void Serializer<MotorJointData>::serialize(const MotorJointData& representation, std::ostream& stream)
{
  naothmessages::JointData message;
  
  for(int i=0; i < JointData::numOfJoint; i++)
  {
    message.add_position(representation.position[i]);
    message.add_stiffness(representation.stiffness[i]);
    message.add_dp(representation.dp[i]);
    message.add_ddp(representation.ddp[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<MotorJointData>::deserialize(std::istream& stream, MotorJointData& representation)
{
  naothmessages::JointData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  // assure the integrity of the message
  ASSERT(message.position().size() == JointData::numOfJoint);
  ASSERT(message.stiffness().size() == JointData::numOfJoint);
  ASSERT(message.dp().size() == JointData::numOfJoint);
  ASSERT(message.ddp().size() == JointData::numOfJoint);

  for(int i=0; i < JointData::numOfJoint; i++)
  {
    representation.position[i] = message.position(i);
    representation.stiffness[i] = message.stiffness(i);
    representation.dp[i] = message.dp(i);
    representation.ddp[i] = message.ddp(i);
  }
}

void Serializer<OffsetJointData>::serialize(const OffsetJointData& representation, std::ostream& stream)
{
  naothmessages::JointData message;

  for(int i=0; i < JointData::numOfJoint; i++)
  {
    message.add_position(representation.position[i]);
    message.add_stiffness(representation.stiffness[i]);
    message.add_dp(representation.dp[i]);
    message.add_ddp(representation.ddp[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<OffsetJointData>::deserialize(std::istream& stream, OffsetJointData& representation)
{
  naothmessages::JointData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  // assure the integrity of the message
  ASSERT(message.position().size() == JointData::numOfJoint);
  ASSERT(message.stiffness().size() == JointData::numOfJoint);
  ASSERT(message.dp().size() == JointData::numOfJoint);
  ASSERT(message.ddp().size() == JointData::numOfJoint);

  for(int i=0; i < JointData::numOfJoint; i++)
  {
    representation.position[i] = message.position(i);
    representation.stiffness[i] = message.stiffness(i);
    representation.dp[i] = message.dp(i);
    representation.ddp[i] = message.ddp(i);
  }
}

