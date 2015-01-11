/**
 * @file Evolution.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Evolution.h"
#include "GAWalk.h"

Evolution::Evolution()
  :ga(NULL)
{
  ga = new GAWalk(getVirtualVision(), getRobotPose(), getCameraMatrix(), getFrameInfo(), getFieldInfo(), getMotionRequest());
}

Evolution::~Evolution()
{
  delete ga;
}

void Evolution::execute()
{
  ASSERT( NULL != ga  );

  if ( !(ga->isFinished()) )
  {
    ga->run();
  }
  else
  {
    std::cout<<"Evolution finished!"<<std::endl;
    exit(0);
  }
}
