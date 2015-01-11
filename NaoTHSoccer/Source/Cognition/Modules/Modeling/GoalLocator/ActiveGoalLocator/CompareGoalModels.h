/**
* @file CompareGoalModels.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*/

#ifndef _CompareGoalModels_h_
#define _CompareGoalModels_h_

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/GoalModel.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CompareGoalModels)
  REQUIRE(LocalGoalModel)
  REQUIRE(SensingGoalModel)
  REQUIRE(SelfLocGoalModel)
END_DECLARE_MODULE(CompareGoalModels)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CompareGoalModels : private CompareGoalModelsBase
{
    public:
      CompareGoalModels();
      virtual ~CompareGoalModels(){}

      /** */
      virtual void execute();

      // debug stuff
      void plotXY();

};

#endif // _CompareGoalModels_h_
