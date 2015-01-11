
/**
 * @file CompareGoalModels.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class CompareGoalModels
 */

#include "CompareGoalModels.h"


// Debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"


CompareGoalModels::CompareGoalModels()
{

    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorLeftPostOfSelfLocGM", "", true);
    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorRightPostOfSelfLocGM", "", true);
    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorLeftPostOfLocGM", "", true);
    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorRightPostOfLocGM", "", true);
    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorAngleToCenterLocGM", "", true);
    DEBUG_REQUEST_REGISTER("CompareGoalModels:errorAngleToCenterSelfLocGM", "", true);


} //Constructor


void CompareGoalModels::execute()
{

    double errorLeftPostOfSelfLocGM = (getSensingGoalModel().goal.leftPost - getSelfLocGoalModel().goal.leftPost).abs();
    PLOT("CompareGoalModels:errorLeftPostOfSelfLocGM", errorLeftPostOfSelfLocGM);

    double errorRightPostOfSelfLocGM = (getSensingGoalModel().goal.rightPost - getSelfLocGoalModel().goal.rightPost).abs();
    PLOT("CompareGoalModels:errorRightPostOfSelfLocGM", errorRightPostOfSelfLocGM);

    double errorLeftPostOfLocGM = (getSensingGoalModel().goal.leftPost - getLocalGoalModel().goal.leftPost).abs();
    PLOT("CompareGoalModels:errorLeftPostOfLocGM", errorLeftPostOfLocGM);

    double errorRightPostOfLocGM = (getSensingGoalModel().goal.rightPost - getLocalGoalModel().goal.rightPost).abs();
    PLOT("CompareGoalModels:errorRightPostOfLocGM", errorRightPostOfLocGM);

    double errorAngleToCenterLocGM = getSensingGoalModel().goal.calculateCenter().angle() - getLocalGoalModel().goal.calculateCenter().angle();
    PLOT("CompareGoalModels:errorAngleToCenterLocGM", Math::toDegrees(errorAngleToCenterLocGM));

    double errorAngleToCenterSelfLocGM = getSensingGoalModel().goal.calculateCenter().angle() - getSelfLocGoalModel().goal.calculateCenter().angle();
    PLOT("CompareGoalModels:errorAngleToCenterSelfLocGM", Math::toDegrees(errorAngleToCenterSelfLocGM));

}

