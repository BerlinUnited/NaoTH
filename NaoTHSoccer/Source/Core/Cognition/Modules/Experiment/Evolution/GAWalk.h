/**
 * @file GAWalk.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * use GA to optimize the walk parameters
 */

#ifndef _GAWALK_H
#define  _GAWALK_H

#include "GeneticAlgorithms.h"
#include "Representations/Infrastructure/VirtualVision.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Motion/Request/MotionRequest.h"

class GAWalk : public GeneticAlgorithms
{
  class Test
  {
  public:
    Test():started(false),theMaxTime(0){}

    Test(unsigned int maxTime, const Pose2D& walkReq);

    Pose2D update(unsigned int time, const Vector2<double>& pos);

    bool isFinished() const;

    double getDistance() const;

  private:
    bool started;
    int theMaxTime;
    Vector2<double> theStartPos;
    Vector2<double> theStopPos;
    Pose2D theWalkRequest;
  };

public:
  GAWalk(const VirtualVision& vv, const RobotPose& rp, const CameraMatrix& cm, const FrameInfo& fi, const FieldInfo& field,
    MotionRequest& mq);

  virtual ~GAWalk(){}

  virtual void run();

private:
  const VirtualVision& theVirtualVision;
  const RobotPose& theRobotPose;
  const CameraMatrix& theCameraMatrix;
  const FrameInfo& theFrameInfo;
  const FieldInfo& theFieldInfo;
  MotionRequest& theMotionRequest;

  std::map<std::string, Vector2<double> > genes;
  unsigned int lastResetTime;
  unsigned int lastTime;
  unsigned int fallenCount;

  std::list<Test> theTests;
  std::list<Test>::iterator theTest;

  void updateFitness(double fitness);
  void reset();
};

#endif  /* _GAWALK_H */

