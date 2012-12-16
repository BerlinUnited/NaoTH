/**
 * @file MotionEngine.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MotionEngine_h_
#define _MotionEngine_h_

#include "../MotionBlackBoard.h"

#include "../AbstractMotion.h"
#include "MotionFactory.h"
#include "HeadMotion/HeadMotionEngine.h"

#include <list>

class MotionEngine
{
public:
  MotionEngine();
  virtual ~MotionEngine();

  void execute();

protected:
  
  void selectMotion();
  
  void changeMotion(AbstractMotion* m);

private:
  MotionBlackBoard& theBlackBoard;

  AbstractMotion* currentlyExecutedMotion;
  EmptyMotion theEmptyMotion;
  
  HeadMotionEngine theHeadMotionEngine;
  std::list<MotionFactory*> theMotionFactories;

  enum State
  {
    initial,
    running,
    exiting
  } state;
};

#endif  // _MotionEngine_h_
