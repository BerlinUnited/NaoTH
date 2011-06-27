/**
* @file ParticleFilterBallLocator.h
*
* @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
* Declaration of class ParticleFilterBallLocator
*/

#ifndef __ParticleFilterBallLocator_h_
#define __ParticleFilterBallLocator_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/OdometryData.h"

#include "Representations/Modeling/BallModel.h"

#include "Tools/Math/Vector3.h"
#include <vector>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ParticleFilterBallLocator)
  REQUIRE(BallPercept)
  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)

  PROVIDE(BallModel)
END_DECLARE_MODULE(ParticleFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ParticleFilterBallLocator : private ParticleFilterBallLocatorBase
{

public:
  ParticleFilterBallLocator();
  virtual ~ParticleFilterBallLocator(){};

  virtual void execute();


private:

  class Sample
  {
  public:
    Sample() : likelihood(0), moving(false){}
    Vector2<double> position;
    Vector2<double> speed;
    double likelihood;
    bool moving;
  };

  typedef std::vector<Sample> SampleSet;

  SampleSet theSampleSet;
  Pose2D lastRobotOdometry;

  void updateByBallPercept(SampleSet& sampleSet);
  void updateByOdometry(SampleSet& sampleSet, bool noise) const;

  void resampleGT07(SampleSet& sampleSet, bool noise);


  void drawBallModel(const BallModel& ballModel) const;
  void drawSamples(const SampleSet& sampleSet) const;
};

#endif //__ParticleFilterBallLocator_h_


