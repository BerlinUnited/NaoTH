/**
* @file LineSymbols2012.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class LineSymbols2012
*/

#ifndef __LineSymbols2012_H_
#define __LineSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Perception/LinePercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/OdometryData.h"

#include "Tools/DataStructures/RingBuffer.h"

BEGIN_DECLARE_MODULE(LineSymbols2012)
  REQUIRE(LinePercept)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
END_DECLARE_MODULE(LineSymbols2012)

class LineSymbols2012: public LineSymbols2012Base
{

public:
  LineSymbols2012()
  :
  linePercept(getLinePercept()),
  frameInfo(getFrameInfo())
  {
    theInstance = this;
  };
  virtual ~LineSymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static LineSymbols2012* theInstance;

  static double getLineTimeSinceLastSeen();
  static double getEstDistanceToOrthPoint();
  static double getAngleToEstOrthPoint();
  static double getDistanceToClosestPoint();
  static double getAngleToClosestPoint();

  // representations
  const LinePercept& linePercept;
  const naoth::FrameInfo& frameInfo;


  // small filter for the line points
  RingBuffer<Vector2d, 10> linePointsBuffer;
  Vector2d linePointsBufferMean;
  Pose2D lastRobotOdometry;

};//end class LineSymbols2012

#endif // __LineSymbols2012_H_
