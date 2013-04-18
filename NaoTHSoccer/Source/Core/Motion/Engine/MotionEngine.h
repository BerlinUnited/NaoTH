/**
 * @file MotionEngine.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _MotionEngine_h_
#define _MotionEngine_h_

#include "AbstractMotion.h"
#include "MotionFactory.h"
#include "HeadMotion/HeadMotionEngine.h"
#include "ArmMotion/IKArmGrasping.h"
#include "ArmMotion/ArmMotionEngine.h"

#include <list>

#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"


BEGIN_DECLARE_MODULE(MotionEngine)
  REQUIRE(FrameInfo)

  PROVIDE(MotionLock) // it's unlocked if a motion is forced

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(MotionStatus)
END_DECLARE_MODULE(MotionEngine)


class MotionEngine: private MotionEngineBase, private ModuleManager
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
  ModuleCreator<IKArmGrasping>* theIKArmGrasping;
  ModuleCreator<ArmMotionEngine>* theArmMotionEngine;

  
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
