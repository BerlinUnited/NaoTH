/**
* @file TestPostParticleFilter.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*/

#ifndef _TestPostParticleFilter_h_
#define _TestPostParticleFilter_h_

#include <ModuleFramework/Module.h>

#include "Representations/Perception/GoalPercept.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(TestPostParticleFilter)
  PROVIDE(GoalPercept)
END_DECLARE_MODULE(TestPostParticleFilter)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class TestPostParticleFilter : private TestPostParticleFilterBase
{
public:
    TestPostParticleFilter();
    virtual ~TestPostParticleFilter(){}

    /** */
    virtual void execute();

private:
    Vector2<double> position;

};

#endif // _TestPostParticleFilter_h_
