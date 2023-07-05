/**
* @file MonteCarloSelfLocator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MonteCarloSelfLocator
*/

#ifndef _MonteCarloSelfLocator_h_
#define _MonteCarloSelfLocator_h_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Math/Probabilistics.h"

// representations
// basic info
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"

// motion / kinematics
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/CameraMatrix.h"

// situation
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SituationPrior.h"

// sensor percepts
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept2018.h"
#include "Representations/Perception/LinePerceptAugmented.h"

// local models
#include "Representations/Modeling/ProbabilisticQuadCompas.h"
#include "Representations/Perception/LineGraphPercept.h"

// this are the results :)
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"

// debug
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

// basic tools
#include "SampleSet.h"
#include "CanopyClustering.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(MonteCarloSelfLocator)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(GameData)

  REQUIRE(OdometryData)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(MotionStatus)
  REQUIRE(BodyState)
  REQUIRE(SituationPrior)

  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  REQUIRE(SensingGoalModel)
  REQUIRE(ProbabilisticQuadCompas)
  REQUIRE(LineGraphPercept)
  //REQUIRE(RansacCirclePercept)

  REQUIRE(RansacLinePercept)
  REQUIRE(RansacCirclePercept2018)
  REQUIRE(ShortLinePercept)
  REQUIRE(LinePerceptAugmented)

  PROVIDE(RobotPose)
  PROVIDE(SelfLocGoalModel)
END_DECLARE_MODULE(MonteCarloSelfLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class MonteCarloSelfLocator : private MonteCarloSelfLocatorBase
{
public:
  MonteCarloSelfLocator();
  virtual ~MonteCarloSelfLocator();

  void execute();

private: // local types

  enum State {
    BLIND, // no sensory information is processed but the state is monitored
    KIDNAPPED, // robot is lifted up 
    LOCALIZE,
    TRACKING
  } state, lastState;

  // NOTE: indecates whether the robot is localized
  bool islocalized;

  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("MCSLParameters")
    {
      // particle filter params
      PARAMETER_REGISTER(thresholdCanopy) = 900;
      PARAMETER_REGISTER(resamplingThreshhold) = 0.02;

      PARAMETER_REGISTER(processNoiseDistance) = 40;
      PARAMETER_REGISTER(processNoiseAngle) = 0.1;

      PARAMETER_REGISTER(updateByOdometryWhenBlind) = true;
      PARAMETER_REGISTER(updateByOdometryRelative) = true;
      PARAMETER_REGISTER(motionNoise) = true;
      PARAMETER_REGISTER(motionNoiseDistance) = 5.0;
      PARAMETER_REGISTER(motionNoiseAngle) = 0.01;
      PARAMETER_REGISTER(motionNoiseDistanceRelative) = 0.1;
      PARAMETER_REGISTER(motionNoiseAngleRelative) = 0.02;

      PARAMETER_REGISTER(resampleSUS) = false;
      PARAMETER_REGISTER(resampleGT07) = true;

      // goal
      PARAMETER_REGISTER(updateByGoalPostTracking) = false;
      PARAMETER_REGISTER(updateByGoalPostLocalize) = true;
      PARAMETER_REGISTER(goalPostSigmaDistance) = 0.1;
      PARAMETER_REGISTER(goalPostSigmaAngle) = 0.1;
      PARAMETER_REGISTER(maxAcceptedGoalErrorWhileTracking) = 3000;
      PARAMETER_REGISTER(sensorResetByGoalModel) = true;

      // this is legacy
      PARAMETER_REGISTER(updateByLinePoints) = false;
      PARAMETER_REGISTER(linePointsSigmaDistance) = 0.2;
      PARAMETER_REGISTER(linePointsSigmaAngle) = 0.1;
      PARAMETER_REGISTER(linePointsMaxNumber) = 10;

      // lines
      PARAMETER_REGISTER(updateByLinePercept) = true;
      PARAMETER_REGISTER(updateByShortLinePercept) = true;
      PARAMETER_REGISTER(lineSigmaDistance) = 0.1;
      PARAMETER_REGISTER(lineSigmaAngle) = 0.1;
      PARAMETER_REGISTER(lineMaxNumber) = 3;
      PARAMETER_REGISTER(lineMinLength) = 300;
      
      // circle
      PARAMETER_REGISTER(sensorResetByMiddleCircle) = true; // LinePerceptAugmented
      PARAMETER_REGISTER(sensorResetByMiddleCircleAngleDecisionDistance) = 1000;
      PARAMETER_REGISTER(updateByRansacCircle) = true;
      PARAMETER_REGISTER(sigmaDistanceCenterCircle) = 0.1;
      PARAMETER_REGISTER(sigmaAngleCenterCircle) = 0.1;

      PARAMETER_REGISTER(updateByOldPose) = false;
      PARAMETER_REGISTER(oldPoseSigmaDistance) = 500; // mm
      PARAMETER_REGISTER(oldPoseSigmaAngle) = Math::fromDegrees(45.0);

      PARAMETER_REGISTER(updateByCompas) = true;

      PARAMETER_REGISTER(treatLiftUp) = true;
      PARAMETER_REGISTER(treatInitState) = true;
      
      PARAMETER_REGISTER(resetOwnHalf) = false;
      PARAMETER_REGISTER(downWeightFactorOwnHalf) = 0.01;
      
      PARAMETER_REGISTER(updateBySituation) = true;
      PARAMETER_REGISTER(startPositionsSigmaDistance) = 500;
      PARAMETER_REGISTER(startPositionsSigmaAngle) = 0.5;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    // particle filter parameters
    double thresholdCanopy;
    double resamplingThreshhold;

    double processNoiseDistance;
    double processNoiseAngle;

    bool updateByOdometryWhenBlind;
    bool updateByOdometryRelative;
    bool motionNoise;
    double motionNoiseDistance;
    double motionNoiseAngle;
    double motionNoiseDistanceRelative;
    double motionNoiseAngleRelative;

    bool resampleSUS;
    bool resampleGT07;

    // goal
    bool updateByGoalPostTracking;
    bool updateByGoalPostLocalize;
    double goalPostSigmaDistance;
    double goalPostSigmaAngle;
    double maxAcceptedGoalErrorWhileTracking;
    bool sensorResetByGoalModel;

    // line points (legacy)
    bool updateByLinePoints;
    double linePointsSigmaDistance;
    double linePointsSigmaAngle;
    int linePointsMaxNumber;
    
    
    // lines
    bool updateByLinePercept;
    bool updateByShortLinePercept;
    double lineSigmaDistance;
    double lineSigmaAngle;
    int lineMaxNumber;
    int lineMinLength;
    
    // cirlcle
    bool sensorResetByMiddleCircle;
    double sensorResetByMiddleCircleAngleDecisionDistance;
    bool updateByRansacCircle;
    double sigmaDistanceCenterCircle;
    double sigmaAngleCenterCircle;

    bool updateByOldPose;
    double oldPoseSigmaDistance;
    double oldPoseSigmaAngle;

    bool updateByCompas;
    bool treatLiftUp;
    bool treatInitState;
    bool resetOwnHalf;
    double downWeightFactorOwnHalf;

    bool updateBySituation;
    double startPositionsSigmaDistance;
    double startPositionsSigmaAngle;

  } parameters;

  class LineDensity {
  private:
    Math::LineSegment segment;
    double angle; 
    double distDeviation; 
    double angleDeviation;

  public:
    LineDensity()
    {
    }

    LineDensity(
      const Vector2d& start, const Vector2d& end, 
      double angle, double distDeviation, double angleDeviation)
      :
      segment(start, end), angle(angle), distDeviation(distDeviation), angleDeviation(angleDeviation)
    {
    }

    double update(const Sample& sample) const {
      double distDiff = segment.minDistance(sample.translation);
      double angleDiff = Math::normalize(angle - sample.rotation);
      return Math::gaussianProbability(distDiff, distDeviation) * 
             Math::gaussianProbability(angleDiff, angleDeviation);
    }

    void draw(DrawingCanvas2D& canvas)
    {
      canvas.pen("000000", 30);
      canvas.drawLine(segment.begin().x, segment.begin().y, segment.end().x, segment.end().y);
      canvas.drawArrow(
            segment.begin().x, segment.begin().y, 
            segment.begin().x + 100*cos(angle), 
            segment.begin().y + 100*sin(angle));
      canvas.drawArrow(
            segment.end().x, segment.end().y, 
            segment.end().x + 100*cos(angle), 
            segment.end().y + 100*sin(angle));
    }
  };


private: // goal posts
  bool updatedByGoalPosts;

private: // data members
  OdometryData lastRobotOdometry;
  SampleSet theSampleSet;
  SampleSet mhBackendSet; // used ONLY by resampleMH
  //SampleSet setBeforeResampling; // sort of 'double buffering'
  CanopyClustering<SampleSet> canopyClustering;

  // statistics:
  // augmentation trends
  double slowWeighting;
  double fastWeighting;
  //
  double effective_number_of_samples;

private: // workers
  void updateByOdometry(SampleSet& sampleSet, bool noise, bool onlyRotation) const;
  void updateByOdometryAbsolute(SampleSet& sampleSet, bool noise, bool onlyRotation) const;
  void updateByOdometryRelative(SampleSet& sampleSet, bool noise, bool onlyRotation) const;

  bool updateBySensors(SampleSet& sampleSet) const;
  void updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const;
  void updateBySingleGoalPost(const GoalPercept::GoalPost& goalPost, SampleSet& sampleSet) const;
  void updateByCompas(SampleSet& sampleSet) const;
  void updateByLinePoints(const LineGraphPercept& linePercept, SampleSet& sampleSet) const;
  //void updateByLines(const LinePercept& linePercept, SampleSet& sampleSet) const;
  //void updateByShortLines(const LinePercept& linePercept, SampleSet& sampleSet) const;

  void updateByLines2018(const LinePercept2018& linePercept, SampleSet& sampleSet) const;

  void updateByMiddleCircle(const Vector2d& middleCircleCenter, SampleSet& sampleSet) const;
  // A-Priori knowledge based on the game state
  void updateBySidePositions(SampleSet& sampleSet) const;
  void updateByStartPositions(SampleSet& sampleSet) const;
  void updateByOwnHalfLookingForward(SampleSet& sampleSet) const;
  void updateByOwnHalf(SampleSet& sampleSet) const;
  void updateByOppHalf(SampleSet& sampleSet) const;
  void updateByGoalBox(SampleSet& sampleSet) const;

  void updateBySituation();

  void updateByOldPose(SampleSet& sampleSet) const;
  void updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double /*sigmaAngle*/) const;

  void updateStatistics(SampleSet& sampleSet);
  void resetLocator();

  void resampleSimple(SampleSet& sampleSet, int number) const;
  void resampleMH(SampleSet& sampleSet);
  void resampleGT07(SampleSet& sampleSet, bool noise) const;
  int resampleSUS(SampleSet& sampleSet, int n) const;

  int sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const;
  void sensorResetByMiddleCircle(SampleSet& sampleSet) const;

  void calculatePose(SampleSet& sampleSet);

private: //debug
  void draw_sensor_belief() const;
  void drawPosition() const;
};

#endif //_MonteCarloSelfLocator_h_
