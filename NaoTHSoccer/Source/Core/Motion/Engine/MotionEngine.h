/**
 * @file MotionEngine.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MotionEngine_h_
#define _MotionEngine_h_

//#include <ModuleFramework/Module.h>

#include "../MotionBlackBoard.h"

#include "../AbstractMotion.h"
#include "MotionFactory.h"
#include "HeadMotion/HeadMotionEngine.h"

#include <list>

#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>

// representations


BEGIN_DECLARE_MODULE(MotionEngine)
  REQUIRE(FrameInfo)
  REQUIRE(MotionLock)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(MotionStatus)
END_DECLARE_MODULE(MotionEngine)


class MotionEngine: private ModuleManager, private MotionEngineBase
{
public:
  MotionEngine();
  virtual ~MotionEngine();

  void execute();

protected:
  
  void selectMotion();
  
  void changeMotion(Module* m);

private:
  //MotionBlackBoard& theBlackBoard;

  Module* currentlyExecutedMotion;
  ModuleCreator<EmptyMotion>* theEmptyMotion;
  
  // submodules
  ModuleCreator<HeadMotionEngine>* theHeadMotionEngine;
  
  typedef std::list<MotionFactory*> MotionFactorieRegistry;
  MotionFactorieRegistry theMotionFactories;


  Module* createEmptyMotion()
  {
    theEmptyMotion->setEnabled(false);
    theEmptyMotion->setEnabled(true);
    return theEmptyMotion->getModule();
  }

  enum State
  {
    initial,
    running,
    exiting
  } state;
};

#endif  // _MotionEngine_h_
