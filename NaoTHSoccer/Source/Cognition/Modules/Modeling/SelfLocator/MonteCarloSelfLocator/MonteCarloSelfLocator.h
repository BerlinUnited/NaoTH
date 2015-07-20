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
#include "Representations/Modeling/SituationStatus.h"

// sensor percepts
#include "Representations/Perception/GoalPercept.h"

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
  REQUIRE(SituationStatus)

  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  REQUIRE(SensingGoalModel)
  REQUIRE(ProbabilisticQuadCompas)
  REQUIRE(LineGraphPercept)

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
      PARAMETER_REGISTER(thresholdCanopy) = 900;
      PARAMETER_REGISTER(resamplingThreshhold) = 0.01;

      PARAMETER_REGISTER(processNoiseDistance) = 70;
      PARAMETER_REGISTER(processNoiseAngle) = 0.1;

      PARAMETER_REGISTER(motionNoise) = false;
      PARAMETER_REGISTER(motionNoiseDistance) = 5.0;
      PARAMETER_REGISTER(motionNoiseAngle) = 0.01;

      PARAMETER_REGISTER(updateByGoalPost) = true;
      PARAMETER_REGISTER(goalPostSigmaDistance) = 0.1;
      PARAMETER_REGISTER(goalPostSigmaAngle) = 0.1;

      PARAMETER_REGISTER(updateByLinePoints) = true;
      PARAMETER_REGISTER(linePointsSigmaDistance) = 0.1;
      PARAMETER_REGISTER(linePointsSigmaAngle) = 0.1;
      PARAMETER_REGISTER(linePointsMaxNumber) = 10;

      PARAMETER_REGISTER(updateByOldPose) = false;
      PARAMETER_REGISTER(oldPoseSigmaDistance) = 0.1;

      PARAMETER_REGISTER(updateByCompas) = true;

      PARAMETER_REGISTER(treatLiftUp) = true;
      PARAMETER_REGISTER(treatInitState) = true;
      
      PARAMETER_REGISTER(resetOwnHalf) = false;
      PARAMETER_REGISTER(downWeightFactorOwnHalf) = 0.01;
      PARAMETER_REGISTER(maxTimeForLiftUp) = 500;
      
      PARAMETER_REGISTER(updateBySituation) = true;
      PARAMETER_REGISTER(startPositionsSigmaDistance) = 500;
      PARAMETER_REGISTER(startPositionsSigmaAngle) = 0.5;

      PARAMETER_REGISTER(resampleSUS) = false;
      PARAMETER_REGISTER(resampleGT07) = true;

      PARAMETER_REGISTER(maxAcceptedGoalErrorWhileTracking) = 0;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    double thresholdCanopy;
    double resamplingThreshhold;

    double processNoiseDistance;
    double processNoiseAngle;

    bool motionNoise;
    double motionNoiseDistance;
    double motionNoiseAngle;

    bool updateByGoalPost;
    double goalPostSigmaDistance;
    double goalPostSigmaAngle;

    bool updateByLinePoints;
    double linePointsSigmaDistance;
    double linePointsSigmaAngle;
    int linePointsMaxNumber;

    bool updateByOldPose;
    double oldPoseSigmaDistance;

    bool updateByCompas;
    bool treatLiftUp;
    bool treatInitState;
    bool resetOwnHalf;
    double downWeightFactorOwnHalf;
    double maxTimeForLiftUp;

    bool updateBySituation;
    double startPositionsSigmaDistance;
    double startPositionsSigmaAngle;

    bool resampleSUS;
    bool resampleGT07;

    double maxAcceptedGoalErrorWhileTracking;
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
      canvas.pen("000000", 10);
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
  void updateByOdometry(SampleSet& sampleSet, bool noise) const;

  bool updateBySensors(SampleSet& sampleSet) const;
  void updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const;
  void updateBySingleGoalPost(const GoalPercept::GoalPost& goalPost, SampleSet& sampleSet) const;
  void updateByCompas(SampleSet& sampleSet) const;
  void updateByLinePoints(const LineGraphPercept& linePercept, SampleSet& sampleSet) const;

  // A-Priori knowledge based on the game state
  void updateByStartPositions(SampleSet& sampleSet) const;
  void updateByOwnHalfLookingForward(SampleSet& sampleSet) const;
  void updateByOwnHalf(SampleSet& sampleSet) const;
  void updateByOppHalf(SampleSet& sampleSet) const;
  void updateByGoalBox(SampleSet& sampleSet) const;

  void updateByOldPose(SampleSet& sampleSet) const;
  void updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double /*sigmaAngle*/) const;

  void updateStatistics(SampleSet& sampleSet);
  void resetLocator();

  void resampleSimple(SampleSet& sampleSet, int number) const;
  void resampleMH(SampleSet& sampleSet);
  void resampleGT07(SampleSet& sampleSet, bool noise) const;
  int resampleSUS(SampleSet& sampleSet, int n) const;

  int sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const;

  void calculatePose(SampleSet& sampleSet);

private: //debug
  void draw_sensor_belief() const;
  void drawPosition() const;
};

#endif //_MonteCarloSelfLocator_h_
