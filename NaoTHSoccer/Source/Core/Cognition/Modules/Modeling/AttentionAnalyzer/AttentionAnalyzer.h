/**
* @file AttentionAnalyzer.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class AttentionAnalyzer
*/

#ifndef _AttentionAnalyzer_h_
#define _AttentionAnalyzer_h_


#include <list>


#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/OdometryData.h"

#include "Representations/Perception/BallPercept.h"

#include "Representations/Modeling/AttentionModel.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(AttentionAnalyzer)
  REQUIRE(CameraMatrix)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
//  REQUIRE(BallPercept)

  PROVIDE(AttentionModel)
END_DECLARE_MODULE(AttentionAnalyzer)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class AttentionAnalyzer : public AttentionAnalyzerBase
{
public:

  AttentionAnalyzer();
  ~AttentionAnalyzer(){}

  /** executes the module */
  void execute();

private:
  class PointOfInterest
  {
  public:
    PointOfInterest(): type(0), weight(0.0) {}
    PointOfInterest(Vector2<double> position): position(position), type(0), weight(0.0){}
    Vector2<double> position;
    int type;
    double weight;
  };

  std::list<PointOfInterest> mapOfInterest;
  std::list<PointOfInterest> closeMapOfInterest;

  bool isSeen(const Vector2<double> point);
  void createMapOfInterest();

  void createRadialMapOfInterest(
    std::list<PointOfInterest>& moi,
    double minDistance,
    double maxDistance,
    double distance_step);

  int compare(const PointOfInterest& one, const PointOfInterest& two);

  void drawMapOfInterest(const std::list<PointOfInterest>& moi) const;
  void drawImageProjection();

  OdometryData lastRobotOdometry;
  FrameInfo lastFrameInfo;
};

#endif //_AttentionAnalyzer_h_
