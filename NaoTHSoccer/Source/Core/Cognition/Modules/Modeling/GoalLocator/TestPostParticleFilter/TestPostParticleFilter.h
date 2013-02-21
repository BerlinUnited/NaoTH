/**
* @file TestPostParticleFilter.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*/

#ifndef _TestPostParticleFilter_h_
#define _TestPostParticleFilter_h_

#include <ModuleFramework/Module.h>

#include "Cognition/Modules/Modeling/GoalLocator/ActiveGoalLocator/PostParticleFilter.h"

#include "Tools/DataStructures/ParameterList.h"
#include <Tools/Debug/DebugParameterList.h>

#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h" //for particlefilter

// Tools
#include "Tools/CameraGeometry.h" //???

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(TestPostParticleFilter)
    REQUIRE(CameraMatrix)
END_DECLARE_MODULE(TestPostParticleFilter)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class TestPostParticleFilter : private TestPostParticleFilterBase
{
public:
    TestPostParticleFilter();
    virtual ~TestPostParticleFilter(){}

    virtual void execute();

    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("TPPFParameters")
        {
          PARAMETER_REGISTER(particleFilter.standardDeviationAngle) = 0.15;
          PARAMETER_REGISTER(particleFilter.motionNoiseDistance) = 0;
          PARAMETER_REGISTER(particleFilter.processNoiseDistance) = 5;
          PARAMETER_REGISTER(particleFilter.resamplingThreshhold) = 0.99;

          syncWithConfig();
          DebugParameterList::getInstance().add(this);
        }

        ~Parameters() {
          DebugParameterList::getInstance().remove(this);
        }

      PostParticleFilter::Parameters particleFilter;
    } theParameters;

private:
    Vector2<double> position;
    PostParticleFilter postParticleFilter;

    void debugRequests(const GoalPercept::GoalPost& newPost);
};

#endif // _TestPostParticleFilter_h_
