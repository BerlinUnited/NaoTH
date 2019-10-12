/* 
 * File:   JointData.h
 * Author: Oliver Welter
 *
 * Created on 2. Januar 2009, 16:15
 */

#ifndef _JointData_H_
#define _JointData_H_

#include <string>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
#include "Tools/Math/Common.h"

namespace naoth
{

class JointData
{
  public:
    enum JointID
    {
      HeadPitch,
      HeadYaw,

      RShoulderRoll,
      LShoulderRoll,
      RShoulderPitch,
      LShoulderPitch,

      RElbowRoll,
      LElbowRoll,
      RElbowYaw,
      LElbowYaw,

      RHipYawPitch, // doesn't exist on Nao
      LHipYawPitch,
      RHipPitch,
      LHipPitch,
      RHipRoll,
      LHipRoll,
      RKneePitch,
      LKneePitch,
      RAnklePitch,
      LAnklePitch,
      RAnkleRoll,
      LAnkleRoll,

      // NOTE: those values don't exist on the old V3.2/V3.3 robots
      //       so, we put them at the end for easier support for the old format
      LWristYaw,
      RWristYaw,
      LHand,
      RHand,

      numOfJoint //error value
    };

    // 'safe' cast operator for int
    // it maps any int value outside the range to numOfJoint
    static inline JointData::JointID getJointID (int n)
    {
      return (n >= 0 && n <= numOfJoint)?(JointID)n:numOfJoint;
    }

    double position[numOfJoint];
    double stiffness[numOfJoint];
    double dp[numOfJoint]; // speed
    double ddp[numOfJoint]; // acceleration

    // HACK: joint limits
    static double min[numOfJoint];
    static double max[numOfJoint];

    JointData();

    virtual ~JointData(){}

    //
    static void loadJointLimitsFromConfig();

    static std::string getJointName(JointID);
    static JointID jointIDFromName(std::string name);

    void mirror();

    /* clamp joint data in min-max range */
    void clamp(JointID id);
    void clamp();

    bool isInRange(JointID id, double ang) const;
    bool isInRange(JointID id) const;
    
    void updateSpeed(const JointData& lastData, double dt);

    void updateAcceleration(const JointData& lastData, double dt);

    bool isLegStiffnessOn() const;
    
    int checkStiffness() const;

    void set(JointID joint, const double value, const double max_delta) {
      position[joint] += Math::clamp(value - position[joint], -max_delta, max_delta);
    }

  protected:
    double mirrorData(JointID joint) const;
    void mirrorFrom(const JointData& jointData);
  };

  class SensorJointData : public JointData, public Printable
  {
  public:
    SensorJointData();
    ~SensorJointData();
    double temperature[numOfJoint];
    double electricCurrent[numOfJoint];
    unsigned int timestamp;

    virtual void print(std::ostream& stream) const;
  };

  class MotorJointData : public JointData, public Printable
  {
  public:
    MotorJointData();
    ~MotorJointData();

    virtual void print(std::ostream& stream) const;
  };

  class OffsetJointData : public JointData, public Printable
  {
  public:
    OffsetJointData();
    ~OffsetJointData();

    virtual void print(std::ostream& stream) const;
  };
  
  template<>
  class Serializer<SensorJointData>
  {
    public:
    static void serialize(const SensorJointData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, SensorJointData& representation);
  };

  template<>
  class Serializer<MotorJointData>
  {
    public:
    static void serialize(const MotorJointData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, MotorJointData& representation);
  };

  template<>
  class Serializer<OffsetJointData>
  {
    public:
    static void serialize(const OffsetJointData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, OffsetJointData& representation);
  };
}

#endif  /* _JointData_H_ */

