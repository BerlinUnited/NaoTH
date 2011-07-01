/**
 * @file Evolution.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _EVOLUTION_H
#define	_EVOLUTION_H

#include "Cognition/Cognition.h"
#include "GeneticAlgorithms.h"
#include "Representations/Infrastructure/VirtualVision.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Motion/Request/MotionRequest.h"

BEGIN_DECLARE_MODULE(Evolution)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(VirtualVision)
  REQUIRE(RobotPose)
  REQUIRE(CameraMatrix)
  REQUIRE(BallModel)

  PROVIDE(MotionRequest)
END_DECLARE_MODULE(Evolution)

class Evolution : public EvolutionBase
{
public:
  Evolution();

  ~Evolution();

  virtual void execute();

private:
  GeneticAlgorithms* ga;
};

#endif	/* _EVOLUTION_H */

