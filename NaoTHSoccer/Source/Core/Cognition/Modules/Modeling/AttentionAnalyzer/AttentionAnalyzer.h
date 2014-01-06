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
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/OdometryData.h"

#include "Representations/Perception/BallPercept.h"

#include "Representations/Modeling/AttentionModel.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(AttentionAnalyzer)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraInfo)
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
  virtual ~AttentionAnalyzer(){}

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

  typedef std::list<PointOfInterest> ListOfInterest;
  ListOfInterest mapOfInterest;
  ListOfInterest closeMapOfInterest;

  /**
   * Project the point to the image and calculate its distance to the 
   * center of the image
   * @return distance to the center of the image in px
   */
  double distanceToImageCenter(const Vector2<double>& point);
  
  void createMapOfInterest();

  void createRadialMapOfInterest(
    std::list<PointOfInterest>& moi,
    double minDistance,
    double maxDistance,
    double distance_step);

  /** 
   * check wether the point is in view of the robot,
   * i.e., can be seen by just moving the head
   */
  bool inView(const PointOfInterest& point);

  /** 
   * compare two points of interest based on their weight
   */
  int compare(const PointOfInterest& one, const PointOfInterest& two);


  void drawMapOfInterest(const std::list<PointOfInterest>& moi) const;
  void drawImageProjection();

  // --- internal state ---
  // needed for motion update
  OdometryData lastRobotOdometry;
  FrameInfo lastFrameInfo;

  // time when the currentPointOfInterest was changed
  unsigned int currentPointOfInterestTimeStamp;
  // pointing to the elemnt in the mapOfInterest which is currently selected
  // as the most interesting point
  ListOfInterest::iterator currentPointOfInterest;
};

#endif //_AttentionAnalyzer_h_
