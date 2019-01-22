/**
* @file LineSymbols.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class LineSymbols
*/

#ifndef _LineSymbols_H_
#define _LineSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Perception/LinePercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/OdometryData.h"

#include "Tools/DataStructures/RingBuffer.h"

#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(LineSymbols)
  REQUIRE(LinePercept)
  REQUIRE(FrameInfo)
  REQUIRE(OdometryData)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
END_DECLARE_MODULE(LineSymbols)

class LineSymbols: public LineSymbolsBase
{

public:
  LineSymbols()
  {
    theInstance = this;
  }

  virtual ~LineSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static LineSymbols* theInstance;

  static double getLineTimeSinceLastSeen();
  static double getEstDistanceToOrthPoint();
  static double getAngleToEstOrthPoint();
  static double getDistanceToClosestPoint();
  static double getAngleToClosestPoint();

  // small filter for the line points
  RingBuffer<Vector2d, 10> linePointsBuffer;
  Vector2d linePointsBufferMean;
  Pose2D lastRobotOdometry;

};//end class LineSymbols

#endif // _LineSymbols_H_
