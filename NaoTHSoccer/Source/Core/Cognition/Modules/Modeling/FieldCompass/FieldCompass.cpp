/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class ActiveGoalLocator
 */

#include "FieldCompass.h"

//for MODIFY
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/Stopwatch.h"

//MATH
#include "Tools/Math/Probabilistics.h"
#include "Representations/Modeling/GoalModel.h"
//#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/SampleSet.h"
#include <cmath>

FieldCompass::FieldCompass() {

} //Constructor

void FieldCompass::execute() {

    getCompassDirection().angle = getRobotPose().rotation;

}//end FieldCompass::execute()
