/*
 * @file VirtualVisionProcessor.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief calculate perceptions from virtual vision
 *
 */

#ifndef _VIRTUAL_VISION_PROCESSOR_H
#define _VIRTUAL_VISION_PROCESSOR_H

#include <set>

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FieldInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>

#include <Representations/Perception/CameraMatrix.h>

#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Modeling/PlayerInfo.h"

#include "Tools/DoubleCamHelpers.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include <Representations/Debug/Stopwatch.h>


BEGIN_DECLARE_MODULE(VirtualVisionProcessor)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(VirtualVision)
  REQUIRE(VirtualVisionTop)
  REQUIRE(PlayerInfo)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
  PROVIDE(GoalPercept)
  PROVIDE(GoalPerceptTop)
  PROVIDE(LinePercept)
  PROVIDE(LinePerceptTop)
  PROVIDE(PlayersPercept)
  //PROVIDE(GPSData)
END_DECLARE_MODULE(VirtualVisionProcessor)

using namespace std;

class VirtualVisionProcessor: public VirtualVisionProcessorBase
{
public:
  VirtualVisionProcessor();
  virtual ~VirtualVisionProcessor();

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

private:

  void execute(const CameraInfo::CameraID id);
  
  /* calculate the object's position in chest coordinates from  virtual vision information (polar)
   */
  Vector3d calculatePosition(const Vector3d& pol);

  void updateBall();
  void updateGoal();
  void updateLine();
  void updatePlayers();
  void updateCorners();

  std::set<std::string> cornerNames;
  std::set<std::string> goalPostNames;
  std::map<std::string, GoalPercept::GoalPost::PostType> goalPostTypes;
  std::map<std::string, ColorClasses::Color> goalPostColors;
  std::map<std::string, Vector2d > flagPosOnField[2];

  class LineName
  {
  public:
    LineName(const std::string& b, const std::string& e):begin(b), end(e){}
    
    std::string begin;
    std::string end;
  };
  std::list<LineName> lineNames;

  // describe the seen points of a player
  class PlayerPoints
  {
  public:
    enum PlayerPointType
    {
      head,
      lfoot,
      rfoot,
      llowerarm,
      rlowerarm,
      numberOfPlayerPointType
    };

    PlayerPoints()
    {
      for(int i = 0; i < numberOfPlayerPointType; i++) {
        seen[i] = false;
    }
    }

    bool set(const std::string& typeName, const Vector3d& pos)
    {
      PlayerPointType type = numberOfPlayerPointType;
      
    if(typeName == "head")
        type = head;
      else if(typeName == "lfoot")
        type = lfoot;
      else if(typeName == "rfoot")
        type = rfoot;
      else if(typeName == "llowerarm")
        type = llowerarm;
      else if(typeName == "rlowerarm")
        type = rlowerarm;

      if(type == numberOfPlayerPointType) return false;
      points[type] = pos;
      seen[type] = true;
      return true;
    }//end set

    Vector3d points[numberOfPlayerPointType];
    bool seen[numberOfPlayerPointType];
    GameData::TeamColor color;
  };
  std::map<int,PlayerPoints> seenPlayerPointsMap;

private:

  void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

  void addLine(const Vector3d& pol0, const Vector3d& pol1);

  /** estimate the intersections from the seen lines */
  void findIntersections();

  /** copied from the LineDetector */
  void classifyIntersections(std::vector<LinePercept::FieldLineSegment>& lineSegments);


  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(VirtualVisionProcessor,CameraInfo);
  DOUBLE_CAM_REQUIRE(VirtualVisionProcessor,CameraMatrix);
  DOUBLE_CAM_REQUIRE(VirtualVisionProcessor,VirtualVision);

  DOUBLE_CAM_PROVIDE(VirtualVisionProcessor,BallPercept);
  DOUBLE_CAM_PROVIDE(VirtualVisionProcessor,GoalPercept);
  DOUBLE_CAM_PROVIDE(VirtualVisionProcessor,LinePercept);
};

#endif // _VIRTUAL_VISION_PROCESSOR_H
