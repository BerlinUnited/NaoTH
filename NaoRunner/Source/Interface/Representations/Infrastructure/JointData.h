/* 
 * File:   JointData.h
 * Author: Oliver Welter
 *
 * Created on 2. Januar 2009, 16:15
 */

#ifndef _JOINTDATA_H
#define	_JOINTDATA_H

#include <string>

#include "Interface/Tools/DataStructures/Printable.h"
#include "Interface/PlatformInterface/PlatformInterchangeable.h"

using namespace std;

namespace naorunner
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
      RHipYawPitch,
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
      numOfJoint,
    };
    double position[numOfJoint];
    double hardness[numOfJoint];
    double dp[numOfJoint];
    double ddp[numOfJoint];

    static double min[numOfJoint];
    static double max[numOfJoint];

    JointData();

    virtual ~JointData(){}

    void init(const std::string& filename);

    static string getJointName(JointID);
    static JointID jointIDFromName(std::string name);

    void mirror();

    /* clamp joint data in min-max range */
    void clamp(JointID id);
    void clamp();

    bool isInRange(JointID id, double ang) const;
    bool isInRange(JointID id) const;

  protected:
    double mirrorData(JointID joint) const;
    void mirrorFrom(const JointData& jointData);
  };

  class SensorJointData : public JointData, public Printable, public PlatformInterchangeable
  {
  public:
    SensorJointData();
    ~SensorJointData();
    double temperature[numOfJoint];
    double electricCurrent[numOfJoint];

    virtual void print(ostream& stream) const;

  };

  class MotorJointData : public JointData, public Printable, public PlatformInterchangeable
  {
  public:
    MotorJointData();
    ~MotorJointData();

    virtual void print(ostream& stream) const;
  };
}

#endif	/* _JOINTDATA_H */

