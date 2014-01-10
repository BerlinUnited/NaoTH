/**
* @file LineSymbols.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class LineSymbols
*/

#ifndef __LineSymbols2011_H_
#define __LineSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Perception/LinePercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/OdometryData.h"

#include "Tools/DataStructures/RingBuffer.h"

BEGIN_DECLARE_MODULE(LineSymbols2011)
  REQUIRE(LinePercept)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
END_DECLARE_MODULE(LineSymbols2011)

class LineSymbols2011: public LineSymbols2011Base
{

public:
  LineSymbols2011()
  :
  linePercept(getLinePercept()),
  frameInfo(getFrameInfo())
  {
    theInstance = this;
  };
  ~LineSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static LineSymbols2011* theInstance;

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

};//end class LineSymbols

#endif // __LineSymbols2011_H_
