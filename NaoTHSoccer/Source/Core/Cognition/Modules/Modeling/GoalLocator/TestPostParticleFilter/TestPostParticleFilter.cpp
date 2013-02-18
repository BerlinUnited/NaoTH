/**
* @file TestPostParticleFilter.cpp
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*/


#include "TestPostParticleFilter.h"

// Debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

TestPostParticleFilter::TestPostParticleFilter()
    :
      position(1000.0, 0.0)
{
    DEBUG_REQUEST_REGISTER("TestPostParticleFilter:draw_percept", "draw the current goal percept on field", true);
    DEBUG_REQUEST_REGISTER("TestPostParticleFilter:draw_particles", "draw the partciles of the filter", true);

    postParticleFilter.sampleSet.resetLikelihood();
    postParticleFilter.sampleSet.setValid();

    //Filter initialisieren
    for (unsigned int i = 0; i < 10; i++)  {
        postParticleFilter.sampleSet[i].translation = position;
    }

} //Constructor

void TestPostParticleFilter::execute()
{
  // copy the parameters
  postParticleFilter.setParams(theParameters.particleFilter);

  GoalPercept::GoalPost newPost;

  //newPost.positionReliable = true;
  //newPost.color = ColorClasses::none;
  //newPost.type = unknownPost;

  double delta = 1.0;
  MODIFY("TestPostParticleFilter:degreeForRotationPerCycle", delta);

  position.rotate(Math::fromDegrees(delta));

  newPost.position = position;

  postParticleFilter.sampleSet.resetLikelihood();

  postParticleFilter.updateByGoalPostPercept(newPost);


  //wie groß maximal??
  std::vector<GoalPercept::GoalPost> postArr (1);

  postArr[0] = newPost;

  postParticleFilter.resampleGT07(postArr, true);

  debugRequests(newPost);

}

void TestPostParticleFilter::debugRequests(const GoalPercept::GoalPost& newPost)
{
    DEBUG_REQUEST("TestPostParticleFilter:draw_percept",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 30);
      CIRCLE(newPost.position.x, newPost.position.y, 20);
    );

    DEBUG_REQUEST("TestPostParticleFilter:draw_particles",
      FIELD_DRAWING_CONTEXT;
      postParticleFilter.drawParticles("0000FF",88);
    );
}
