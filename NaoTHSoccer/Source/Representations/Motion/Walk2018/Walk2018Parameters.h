/**
* @file Walk2018Parameters.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Kaden, Steffen</a>
*
*/

#ifndef _WALK_2018_Parameters_H_
#define _WALK_2018_Parameters_H_

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugParameterList.h"

class FeetStabilizerParameters: public ParameterList{
      public:
        FeetStabilizerParameters() : ParameterList("Walk_FeetStabilizer")
        {
          PARAMETER_REGISTER(stabilizeFeet) = true;

          PARAMETER_REGISTER(P.x) = -0.1;
          PARAMETER_REGISTER(P.y) = -0.1;
          PARAMETER_REGISTER(D.x) = 0.01;
          PARAMETER_REGISTER(D.y) = 0.01;

          PARAMETER_REGISTER(gyroFilterAlpha) = 0.2;

          syncWithConfig();
        }

        Vector2d P;
        Vector2d D;

        bool stabilizeFeet;
        double gyroFilterAlpha;
};

class FootStepPlanner2018Parameters: public ParameterList{
    public:
      FootStepPlanner2018Parameters() : ParameterList("Walk_FootStepPlanner2018")
      {
          PARAMETER_ANGLE_REGISTER(limits.maxTurnInner) = 10;
          PARAMETER_ANGLE_REGISTER(limits.maxStepTurn)  = 30;
          PARAMETER_REGISTER(limits.maxStepLength)      = 50;
          PARAMETER_REGISTER(limits.maxStepLengthBack)  = 35;
          PARAMETER_REGISTER(limits.maxStepWidth)       = 60;
          PARAMETER_REGISTER(limits.maxStepChange)      = 0.3;

          PARAMETER_ANGLE_REGISTER(limits.maxCtrlTurn) = 30;
          PARAMETER_REGISTER(limits.maxCtrlLength) = 80;
          PARAMETER_REGISTER(limits.maxCtrlWidth)  = 50;

          PARAMETER_REGISTER(footOffsetY) = 0;

          PARAMETER_REGISTER(step.doubleSupportTime) = 0;
          PARAMETER_REGISTER(step.duration) = 260;
          PARAMETER_REGISTER(step.dynamicDuration) = true;

          PARAMETER_REGISTER(stabilization.dynamicStepSize)  = true;
          PARAMETER_REGISTER(stabilization.dynamicStepSizeP) = -0.1;
          PARAMETER_REGISTER(stabilization.dynamicStepSizeD) = 0.05;
          PARAMETER_REGISTER(stabilization.emergencyStopError)  = 250;
          PARAMETER_REGISTER(stabilization.maxEmergencyCounter) = 500;

          syncWithConfig();
      }

      struct Limits {
        double maxTurnInner;
        double maxStepTurn;
        double maxStepLength;
        double maxStepLengthBack;
        double maxStepWidth;
        double maxStepChange;

        double maxCtrlTurn;
        double maxCtrlLength;
        double maxCtrlWidth;
      } limits;

      struct Step {
        int  doubleSupportTime;
        int  duration;
        bool dynamicDuration;
      } step;

      struct Stabilization {
        bool   dynamicStepSize;
        double dynamicStepSizeP;
        double dynamicStepSizeD;

        double emergencyStopError;
        double maxEmergencyCounter;
      } stabilization;

      double footOffsetY;
};

class LiftingFootCompensatorParameters: public ParameterList{
      public:
        LiftingFootCompensatorParameters() : ParameterList("Walk_LiftingFootCompensator")
        {
          PARAMETER_REGISTER(comHeightOffset) = 0.18;
          PARAMETER_REGISTER(comStepOffsetY)  = 0;

          syncWithConfig();
        }

        double comHeightOffset;
        double comStepOffsetY;
};

class FootTrajectoryGenerator2018Parameters: public ParameterList{
  public:
      FootTrajectoryGenerator2018Parameters() : ParameterList("Walk_FootTrajectoryGenerator2018")
      {
        PARAMETER_REGISTER(stepHeight) = 21;
        PARAMETER_REGISTER(kickHeight) = 35;
        PARAMETER_REGISTER(useSplineFootTrajectory)  = true;

        syncWithConfig();
      }

      double kickHeight;
      double stepHeight;
      bool   useSplineFootTrajectory;
};

class HipRotationOffsetModifierParameters: public ParameterList{
    public:
        HipRotationOffsetModifierParameters() : ParameterList("Walk_HipRotationOffsetModifier")
        {
          PARAMETER_REGISTER(hipOffsetBasedOnStepChange.x) = 0;
          PARAMETER_REGISTER(hipOffsetBasedOnStepChange.y) = 0;
          PARAMETER_ANGLE_REGISTER(bodyPitchOffset) = 0.1;

          syncWithConfig();
        }

        Vector2d hipOffsetBasedOnStepChange;
        double   bodyPitchOffset;
};

class TorsoRotationStabilizerParameters: public ParameterList{
  public:
    TorsoRotationStabilizerParameters() : ParameterList("Walk_TorsoRotationStabilizer")
    {
      PARAMETER_REGISTER(rotationStabilize) = true;

      PARAMETER_REGISTER(rotation.P.x) = -0.03;
      PARAMETER_REGISTER(rotation.P.y) = -0.03;
      PARAMETER_REGISTER(rotation.VelocityP.x) = 0.03;
      PARAMETER_REGISTER(rotation.VelocityP.y) = 0.03;
      PARAMETER_REGISTER(rotation.D.x) = 0;
      PARAMETER_REGISTER(rotation.D.y) = 0;

      PARAMETER_REGISTER(localRotationCalibration) = true;
      PARAMETER_REGISTER(useSteffensInertial) = true;
      PARAMETER_REGISTER(gyroFilterAlpha) = 0.5;
      syncWithConfig();
    }

    struct RotationStabilization{
        Vector2d P;
        Vector2d VelocityP;
        Vector2d D;
    } rotation;

    bool useSteffensInertial;
    bool rotationStabilize;
    bool localRotationCalibration;
    double gyroFilterAlpha;
};

class ZMPPlanner2018Parameters: public ParameterList{
public:
  ZMPPlanner2018Parameters() : ParameterList("Walk_ZMPPlanner2018")
  {
      PARAMETER_REGISTER(bezierZMP.inFootScalingY) = 1;
      PARAMETER_REGISTER(bezierZMP.inFootSpacing)  = 10;
      PARAMETER_REGISTER(bezierZMP.transitionScaling) = 0.6;
      PARAMETER_REGISTER(bezierZMP.offsetX) = 20;
      PARAMETER_REGISTER(bezierZMP.offsetY) = -10;
      PARAMETER_REGISTER(bezierZMP.offsetXForKicks) = 0;
      PARAMETER_REGISTER(bezierZMP.offsetYForKicks) = -20;

      PARAMETER_REGISTER(simpleZMP.offsetX) = 20;
      PARAMETER_REGISTER(simpleZMP.offsetY) = -7;
      PARAMETER_REGISTER(simpleZMP.kickOffsetY)  = -7;

      PARAMETER_REGISTER(comHeight) = 260;
      PARAMETER_REGISTER(ZMPOffsetYByCharacter) = 0;
      PARAMETER_REGISTER(newZMP_ON) = true;
      PARAMETER_REGISTER(useZMPHackForKicks) = false;

      PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLength.x) = 5;
      PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLength.y) = 0;
      PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLengthForKicks.x) = 0;
      PARAMETER_REGISTER(stabilization.maxHipOffsetBasedOnStepLengthForKicks.y) = 0;

      syncWithConfig();
  }

  struct Bezier {
    double inFootScalingY;
    double inFootSpacing;
    double transitionScaling;

    double offsetX;
    double offsetY;
    double offsetXForKicks;
    double offsetYForKicks;
  } bezierZMP;

  struct Simple {
    double offsetX;
    double offsetY;
    double kickOffsetY;
  } simpleZMP;

  struct Stabilization {
    Vector2d maxHipOffsetBasedOnStepLength;
    Vector2d maxHipOffsetBasedOnStepLengthForKicks;
  } stabilization;

  bool newZMP_ON;
  bool useZMPHackForKicks;

  double comHeight;
  double ZMPOffsetYByCharacter;
};

class Walk2018Parameters : public naoth::Printable
{
public:
    Walk2018Parameters(){
    }

    FeetStabilizerParameters      feetStabilizerParams;
    FootStepPlanner2018Parameters footStepPlanner2018Params;
    FootTrajectoryGenerator2018Parameters footTrajectoryGenerator2018Params;
    HipRotationOffsetModifierParameters hipRotationOffsetModifierParams;
    LiftingFootCompensatorParameters liftingFootCompensatorParams;
    TorsoRotationStabilizerParameters torsoRotationStabilizerParams;
    ZMPPlanner2018Parameters zmpPlanner2018Params;

    void init(DebugParameterList& dbpl){
        dbpl.add(&feetStabilizerParams);
        dbpl.add(&footStepPlanner2018Params);
        dbpl.add(&footTrajectoryGenerator2018Params);
        dbpl.add(&hipRotationOffsetModifierParams);
        dbpl.add(&liftingFootCompensatorParams);
        dbpl.add(&torsoRotationStabilizerParams);
        dbpl.add(&zmpPlanner2018Params);
    }

    void remove(DebugParameterList& dbpl){
        dbpl.remove(&feetStabilizerParams);
        dbpl.remove(&footStepPlanner2018Params);
        dbpl.remove(&footTrajectoryGenerator2018Params);
        dbpl.remove(&hipRotationOffsetModifierParams);
        dbpl.remove(&liftingFootCompensatorParams);
        dbpl.remove(&torsoRotationStabilizerParams);
        dbpl.remove(&zmpPlanner2018Params);
    }

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<CoMErrors>
//  {
//  public:
//    static void serialize(const CoMErrors& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, CoMErrors& representation);
//  };
//}

#endif // __WALK_2018_Parameters_H_
