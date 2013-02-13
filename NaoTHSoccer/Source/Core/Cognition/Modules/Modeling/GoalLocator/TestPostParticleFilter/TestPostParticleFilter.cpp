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
    DEBUG_REQUEST_REGISTER("TestPostParticleFilter:draw_percept", "draw the current goal percept on field", false);

} //Constructor

void TestPostParticleFilter::execute()
{

  getGoalPercept().reset();

  GoalPercept::GoalPost newPost;

  //newPost.positionReliable = true;
  //newPost.color = ColorClasses::none;
  //newPost.type = unknownPost;

  double delta = 1.0;
  MODIFY("TestPostParticleFilter:degreeForRotationPerCycle", delta);

  position.rotate(Math::fromDegrees(delta));

  newPost.position = position;

  getGoalPercept().add(newPost);

  DEBUG_REQUEST("TestPostParticleFilter:draw_percept",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 30);
    for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
    {
      const Vector2<double>& percept = getGoalPercept().getPost(i).position;
      CIRCLE(percept.x, percept.y, 20);
    }//end for
  );



}
