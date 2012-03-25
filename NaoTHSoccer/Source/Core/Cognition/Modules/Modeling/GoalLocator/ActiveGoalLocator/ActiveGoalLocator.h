/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Declaration of class ActiveGoalLocatorSimpleParticle
 */

#ifndef __ActiveGoalLocator_h_
#define __ActiveGoalLocator_h_

#include <ModuleFramework/Module.h>
#include "AGLParameters.h"
#include "AGLSampleSet.h"
#include "AGLSampleBuffer.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/CompassDirection.h"

// Tools
#include <vector>
#include "Tools/Math/Geometry.h"
#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/CanopyClustering.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ActiveGoalLocator)
  REQUIRE(GoalPercept)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(CameraMatrix)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(CompassDirection)

  PROVIDE(LocalGoalModel)
END_DECLARE_MODULE(ActiveGoalLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ActiveGoalLocator : private ActiveGoalLocatorBase
{

public:
  ActiveGoalLocator();
  ~ActiveGoalLocator(){}

  virtual void execute();

private:

  double goalWidth;

  double averageWeighting;

  AGLParameters parameters;


  CanopyClustering<AGLSampleBuffer> ccTrashBuffer;
  AGLSampleBuffer theSampleBuffer;


  OdometryData lastRobotOdometry;

  //needs to check the need for using new percept
  // if high, no need for insert new percept
  double timeFilter;

  class Cluster
  {
  public:
      Cluster():
          canopyClustering(theSampleSet){}

      CanopyClustering<AGLSampleSet> canopyClustering;
      AGLSampleSet theSampleSet;
  };

    Cluster ccSamples[10];

  class Sample
  {
  public:
    Sample() : likelihood(0){}
    Vector2<double> position;
    double likelihood;
    //hacked for canopy
    int cluster;
  };

  std::vector<Sample> sampleSet;

  class CanopyCluster
  {
  public:
    ~CanopyCluster(){}
    CanopyCluster():size(0){}

    double size;
    Vector2<double> clusterSum;
    Sample center;

    void add(const Sample& sample);
    double distance(const Sample& sample) const;

  private:
    double manhattanDistance(const Sample& sample) const;
    double euclideanDistance(const Sample& sample) const;
  };

  bool isInCluster(const CanopyCluster& cluster, const Sample& sample) const;

  static const int maxNumberOfClusters = 100;   //FIXME
  CanopyCluster clusters[maxNumberOfClusters]; //FIXME

  int numOfClusters; //TODO: side-effect!!

  void cluster();
  int getClusterSize(const Vector2<double> start);

  void debugDrawings();
  void updateByRobotOdometry();
  void updateByGoalPercept();
  void resampleGT07(bool noise);
};

#endif //__ActiveGoalLocator_h_
