/*
 * @file VirtualVisionProcessor.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief calculate perceptions from virtual vision
 *
 */

#include "VirtualVisionProcessor.h"

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/Matrix_mxn.h>
#include <Tools/CameraGeometry.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/DataConversion.h"


VirtualVisionProcessor::VirtualVisionProcessor()
{
  goalPostNames.insert("G1L");
  goalPostNames.insert("G2L");
  goalPostNames.insert("G1R");
  goalPostNames.insert("G2R");

  goalPostTypes["G1L"] = GoalPercept::GoalPost::rightPost;
  goalPostTypes["G2L"] = GoalPercept::GoalPost::leftPost;
  goalPostTypes["G1R"] = GoalPercept::GoalPost::leftPost;
  goalPostTypes["G2R"] = GoalPercept::GoalPost::rightPost;

  // NOTE: this is the configuration of SPL, blue team defence blue goal
  // it is not match the visualization of SimSpark
  goalPostColors["G1L"] = ColorClasses::skyblue;
  goalPostColors["G2L"] = ColorClasses::skyblue;
  goalPostColors["G1R"] = ColorClasses::yellow;
  goalPostColors["G2R"] = ColorClasses::yellow;

  lineNames.push_back(LineName("F1L","F1R"));
  lineNames.push_back(LineName("F2L","F2R"));
  lineNames.push_back(LineName("F1L","F2L"));
  lineNames.push_back(LineName("F1R","F2R"));

  cornerNames.insert("F1L");
  cornerNames.insert("F1R");
  cornerNames.insert("F2L");
  cornerNames.insert("F2R");

  const FieldInfo& theFieldInfo = getFieldInfo();
  flagPosOnField[GameData::blue]["F1L"] = theFieldInfo.crossings[FieldInfo::ownCornerLeft].position;
  flagPosOnField[GameData::blue]["F2L"] = theFieldInfo.crossings[FieldInfo::ownCornerRight].position;
  flagPosOnField[GameData::blue]["F1R"] = theFieldInfo.crossings[FieldInfo::opponentCornerLeft].position;
  flagPosOnField[GameData::blue]["F2R"] = theFieldInfo.crossings[FieldInfo::opponentCornerRight].position;

  flagPosOnField[GameData::red]["F1L"] = theFieldInfo.crossings[FieldInfo::opponentCornerRight].position;
  flagPosOnField[GameData::red]["F2L"] = theFieldInfo.crossings[FieldInfo::opponentCornerLeft].position;
  flagPosOnField[GameData::red]["F1R"] = theFieldInfo.crossings[FieldInfo::ownCornerRight].position;
  flagPosOnField[GameData::red]["F2R"] = theFieldInfo.crossings[FieldInfo::ownCornerLeft].position;

  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:draw_lines","draw the all the seen lines in the field viewer",false);
  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:draw_ball_on_field", "draw the virtual ball", false);
  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:draw_goal_on_field", "draw the virtual goal percept on the field", false);
  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:corner_flags", "draw the corner flags", false);

  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:LineDetector:mark_circle", "", false);
}

VirtualVisionProcessor::~VirtualVisionProcessor()
{
  
}

void VirtualVisionProcessor::execute()
{
  updateBall();
  updateGoal();
  updateLine();
  updateCorners();
  updatePlayers();

  findIntersections();
  LinePercept& theLinePercept = getLinePercept();
  classifyIntersections(theLinePercept.lines);

  DEBUG_REQUEST("VirtualVisionProcessor:draw_lines",
    FIELD_DRAWING_CONTEXT;
    PEN("999999", 20);

    for(vector<LinePercept::FieldLineSegment>::const_iterator iter=theLinePercept.lines.begin(); iter!=theLinePercept.lines.end(); ++iter)
    {
      Vector2<double> b = iter->lineOnField.begin();
      Vector2<double> e = iter->lineOnField.end();
      
      if(iter->partOfCircle)
        PEN("FF0000", 20);

      LINE(b.x, b.y, e.x, e.y);
    }//end for

    PEN("0000FF", 20);
    for(unsigned int i = 0; i < theLinePercept.intersections.size(); i++)
    {
      Vector2<double> b = theLinePercept.intersections[i].getPosOnField();
      CIRCLE(b.x, b.y, 50);
      string type = "N";
      switch(theLinePercept.intersections[i].getType())
      {
        case Math::Intersection::C: type="C"; break;
        case Math::Intersection::T: type="T"; break;
        case Math::Intersection::L: type="L"; break;
        case Math::Intersection::E: type="E"; break;
        case Math::Intersection::X: type="X"; break;
        default: break;
      }
      TEXT_DRAWING(b.x + 100, b.y + 100, type);
    }//end for
  );

  
  DEBUG_REQUEST("VirtualVisionProcessor:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF9900", 20);
    const BallPercept& theBallPercept = getBallPercept();
    CIRCLE(theBallPercept.bearingBasedOffsetOnField.x,
           theBallPercept.bearingBasedOffsetOnField.y,
           45);
  );

  DEBUG_REQUEST("VirtualVisionProcessor:draw_goal_on_field",
    const GoalPercept& theGoalPercept = getGoalPercept();
    const FieldInfo& theFieldInfo = getFieldInfo();
    for(unsigned int n=0; n<theGoalPercept.getNumberOfSeenPosts(); n++)
    {
      const GoalPercept::GoalPost& post = theGoalPercept.getPost(n);

      FIELD_DRAWING_CONTEXT;
      if(post.color==ColorClasses::skyblue && post.positionReliable)
        PEN("8888FF", 20);
      else if(post.color==ColorClasses::yellow && post.positionReliable)
        PEN("FFFF00", 20);
      else
        PEN("FF0000", 20);

      CIRCLE(post.position.x,
             post.position.y,
             theFieldInfo.goalpostRadius);

      std::string typeString("?");
      if(post.type == GoalPercept::GoalPost::leftPost)
        typeString = "L";
      else if(post.type == GoalPercept::GoalPost::rightPost)
        typeString = "R";

      TEXT_DRAWING(post.position.x+50, post.position.y+50, typeString);
    }
  );//end DEBUG

}//end execute

Vector3<double> VirtualVisionProcessor::calculatePosition(const Vector3<double>& pol)
{
  return getCameraMatrix() * pol2xyz(pol);
}//end calculatePosition

void VirtualVisionProcessor::updateBall()
{
  const VirtualVision& theVirtualVision = getVirtualVision();
  BallPercept& theBallPercept = getBallPercept();
  const FrameInfo& theFrameInfo = getFrameInfo();
  const FieldInfo& theFieldInfo = getFieldInfo();
  const CameraInfo& theCameraInfo = getCameraInfo();
  const CameraMatrix& theCameraMatrix = getCameraMatrix();
  map<string, Vector3<double> >::const_iterator iter = theVirtualVision.data.find("B");
  if ( iter != theVirtualVision.data.end() )
  {
    theBallPercept.ballWasSeen = true;
    theBallPercept.frameInfoWhenBallWasSeen = theFrameInfo;
    Vector3<double> p = calculatePosition(iter->second);
    theBallPercept.bearingBasedOffsetOnField.x = p.x;
    theBallPercept.bearingBasedOffsetOnField.y = p.y;

    theBallPercept.radiusInImage = theFieldInfo.ballRadius / iter->second.x * theCameraInfo.focalLength;
    theBallPercept.centerInImage = CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, p);
  }
  else
  {
    theBallPercept.ballWasSeen = false;
  }
}//end updateBall

void VirtualVisionProcessor::updateGoal()
{
  GoalPercept& theGoalPercept = getGoalPercept();
  const VirtualVision& theVirtualVision = getVirtualVision();
  const CameraInfo& theCameraInfo = getCameraInfo();
  const CameraMatrix& theCameraMatrix = getCameraMatrix();
  theGoalPercept.reset();

  GoalPercept::GoalPost yellowPostOne;
  GoalPercept::GoalPost yellowPostTwo;

  GoalPercept::GoalPost bluePostOne;
  GoalPercept::GoalPost bluePostTwo;

  for(set<string>::const_iterator i=goalPostNames.begin(); i!=goalPostNames.end(); ++i)
  {
    map<string, Vector3<double> >::const_iterator j = theVirtualVision.data.find(*i);
    if ( j != theVirtualVision.data.end() )
    {
      Vector3<double> p = calculatePosition(j->second);
      Vector2<double> position(p.x, p.y);

      p.z = 0;
      Vector2<int> basePoint = CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, p);

      ColorClasses::Color color = goalPostColors.find(*i)->second;

      if(color == ColorClasses::yellow)
      {
        if(!yellowPostOne.positionReliable)
        {
          yellowPostOne.basePoint = basePoint;
          yellowPostOne.position = position;
          yellowPostOne.color = color;
          yellowPostOne.position = position;
          yellowPostOne.positionReliable = true;
        }
        else
        { // first post already seen
          yellowPostTwo.basePoint = basePoint;
          yellowPostTwo.position = position;
          yellowPostTwo.color = color;
          yellowPostTwo.position = position;
          yellowPostTwo.positionReliable = true;
        }
      }
      else
      {
        if(!bluePostOne.positionReliable)
        {
          bluePostOne.basePoint = basePoint;
          bluePostOne.position = position;
          bluePostOne.color = color;
          bluePostOne.position = position;
          bluePostOne.positionReliable = true;
        }
        else
        {
          bluePostTwo.basePoint = basePoint;
          bluePostTwo.position = position;
          bluePostTwo.color = color;
          bluePostTwo.position = position;
          bluePostTwo.positionReliable = true;
        }
      }

      //GoalPercept::GoalPost::PostType type = GoalPercept::GoalPost::rightPost; //goalPostTypes.find(*i)->second;
      //theGoalPercept.add(basePoint, position, type, color, true);
    }
  }//end for


  // add the percepts
  if(yellowPostOne.positionReliable)
  {
    if(yellowPostTwo.positionReliable)
    {
      double angle = Math::normalize(yellowPostTwo.position.angle() - yellowPostOne.position.angle());

      if(angle > 0)
      {
        yellowPostOne.type = GoalPercept::GoalPost::rightPost;
        yellowPostTwo.type = GoalPercept::GoalPost::leftPost;
      }
      else
      {
        yellowPostOne.type = GoalPercept::GoalPost::leftPost;
        yellowPostTwo.type = GoalPercept::GoalPost::rightPost;
      }

      theGoalPercept.add(yellowPostOne);
      theGoalPercept.add(yellowPostTwo);
    }
    else
    {
      yellowPostOne.type = GoalPercept::GoalPost::unknownPost;
      theGoalPercept.add(yellowPostOne);
    }
  }//end if yellow

  if(bluePostOne.positionReliable)
  {
    if(bluePostTwo.positionReliable)
    {
      double angle = Math::normalize(bluePostTwo.position.angle() - bluePostOne.position.angle());

      if(angle > 0)
      {
        bluePostOne.type = GoalPercept::GoalPost::rightPost;
        bluePostTwo.type = GoalPercept::GoalPost::leftPost;
      }
      else
      {
        bluePostOne.type = GoalPercept::GoalPost::leftPost;
        bluePostTwo.type = GoalPercept::GoalPost::rightPost;
      }

      theGoalPercept.add(bluePostOne);
      theGoalPercept.add(bluePostTwo);
    }
    else
    {
      bluePostOne.type = GoalPercept::GoalPost::unknownPost;
      theGoalPercept.add(bluePostOne);
    }
  }//end if blue
}//end updateGoal

void VirtualVisionProcessor::updateLine()
{
  LinePercept& theLinePercept = getLinePercept();
  const VirtualVision& theVirtualVision = getVirtualVision();
  theLinePercept.reset();

  // lines from flags
  if (theVirtualVision.lines.empty())
  {
    for (list<LineName>::const_iterator i = lineNames.begin(); i != lineNames.end(); ++i)
    {
      map<string, Vector3<double> >::const_iterator b = theVirtualVision.data.find(i->begin);
      map<string, Vector3<double> >::const_iterator e = theVirtualVision.data.find(i->end);

      if (b != theVirtualVision.data.end() && e != theVirtualVision.data.end())
      {
        addLine(b->second, e->second);
      }
    }//end for
  } else
  {
    for (std::list<VirtualVision::Line>::const_iterator iter = theVirtualVision.lines.begin(); iter != theVirtualVision.lines.end(); ++iter)
    {
      addLine(iter->p0, iter->p1);
    }
  }
}//end updateLine



void VirtualVisionProcessor::findIntersections()
{
  LinePercept& theLinePercept = getLinePercept();
  const CameraInfo& theCameraInfo = getCameraInfo();
  const CameraMatrix& theCameraMatrix = getCameraMatrix();
  for (unsigned int i = 0; i < theLinePercept.lines.size(); i++)
  {
    const Math::LineSegment& lineOne = theLinePercept.lines[i].lineOnField;

    for (unsigned int j = i + 1; j < theLinePercept.lines.size(); j++)
    {
      const Math::LineSegment& lineTwo = theLinePercept.lines[j].lineOnField;
      
      double tOneL = lineOne.Line::intersection(lineTwo);
      double tTwoL = lineTwo.Line::intersection(lineOne);

      Vector2<double> point = lineOne.Line::point(tOneL);
      if ( Math::isInf(point.x) || Math::isNan(point.x)
        || Math::isInf(point.y) || Math::isNan(point.y) )
      {
        continue; // skip bad values
      }

      //LinePercept::Intersection intersection(theCameraMatrix, Platform::getInstance().theCameraInfo, point);
      LinePercept::Intersection intersection;
      intersection.setPosOnField(point);
      Vector3<double> pSpace(point.x, point.y, 0.0);
      Vector2<int> pointInImage = CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, pSpace);
      intersection.setPosInImage(pointInImage);

      double d1 = lineOne.minDistance(intersection.getPosOnField());
      double d2 = lineTwo.minDistance(intersection.getPosOnField());

      if(max(d1, d2) > 150) continue;

      // TODO: do we need it here? (copied from the LineDetector)
      double tOne = Math::clamp(tOneL, 0.0, lineOne.getLength());
      double tTwo = Math::clamp(tTwoL, 0.0, lineTwo.getLength());
      
      intersection.setSegments(i, j, tOne, tTwo);

      theLinePercept.intersections.push_back(intersection);
    }//end for
  }//end for
}//end findIntersections



void VirtualVisionProcessor::classifyIntersections(vector<LinePercept::FieldLineSegment>& lineSegments)
{
  LinePercept& theLinePercept = getLinePercept();
  const double MAX_LINE_ANGLE_DIFF = 0.085; //0.1//0.035
  const double MAX_FIELDLINE_ANGLE_DIFF = 10 * MAX_LINE_ANGLE_DIFF; //0.1//0.035

  vector<Vector2<double> > circlePoints;
  circlePoints.reserve(theLinePercept.intersections.size());

  vector<Vector2<double> > circleMiddlePoints;
  circleMiddlePoints.reserve(theLinePercept.intersections.size());

  // the drawing context is needed for furthrr drawings
  DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
    FIELD_DRAWING_CONTEXT;
  );

  for(unsigned int i = 0; i < theLinePercept.intersections.size(); i++)
  {
    Vector2<unsigned int> indices = theLinePercept.intersections[i].getSegmentIndices();
    LinePercept::FieldLineSegment segOne = lineSegments[indices[0]];
    LinePercept::FieldLineSegment segTwo = lineSegments[indices[1]];

    double angleDiff = fabs(Math::normalize(segOne.lineOnField.getDirection().angle() - segTwo.lineOnField.getDirection().angle()));
    angleDiff = min(angleDiff, Math::pi - angleDiff);

    if(angleDiff < MAX_LINE_ANGLE_DIFF) // shoud never be the case in simulation
    {
      theLinePercept.intersections[i].setType(Math::Intersection::E);
    }
    else if(angleDiff > MAX_LINE_ANGLE_DIFF * 2 && angleDiff < Math::pi_2 - MAX_LINE_ANGLE_DIFF * 2 )
    {
      theLinePercept.intersections[i].setType(Math::Intersection::C);
    }
    else if(angleDiff > Math::pi_2 - MAX_FIELDLINE_ANGLE_DIFF && angleDiff < Math::pi_2 + MAX_FIELDLINE_ANGLE_DIFF )
    {
      Math::Intersection::IntersectionType type = theLinePercept.intersections[i].getType();
      if
      (
        type != Math::Intersection::C &&
        type != Math::Intersection::L &&
        type != Math::Intersection::T &&
        type != Math::Intersection::X 
      )
      {
        Vector2<double> segmentDistancesToIntersection = theLinePercept.intersections[i].getSegmentsDistancesToIntersection();
        bool tIntersectsOne = segmentDistancesToIntersection[0] > segTwo.thickness && segmentDistancesToIntersection[0] < segOne.lineOnField.getLength() - segTwo.thickness;
        bool tIntersectsTwo = segmentDistancesToIntersection[1] > segOne.thickness && segmentDistancesToIntersection[1] < segTwo.lineOnField.getLength() - segOne.thickness;

        if(tIntersectsOne && tIntersectsTwo)
        {
          // maybe X intersection, because if not its overwritten with L or so
          theLinePercept.intersections[i].setType(Math::Intersection::X);
        }else if(tIntersectsOne || tIntersectsTwo)
        {
          // should be an T intersection
          theLinePercept.intersections[i].setType(Math::Intersection::T);
//          cout << " T" << endl;
        }
        else
        {
          // only L intersection
          theLinePercept.intersections[i].setType(Math::Intersection::L);
        }
      }
    }//end if


    if( theLinePercept.intersections[i].getType() == Math::Intersection::C)
    {
      Vector2<double> middlePointOne = (segOne.lineOnField.begin() + segOne.lineOnField.end()) / 2;
      Vector2<double> middlePointTwo = (segTwo.lineOnField.begin() + segTwo.lineOnField.end()) / 2;

      Math::Line lineNormalOne(middlePointOne, segOne.lineOnField.getDirection().rotateLeft());
      Math::Line lineNormalTwo(middlePointTwo, segTwo.lineOnField.getDirection().rotateLeft());

      double p = lineNormalOne.intersection(lineNormalTwo);
      if(p != std::numeric_limits<double>::infinity())
      {
        Vector2<double> intersectPoint = lineNormalOne.point(p);
        DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
          PEN("FFFFFF", 5); 
          LINE(middlePointOne.x, middlePointOne.y, intersectPoint.x, intersectPoint.y);
          LINE(middlePointTwo.x, middlePointTwo.y, intersectPoint.x, intersectPoint.y);

          PEN("FF0000", 30); 
          CIRCLE(middlePointOne.x, middlePointOne.y, 10);
          CIRCLE(middlePointTwo.x, middlePointTwo.y, 10);
          CIRCLE(theLinePercept.intersections[i].getPosOnField().x, theLinePercept.intersections[i].getPosOnField().y, 10);
        );
        circlePoints.push_back(middlePointOne);
        circlePoints.push_back(middlePointTwo);
        circlePoints.push_back(theLinePercept.intersections[i].getPosOnField());
        circleMiddlePoints.push_back(intersectPoint);

        // mark the lines as circle lines
        Vector2<unsigned int> lineIdx = theLinePercept.intersections[i].getSegmentIndices();
        lineSegments[lineIdx[0]].partOfCircle = true;
        lineSegments[lineIdx[1]].partOfCircle = true;
      }//end if
    }//end if
  }//end for


  STOPWATCH_START("LineDetector ~ detect circle");
  if(circlePoints.size() > 3)
  {
    Vector2<double> middle;
    for(unsigned int i = 0; i < circleMiddlePoints.size(); i++)
    {
      middle += circleMiddlePoints[i];
    }
    middle = middle / circleMiddlePoints.size();

    Math::Matrix_mxn<double> A(circlePoints.size(), 3);
    Math::Matrix_mxn<double> l(circlePoints.size(), 1);
    for(unsigned int i = 0; i < circlePoints.size(); i++)
    {
      /*
      A(i, 0) = circlePoints[i].abs2();
      A(i, 1) = 2 * circlePoints[i].x;
      A(i, 2) = 2 * circlePoints[i].y;
      l(i, 0) = -1;
      */
      A(i, 0) = circlePoints[i].x;
      A(i, 1) = circlePoints[i].y;
      A(i, 2) = 1.0;
      l(i, 0) = - circlePoints[i].abs2();
    }//end for
    
    Math::Matrix_mxn<double> AT(A.transpose());
    Math::Matrix_mxn<double> result(((AT * A).invert() * AT) * l);

    //Vector2<double> middle1(-result(1, 0) / result(0, 0), -result(2, 0) / result(0, 0));
    Vector2<double> middle1(-result(0, 0)*0.5, -result(1, 0)*0.5);
  
    if( (middle - middle1).abs() < 150) // 15cm
    {
      theLinePercept.middleCircleWasSeen = true;
      theLinePercept.middleCircleCenter = (middle + middle1) / 2;
    }//end if

    DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
      PEN("FF000099", 10);
      CIRCLE(middle.x, middle.y, 50);
      PEN("0000FF99", 10);
      CIRCLE(middle1.x, middle1.y, 50);
    );
  }//end if

  // TODO: make it more efficient
  // check if the center line is seen
  if(theLinePercept.middleCircleWasSeen)
  {
    for(unsigned int i = 0; i < lineSegments.size(); i++)
    {
      if(!lineSegments[i].partOfCircle)
      {
        double d = lineSegments[i].lineOnField.minDistance(theLinePercept.middleCircleCenter);
        if(d < 300.0)
        {
          lineSegments[i].seen_id = LinePercept::FieldLineSegment::center_line;
          // estimate the orientation of the circle
          // TODO: treat the case if there are several lines
          theLinePercept.middleCircleOrientationWasSeen = true;
          theLinePercept.middleCircleOrientation = lineSegments[i].lineOnField.getDirection();
        }
      }
    }//end for


    DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
      const LinePercept& theLinePercept = getLinePercept();
      const FieldInfo& theFieldInfo = getFieldInfo();
      const Vector2<double>& center = theLinePercept.middleCircleCenter;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, theFieldInfo.centerCircleRadius - 25);

      if(theLinePercept.middleCircleOrientationWasSeen)
      {
        const Vector2<double> direction = theLinePercept.middleCircleOrientation*(theFieldInfo.centerCircleRadius+100);
        LINE(
          center.x + direction.x,
          center.y + direction.y,
          center.x - direction.x,
          center.y - direction.y
          );
      }//end if
    );
  }//end if
  STOPWATCH_STOP("LineDetector ~ detect circle");

}//end classifyIntersections



void VirtualVisionProcessor::addLine(const Vector3<double>& pol0, const Vector3<double>& pol1)
{
  const CameraMatrix& theCameraMatrix = getCameraMatrix();
  const CameraInfo& theCameraInfo = getCameraInfo();
  Vector3<double> bP = calculatePosition(pol0);
  Vector3<double> eP = calculatePosition(pol1);
  Vector2<double> bPos(bP.x, bP.y);
  Vector2<double> ePos(eP.x, eP.y);

  LinePercept::FieldLineSegment line;
  line.thickness = 100; //TODO
  line.set
    (
    CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, bP),
    CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, eP)
    );

  //line.begin = CameraGeometry::relativePointToImage(theCameraMatrix, Platform::getInstance().theCameraInfo, bP);
  //line.end = CameraGeometry::relativePointToImage(theCameraMatrix, Platform::getInstance().theCameraInfo, eP);
  line.lineOnField = Math::LineSegment(bPos, ePos);
  line.valid = true;

  getLinePercept().lines.push_back(line);
}

void VirtualVisionProcessor::updatePlayers()
{
  PlayersPercept& thePlayersPercept = getPlayersPercept();
  const VirtualVision& theVirtualVision = getVirtualVision();
  const PlayerInfo& thePlayerInfo = getPlayerInfo();
  const std::string& teamName = thePlayerInfo.gameData.teamName;
  const unsigned int playerNumber = thePlayerInfo.gameData.playerNumber;
  const GameData::TeamColor teamColor = thePlayerInfo.gameData.teamColor;

  thePlayersPercept.reset();
  thePlayersPercept.theFrameInfo = getFrameInfo();
  seenPlayerPointsMap.clear();

  map<string, Vector3<double> >::const_iterator iter = theVirtualVision.data.begin();
  for(;iter != theVirtualVision.data.end(); ++iter)
  {
    string key = iter->first;
    if(key[0] == 'P')
    {
      /* exapmples
      "P NaoTH-red 1 head"
      "P NaoTH-red 1 lfoot"
      "P NaoTH-red 1 rfoot"
      "P NaoTH-red 1 llowerarm"
      "P NaoTH-red 1 rlowerarm"
      "P <teamname> <nr> <part>"
      */
      vector<string> tokens;
      tokenize(key, tokens); // split by whitespaces

      int id;
      if(!DataConversion::strTo(tokens[2],id))
        continue; // id couldn't be parsed

      // get the name of my team
      if ( tokens[1] != teamName )
        id = -id; // negative id means opponent

      if ( id == playerNumber ) continue;

      if(!seenPlayerPointsMap[id].set(tokens[3], calculatePosition(iter->second)))
        continue; // name of the body part couldn't be parsed
      
      // team color
      seenPlayerPointsMap[id].color = id >0 ? teamColor : !teamColor;
      
    }//end if
  }//end for


  std::map<int,PlayerPoints>::const_iterator playerIter = seenPlayerPointsMap.begin();
  for(;playerIter != seenPlayerPointsMap.end(); ++playerIter)
  {
    PlayersPercept::Player player;

    // estrimate the center of gravity
    Vector3<double> cog;
    double numberOfSeenPoints = 0;
    for(int i = 0; i < PlayerPoints::numberOfPlayerPointType; i++)
    {
      if(playerIter->second.seen[i])
      {
        cog += playerIter->second.points[i];
        numberOfSeenPoints++;
      }
    }//end for
    
    if(numberOfSeenPoints > 0)
      cog *= 1.0/numberOfSeenPoints;
    else
      continue;

    // if the three points are seen estimate the direction 
    // of the robot
    if(playerIter->second.seen[PlayerPoints::head] &&
       playerIter->second.seen[PlayerPoints::lfoot] &&
       playerIter->second.seen[PlayerPoints::rfoot])
    {
      Vector3<double> a = 
        playerIter->second.points[PlayerPoints::rfoot] - 
        playerIter->second.points[PlayerPoints::head];
      Vector3<double> b = 
        playerIter->second.points[PlayerPoints::lfoot] - 
        playerIter->second.points[PlayerPoints::head];
      
      Vector3<double> w = (a^b).normalize();
      player.pose.rotation = (Vector2<double>(w.x,w.y)).angle();
      player.poseRotationValid = true;
    }//end if

    player.pose.translation = Vector2<double>(cog.x, cog.y);
    player.teamColor = playerIter->second.color;
    player.teamColorIsValid = true;
    player.number = abs(playerIter->first);
    player.numberIsValid = true;
    
    thePlayersPercept.addPlayer(player);
  }//end for
}//end updatePlayers



void VirtualVisionProcessor::tokenize(
                      const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }//end while
}//end tokenize

void VirtualVisionProcessor::updateCorners()
{
  const GameData::TeamColor teamColor = getPlayerInfo().gameData.teamColor;
  ASSERT( teamColor == GameData::blue || teamColor == GameData::red );
  const VirtualVision& theVirtualVision = getVirtualVision();

  const map<string, Vector2<double> >& fpof = flagPosOnField[teamColor];

  for(set<string>::const_iterator i=cornerNames.begin(); i!=cornerNames.end(); ++i)
  {
    map<string, Vector3<double> >::const_iterator j = theVirtualVision.data.find(*i);
    if ( j != theVirtualVision.data.end() )
    {
      Vector3<double> p = calculatePosition(j->second);
      Vector2<double> position(p.x, p.y);

      map<string, Vector2<double> >::const_iterator flagPos = fpof.find(*i);

      ASSERT(flagPos != fpof.end());

      LinePercept::Flag flag(position, flagPos->second);

      getLinePercept().flags.push_back(flag);
    }

    DEBUG_REQUEST("VirtualVisionProcessor:corner_flags",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 20);
      const Vector2<double>& p = fpof.find(*i)->second;
      FILLOVAL(p.x, p.y, 20, 20);
      TEXT_DRAWING(p.x+30, p.y+30, *i);
    );
  }//end for
}//end updateCorners

/*
void VirtualVisionProcessor::updateGPS()
{

  const VirtualVision& theVirtualVision = getVirtualVision();
  if (theVirtualVision.data.find("mypos") != theVirtualVision.data.end())
  {
    // only translation is provided
    theGPSData.data.translation = theVirtualVision.data.find("mypos")->second;
  }
}
*/
