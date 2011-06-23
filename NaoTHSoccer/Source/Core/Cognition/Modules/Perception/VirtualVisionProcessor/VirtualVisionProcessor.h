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

BEGIN_DECLARE_MODULE(VirtualVisionProcessor)
  REQUIRE(PlayerInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraMatrix)
  REQUIRE(VirtualVision)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
  PROVIDE(GoalPercept)
  PROVIDE(LinePercept)
  PROVIDE(PlayersPercept)
  //PROVIDE(GPSData)
END_DECLARE_MODULE(VirtualVisionProcessor)

class VirtualVisionProcessor: public VirtualVisionProcessorBase
{
public:
  VirtualVisionProcessor();
  virtual ~VirtualVisionProcessor();

  virtual void execute();

private:
  /* calculate the object's position in chest coordinates from  virtual vision information (polar)
   */
  Vector3<double> calculatePosition(const Vector3<double>& pol);

  void updateBall();
  void updateGoal();
  void updateLine();
  void updatePlayers();
  void updateCorners();

  std::set<std::string> cornerNames;
  std::set<std::string> goalPostNames;
  std::map<std::string, GoalPercept::GoalPost::PostType> goalPostTypes;
  std::map<std::string, ColorClasses::Color> goalPostColors;
  std::map<std::string, Vector2<double> > flagPosOnField[2];

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
      for(int i = 0; i < numberOfPlayerPointType; i++)
        seen[i] = false;
    }

    bool set(const std::string& typeName, const Vector3<double>& pos)
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

    Vector3<double> points[numberOfPlayerPointType];
    bool seen[numberOfPlayerPointType];
    GameData::TeamColor color;
  };
  std::map<int,PlayerPoints> seenPlayerPointsMap;

private:

  void tokenize(const string& str, vector<string>& tokens,const string& delimiters = " ");

  void addLine(const Vector3<double>& pol0, const Vector3<double>& pol1);

  /** estimate the intersections from the seen lines */
  void findIntersections();

  /** copied from the LineDetector */
  void classifyIntersections(vector<LinePercept::FieldLineSegment>& lineSegments);
};

#endif // _VIRTUAL_VISION_PROCESSOR_H
