/**
 * @file GAWalk.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * use GA to optimize the walk parameters
 */

#include "GAWalk.h"
#include "Tools/NaoInfo.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/DataConversion.h"
#include <DebugCommunication/DebugCommandManager.h>

using namespace std;

GAWalk::GAWalk(const VirtualVision& vv, const RobotPose& rp, const CameraMatrix& cm, const FrameInfo& fi , const FieldInfo& field,
  MotionRequest& mq)
:theVirtualVision(vv),
  theRobotPose(rp),
  theCameraMatrix(cm),
  theFrameInfo(fi),
  theFieldInfo(field),
  theMotionRequest(mq),
  lastResetTime(0),
  fallenCount(0)
{
  maxGeneration = 1000;

  Individual i;

#define REG_WALK_PARAMETER(name, minv, maxv)\
genes[#name] = Vector2<double>(minv, maxv);\
i.gene[#name]

  REG_WALK_PARAMETER(bodyOffsetX, 0, 20) = 10;

//  REG_WALK_PARAMETER(walk.doubleSupportTime, 0);
  REG_WALK_PARAMETER(walk.singleSupportTime, 100, 500) = 400;
//  REG_WALK_PARAMETER(walk.bodyRollOffset, 0);
  REG_WALK_PARAMETER(walk.bodyPitchOffset, 0, 10) = 5;
  REG_WALK_PARAMETER(walk.ZMPOffsetY, -10, 10) =  0;
//  REG_WALK_PARAMETER(walk.ZMPOffsetSpeedX, 0.0);
//  REG_WALK_PARAMETER(walk.ZMPOffsetSpeedY, 0.0);
  REG_WALK_PARAMETER(walk.footOffsetY, -10, 10) = 0;
  REG_WALK_PARAMETER(walk.stepHeight, 5, 50) = 15;
//  REG_WALK_PARAMETER(walk.footPitchOffset, -5, 5, 0);
//  REG_WALK_PARAMETER(walk.footYawOffset, -10, 10, 0);
//  REG_WALK_PARAMETER(walk.leftFootRollOffset, 0.0);
//  REG_WALK_PARAMETER(walk.rightFootRollOffset, 0.0);
  REG_WALK_PARAMETER(walk.footCurveFactor, 5, 15) = 10;
  REG_WALK_PARAMETER(walk.maxStepLength, 50, 150) = 100.0;
//  REG_WALK_PARAMETER(walk.maxStepWidth, 20, 50) = 30.0;
//  REG_WALK_PARAMETER(walk.maxStepTurn, 20, 60) = 40.0;
  REG_WALK_PARAMETER(walk.comHeight, 220, 265) = 260;
//  REG_WALK_PARAMETER(walk.stiffness, 1);
  //REG_WALK_PARAMETER(walk.useArm, -1, 1) = 0;
//  REG_WALK_PARAMETER(walk.enableDynamicStabilizer, 0);
//  REG_WALK_PARAMETER(walk.leftHipRollSingleSupFactor, 1);
//  REG_WALK_PARAMETER(walk.rightHipRollSingleSupFactor, 1);
  //REG_WALK_PARAMETER(walk.comFootDiffTime, -50, 50) = 0;

  //REG_WALK_PARAMETER(arm.shoulderPitchInterialSensorRate, -10, 0) = -10;
  //REG_WALK_PARAMETER(arm.shoulderRollInterialSensorRate, -10, 0) = -10;
  //REG_WALK_PARAMETER(arm.maxSpeed, 10, 300) = 200;

//  REG_WALK_PARAMETER(dynamicStabilizer.inertialSensorFactorX, 100);
//  REG_WALK_PARAMETER(dynamicStabilizer.inertialSensorFactorY, -100);
//  REG_WALK_PARAMETER(dynamicStabilizer.inertialSensorMinX, 1.5); // in degrees
//  REG_WALK_PARAMETER(dynamicStabilizer.inertialSensorMinY, 1.5);

  vector<Individual> initialGeneration;
  initialGeneration.push_back(i);
  initialGeneration.push_back(i); // to make sure inital value is tested correctly
  init(initialGeneration);

  lastTime = theFrameInfo.getTime();
  theTest = theTests.end();
}

void GAWalk::run()
{
  const unsigned int resetingTime = 3000;
  const unsigned int standingTime = 2000;
  const unsigned int runningTime = 20000;

  Vector3<double> mypos;
  if (theVirtualVision.data.find("mypos") != theVirtualVision.data.end())
  {
    // use debug infomation from simulation server
    mypos = theVirtualVision.data.find("mypos")->second;
  } else
  {
    // use self location
    mypos.x = theRobotPose.translation.x;
    mypos.y = theRobotPose.translation.y;
    mypos.z = theCameraMatrix.translation.z;
  }

  if ( mypos.z < NaoInfo::TibiaLength + NaoInfo::ThighLength )
  {
    fallenCount++;
  }
  else
  {
    fallenCount = 0;
  }

  bool isFallenDown = (fallenCount > 3 && lastResetTime + resetingTime < theFrameInfo.getTime());

  Vector2<double> currentPos(mypos.x, mypos.y);

  if (lastResetTime + resetingTime + runningTime + standingTime < theFrameInfo.getTime()
    || isFallenDown || theTest == theTests.end() )
  {
    double fitness =0;
    for( list<Test>::const_iterator iter=theTests.begin(); iter!=theTests.end(); ++iter)
    {
      fitness += iter->getDistance();
    }
    updateFitness(fitness);
    reset();

    theTests.clear();
    theTests.push_back(Test(runningTime, Pose2D(0,10000,0)));
//    theTests.push_back(Test(runningTime/4, Pose2D(0,1000,0)));
//    theTests.push_back(Test(runningTime/4, Pose2D(Math::fromDegrees(30),500,0)));
//    theTests.push_back(Test(runningTime/4, Pose2D(Math::fromDegrees(-30),500,0)));
//    theTests.push_back(Test(runningTime/4, Pose2D(0,-1000,0)));
    theTest = theTests.begin();
  } else if (lastResetTime + resetingTime > theFrameInfo.getTime())
  {
    theMotionRequest.id = motion::stand;
    theMotionRequest.forced = true;
  } else if (lastResetTime + resetingTime + standingTime > theFrameInfo.getTime())
  {
    stringstream answer;
    map<string, string> args;
    args["off"] = "";
    DebugRequest::getInstance().executeDebugCommand("SimSparkController:beam", args, answer);
    theMotionRequest.id = motion::stand;
    theMotionRequest.forced = false;
  } else
  {
    // run
    Pose2D walkReq = theTest->update(theFrameInfo.getTime() - lastTime, currentPos);
    theMotionRequest.id = motion::walk;
    theMotionRequest.walkRequest.target = walkReq;
  }

  if (theTest != theTests.end() && theTest->isFinished())
  {
    theTest++;
    if ( theTest == theTests.end() ) theTest = theTests.begin();
  }
  lastTime = theFrameInfo.getTime();
}

void GAWalk::updateFitness(double fitness)
{
  cout << "GAWalk: fitness = " << fitness << endl;
  GeneticAlgorithms::Individual& i = getIndividual();
  i.fitness = max(0.0, fitness);

  GeneticAlgorithms::Individual& j = getIndividual();

  // set the parameters
  map<string, string> args;
  for( std::map<std::string, Vector2<double> >::const_iterator g=genes.begin();
    g!=genes.end(); ++g)
  {
    j.gene[g->first] = Math::clamp(j.gene[g->first], g->second.x, g->second.y);
    if (g->first == "walk.useArm" || g->first== "walk.enableDynamicStabilizer")
      args[g->first] = DataConversion::toStr(j.gene[g->first]  > 0);
    else
      args[g->first] = DataConversion::toStr(j.gene[g->first]);
  }
  stringstream answer;
  DebugCommandManager::getInstance().handleCommand("ParameterList:IKParameters:set", args, answer);
}

void GAWalk::reset()
{
  lastResetTime = theFrameInfo.getTime();

  stringstream answer;
  map<string,string> args;
  args["on"] = "";
  DebugRequest::getInstance().executeDebugCommand("SimSparkController:beam",args,answer);
  theMotionRequest.id = motion::init;
  fallenCount = 0;
}

GAWalk::Test::Test(unsigned int maxTime, const Pose2D& walkReq)
: started(false),
theMaxTime(maxTime),
theWalkRequest(walkReq)
{

}

Pose2D GAWalk::Test::update(unsigned int time, const Vector2<double>& pos)
{
  theMaxTime -= time;

  if ( !started )
  {
    theStartPos = pos;
    started = true;
  }

  theStopPos = pos;

  return theWalkRequest;
}

bool GAWalk::Test::isFinished() const
{
  return theMaxTime <= 0;
}

double GAWalk::Test::getDistance() const
{
  return (theStopPos-theStartPos).abs();
}

