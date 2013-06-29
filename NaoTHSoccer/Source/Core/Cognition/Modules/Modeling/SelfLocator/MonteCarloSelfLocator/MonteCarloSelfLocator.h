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
#include "Tools/LinesTable.h"
#include "MCSLParameters.h"

// Representations
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SituationStatus.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include <Representations/Perception/FieldSidePercept.h>

//
#include "SampleSet.h"
#include "CanopyClustering.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(MonteCarloSelfLocator)
  REQUIRE(FieldSidePercept)
  
  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)
  REQUIRE(LinePercept)
  REQUIRE(LinePerceptTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(SensingGoalModel)
  REQUIRE(LocalGoalModel)
  REQUIRE(CompassDirection)
 
  REQUIRE(PlayerInfo)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
  REQUIRE(BodyState)
  
  REQUIRE(SituationStatus)

  PROVIDE(RobotPose)
  PROVIDE(SelfLocGoalModel)
END_DECLARE_MODULE(MonteCarloSelfLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////



class MonteCarloSelfLocator : private MonteCarloSelfLocatorBase
{
public:
  MonteCarloSelfLocator();
  virtual ~MonteCarloSelfLocator(){}

  /** executes the module */
  void execute();

private:
  MCSLParameters parameters;
  OdometryData lastRobotOdometry;
  SampleSet theSampleSet;

  CanopyClustering<SampleSet> canopyClustering;

  bool initialized; // particles did cluster

  // sample set tools
  void resetSampleSet(SampleSet& sampleSet);
  void clampSampleSetToField(SampleSet& sampleSet);
  void mirrorSampleSetFieldSides(SampleSet& sampleSet);
  
  bool generateTemplateFromPosition(
    Sample& newTemplate,
    const Vector2<double>& posSeen, 
    const Vector2<double>& posReal) const;


  // TODO: this is maybe not the best solution for this
  Vector2<double> fieldMin;
  Vector2<double> fieldMax;

  /** */
  bool isInsideCarpet(const Vector2d& p) const;

  /** */
  void createRandomSample(Sample& sample) const;

  /** */
  int sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const;
  int sensorResetByGoalPosts(SampleSet& sampleSet, int n) const;

  /** */
  void resample(SampleSet& sampleSet);
  void resampleGT07(SampleSet& sampleSet, bool noise);


  // some helper (copied from GT07) 
  // not used now
  double getDistanceDeviation(const Pose2D& fromPose, const Vector2<double>& toPoint, double measuredDistance) const;
  double getBearingDeviation(const Pose2D& fromPose, const Vector2<double>& toPoint, double measuredBearing) const;
  double gaussian(double d) const {return exp(- d * d);}

  /** */
  double computeAngleWeighting(
                                double measuredAngle, 
                                double expectedAngle,
                                double standardDeviation, 
                                double bestPossibleWeighting = 1.0) const;

  /** */
  double computeDistanceWeighting(
                                double measuredDistance, 
                                double expectedDistance,
                                double cameraZ,
                                double standardDeviation, 
                                double bestPossibleWeighting = 1.0) const;


  /****************************************
    update methods
   ****************************************/
  void updateByOdometry(SampleSet& sampleSet, bool noise) const;
  void updateByGoalModel(SampleSet& sampleSet) const;

  void updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const;
  void updateByLinesTable(const LinePercept& linePercept, SampleSet& sampleSet) const;
  void updateByCornersTable(const LinePercept& linePercept, SampleSet& sampleSet) const;
  void updateByMiddleCircle(const LinePercept& linePercept, SampleSet& sampleSet) const;
  
  // for simulation only
  void updateByFlags(SampleSet& sampleSet) const;

  void updateFallDown(SampleSet& sampleSet) const;

  void updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double sigmaAngle) const;
  void updateByOldPose(SampleSet& sampleSet) const;

  /** apply all the updates ba any avaliable sensor data */
  bool updateBySensors(SampleSet& sampleSet) const;

  /** should be always a logical mirror of updateBySensor */
  bool hasSensorUpdate() const;

  void updateByOwnHalf(SampleSet& sampleSet) const;


  /****************************************
    drawing methods
   ****************************************/
  void drawPosition() const;
  void drawSamplesImportance(SampleSet& sampleSet) const;
  void drawSelfLocGoalModel() const;
  void draw_sensor_belief() const;

  /****************************************/
  SampleSet setBeforeResampling;
};

#endif //_MonteCarloSelfLocator_h_
