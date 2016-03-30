/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class SituationPriorProvider
*/

#ifndef _SituationPriorProvider_H
#define _SituationPriorProvider_H

#include <ModuleFramework/Module.h>
#include "Tools/Math/Geometry.h"

#include "Tools/Math/Probabilistics.h"

// basic info
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"

// situation
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SituationStatus.h"

#include "Representations/Modeling/SituationPrior.h"

// debug
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(SituationPriorProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(GameData)

  REQUIRE(MotionStatus)
  REQUIRE(BodyState)
  REQUIRE(SituationStatus)

  PROVIDE(SituationPrior)
END_DECLARE_MODULE(SituationPriorProvider)

class SituationPriorProvider: public SituationPriorProviderBase
{
public:
 SituationPriorProvider();
 ~SituationPriorProvider();

 virtual void execute();

private:

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

    /*double update(const Sample& sample) const {
      double distDiff = segment.minDistance(sample.translation);
      double angleDiff = Math::normalize(angle - sample.rotation);
      return Math::gaussianProbability(distDiff, distDeviation) * 
             Math::gaussianProbability(angleDiff, angleDeviation);
    }*/

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

  
  void reset();
  bool isLiftedUp();

  GameData::GameState lastState;
  GameData::GameState currentState;

  bool startedWalking;
  bool wasLiftedUp;
  bool body_lift_up;
};

#endif  /* _SituationPriorProvider_H */