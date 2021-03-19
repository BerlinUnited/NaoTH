/*
 * @file VirtualVisionProcessor.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief calculate perceptions from virtual vision
 *
 */

#include "VirtualVisionProcessor.h"

#include <Tools/Math/Matrix_mxn.h>
#include <Tools/StringTools.h>
#include "Tools/CameraGeometry.h"

using namespace std;

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

  flagPosOnField[GameData::blue]["F1L"] = getFieldInfo().crossings[FieldInfo::ownCornerLeft].position;
  flagPosOnField[GameData::blue]["F2L"] = getFieldInfo().crossings[FieldInfo::ownCornerRight].position;
  flagPosOnField[GameData::blue]["F1R"] = getFieldInfo().crossings[FieldInfo::opponentCornerLeft].position;
  flagPosOnField[GameData::blue]["F2R"] = getFieldInfo().crossings[FieldInfo::opponentCornerRight].position;

  flagPosOnField[GameData::red]["F1L"] = getFieldInfo().crossings[FieldInfo::opponentCornerRight].position;
  flagPosOnField[GameData::red]["F2L"] = getFieldInfo().crossings[FieldInfo::opponentCornerLeft].position;
  flagPosOnField[GameData::red]["F1R"] = getFieldInfo().crossings[FieldInfo::ownCornerRight].position;
  flagPosOnField[GameData::red]["F2R"] = getFieldInfo().crossings[FieldInfo::ownCornerLeft].position;

  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:corner_flags", "draw the corner flags", false);

  DEBUG_REQUEST_REGISTER("VirtualVisionProcessor:LineDetector:mark_circle", "", false);
}

VirtualVisionProcessor::~VirtualVisionProcessor()
{
  
}

void VirtualVisionProcessor::execute()
{
  getMultiBallPercept().reset();
  getVirtualLinePercept().reset();

  execute(CameraInfo::Top);
  execute(CameraInfo::Bottom);
}

void VirtualVisionProcessor::execute(const CameraInfo::CameraID id)
{
  cameraID = id;

  updateBall();
  updateGoal();
  
  updatePlayers();

  // line detection
  updateLine();
  findIntersections();
  
  updateCorners();
}//end execute

Vector3d VirtualVisionProcessor::calculatePosition(const Vector3d& pol)
{
  return getCameraMatrix() * pol2xyz(pol);
}

void VirtualVisionProcessor::updateBall()
{
  //NOTE: MultiBallPercept is used for both cameras and is reset in execute()
  //getMultiBallPercept().reset();

  VirtualVision::PointMap::const_iterator iter = getVirtualVision().data.find("B");
  if ( iter != getVirtualVision().data.end() )
  {
    Vector3d p = calculatePosition(iter->second);
    
    // create a ball percept  
    MultiBallPercept::BallPercept ballPercept;
    ballPercept.cameraId = cameraID;
    CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), p, ballPercept.centerInImage);
    ballPercept.radiusInImage = getFieldInfo().ballRadius / iter->second.x * getCameraInfo().getFocalLength();
    ballPercept.positionOnField.x = p.x;
    ballPercept.positionOnField.y = p.y;

    getMultiBallPercept().add(ballPercept);
    getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
  }
}//end updateBall

void VirtualVisionProcessor::updateGoal()
{
  GoalPercept& theGoalPercept = getGoalPercept();
  const VirtualVision& theVirtualVision = getVirtualVision();
  const CameraInfo& theCameraInfo = getCameraInfo();
  const CameraMatrix& theCameraMatrix = getCameraMatrix();
  theGoalPercept.reset();
  theGoalPercept.horizonScan = true;

  GoalPercept::GoalPost yellowPostOne;
  GoalPercept::GoalPost yellowPostTwo;

  GoalPercept::GoalPost bluePostOne;
  GoalPercept::GoalPost bluePostTwo;

  for(set<string>::const_iterator i=goalPostNames.begin(); i!=goalPostNames.end(); ++i)
  {
    map<string, Vector3d>::const_iterator j = theVirtualVision.data.find(*i);
    if ( j != theVirtualVision.data.end() )
    {
      Vector3d p = calculatePosition(j->second);
      Vector2d position(p.x, p.y);

      p.z = 0;
      Vector2i basePoint;
      CameraGeometry::relativePointToImage(theCameraMatrix, theCameraInfo, p, basePoint);

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
  //NOTE: VirtualLinePercept is used for both cameras and is reset in execute()
  //getVirtualLinePercept().reset();

  const VirtualVision& theVirtualVision = getVirtualVision();

  // lines from flags
  if (theVirtualVision.lines.empty())
  {
    for (list<LineName>::const_iterator i = lineNames.begin(); i != lineNames.end(); ++i)
    {
      map<string, Vector3d>::const_iterator b = theVirtualVision.data.find(i->begin);
      map<string, Vector3d>::const_iterator e = theVirtualVision.data.find(i->end);

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
  const std::vector<Math::LineSegment>& lines = getVirtualLinePercept().fieldLineSegments;
  std::vector<LineIntersection>& intersections = getVirtualLinePercept().intersections;

  
  vector<Vector2d> circlePoints;
  circlePoints.reserve(intersections.size());

  vector<Vector2d> circleMiddlePoints;
  circleMiddlePoints.reserve(intersections.size());

  for (size_t i = 0; i < lines.size(); i++) {
    for (size_t j = i + 1; j < lines.size(); j++) {
      LineIntersection intersection = LinesTable::estimateIntersection(lines[i], lines[j]);
      intersections.push_back(intersection);

      // extract circle from intersections
      if(intersection.type == LineIntersection::C) 
      {
        Vector2d middlePointOne = (lines[i].begin() + lines[i].end()) / 2;
        Vector2d middlePointTwo = (lines[j].begin() + lines[j].end()) / 2;

        Math::Line lineNormalOne(middlePointOne, lines[i].getDirection().rotateLeft());
        Math::Line lineNormalTwo(middlePointTwo, lines[j].getDirection().rotateLeft());

        double p = lineNormalOne.intersection(lineNormalTwo);
        if(p != std::numeric_limits<double>::infinity())
        {
          Vector2d intersectPoint = lineNormalOne.point(p);
          DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
            PEN("FFFFFF", 5); 
            LINE(middlePointOne.x, middlePointOne.y, intersectPoint.x, intersectPoint.y);
            LINE(middlePointTwo.x, middlePointTwo.y, intersectPoint.x, intersectPoint.y);

            PEN("FF0000", 30); 
            CIRCLE(middlePointOne.x, middlePointOne.y, 10);
            CIRCLE(middlePointTwo.x, middlePointTwo.y, 10);
            CIRCLE(intersection.pos.x, intersection.pos.y, 10);
          );

          circlePoints.push_back(middlePointOne);
          circlePoints.push_back(middlePointTwo);
          circlePoints.push_back(intersection.pos);
          circleMiddlePoints.push_back(intersectPoint);

          // mark the lines as circle lines
          //Vector2<unsigned int> lineIdx = intersections[i].getSegmentIndices();
          //lineSegments[lineIdx[0]].type = LinePercept::C;
          //lineSegments[lineIdx[1]].type = LinePercept::C;
        }//end if
      }
    }
  }


  STOPWATCH_START("LineDetector ~ detect circle");
  if(circlePoints.size() > 3)
  {
    Vector2d middle;
    for(size_t i = 0; i < circleMiddlePoints.size(); i++) {
      middle += circleMiddlePoints[i];
    }
    middle = middle / static_cast<double> (circleMiddlePoints.size());

    Vector2d middle1;
    double radius1;
    if( Geometry::estimateCircle(circlePoints, middle1, radius1) && (middle - middle1).abs() < 150) // 15cm
    {
      getVirtualLinePercept().middleCircle.wasSeen = true;
      getVirtualLinePercept().middleCircle.center = (middle + middle1) / 2;
    }

    DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
      PEN("FF000099", 10);
      CIRCLE(middle.x, middle.y, 50);
      PEN("0000FF99", 10);
      CIRCLE(middle1.x, middle1.y, 50);
    );
  }//end if

  DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
    if(getVirtualLinePercept().middleCircle.wasSeen) {
      const Vector2d& center = getVirtualLinePercept().middleCircle.center;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);
  });

  STOPWATCH_STOP("LineDetector ~ detect circle");

}//end findIntersections

/*
void VirtualVisionProcessor::classifyIntersectionsDetectCircle()
{
  vector<LinePercept::FieldLineSegment>& lineSegments = getLinePercept().lines;
  std::vector<LinePercept::Intersection>& intersections = getLinePercept().intersections;

  const double MAX_LINE_ANGLE_DIFF = 0.085; //0.1//0.035
  const double MAX_FIELDLINE_ANGLE_DIFF = 10 * MAX_LINE_ANGLE_DIFF; //0.1//0.035

  vector<Vector2d> circlePoints;
  circlePoints.reserve(intersections.size());

  vector<Vector2d> circleMiddlePoints;
  circleMiddlePoints.reserve(intersections.size());

  // the drawing context is needed for furthrr drawings
  DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
    FIELD_DRAWING_CONTEXT;
  );

  for(size_t i = 0; i < intersections.size(); i++)
  {
    Vector2<unsigned int> indices = intersections[i].getSegmentIndices();
    const LinePercept::FieldLineSegment& segOne = lineSegments[indices[0]];
    const LinePercept::FieldLineSegment& segTwo = lineSegments[indices[1]];

    double angleDiff = fabs(Math::normalize(segOne.lineOnField.getDirection().angle() - segTwo.lineOnField.getDirection().angle()));
    angleDiff = min(angleDiff, Math::pi - angleDiff);

    if(angleDiff < MAX_LINE_ANGLE_DIFF) // shoud never be the case in simulation
    {
      intersections[i].setType(LineIntersection::E);
    }
    else if(angleDiff > MAX_LINE_ANGLE_DIFF * 2 && angleDiff < Math::pi_2 - MAX_LINE_ANGLE_DIFF * 2 )
    {
      intersections[i].setType(LineIntersection::C);
    }
    else if(angleDiff > Math::pi_2 - MAX_FIELDLINE_ANGLE_DIFF && angleDiff < Math::pi_2 + MAX_FIELDLINE_ANGLE_DIFF )
    {
      LineIntersection::Type type = intersections[i].getType();
      if
      (
        type != LineIntersection::C &&
        type != LineIntersection::L &&
        type != LineIntersection::T &&
        type != LineIntersection::X 
      )
      {
        Vector2d segmentDistancesToIntersection = intersections[i].getSegmentsDistancesToIntersection();
        bool tIntersectsOne = segmentDistancesToIntersection[0] > segTwo.lineInImage.thickness && segmentDistancesToIntersection[0] < segOne.lineOnField.getLength() - segTwo.lineInImage.thickness;
        bool tIntersectsTwo = segmentDistancesToIntersection[1] > segOne.lineInImage.thickness && segmentDistancesToIntersection[1] < segTwo.lineOnField.getLength() - segOne.lineInImage.thickness;

        if(tIntersectsOne && tIntersectsTwo)
        {
          // maybe X intersection, because if not its overwritten with L or so
          intersections[i].setType(LineIntersection::X);
        } 
        else if(tIntersectsOne || tIntersectsTwo)
        {
          // should be an T intersection
          intersections[i].setType(LineIntersection::T);
//          cout << " T" << endl;
        }
        else
        {
          // only L intersection
          intersections[i].setType(LineIntersection::L);
        }
      }
    }//end if


    if( intersections[i].getType() == LineIntersection::C)
    {
      Vector2d middlePointOne = (segOne.lineOnField.begin() + segOne.lineOnField.end()) / 2;
      Vector2d middlePointTwo = (segTwo.lineOnField.begin() + segTwo.lineOnField.end()) / 2;

      Math::Line lineNormalOne(middlePointOne, segOne.lineOnField.getDirection().rotateLeft());
      Math::Line lineNormalTwo(middlePointTwo, segTwo.lineOnField.getDirection().rotateLeft());

      double p = lineNormalOne.intersection(lineNormalTwo);
      if(p != std::numeric_limits<double>::infinity())
      {
        Vector2d intersectPoint = lineNormalOne.point(p);
        DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
          PEN("FFFFFF", 5); 
          LINE(middlePointOne.x, middlePointOne.y, intersectPoint.x, intersectPoint.y);
          LINE(middlePointTwo.x, middlePointTwo.y, intersectPoint.x, intersectPoint.y);

          PEN("FF0000", 30); 
          CIRCLE(middlePointOne.x, middlePointOne.y, 10);
          CIRCLE(middlePointTwo.x, middlePointTwo.y, 10);
          CIRCLE(intersections[i].getPosOnField().x, intersections[i].getPosOnField().y, 10);
        );

        circlePoints.push_back(middlePointOne);
        circlePoints.push_back(middlePointTwo);
        circlePoints.push_back(intersections[i].getPosOnField());
        circleMiddlePoints.push_back(intersectPoint);

        // mark the lines as circle lines
        Vector2<unsigned int> lineIdx = intersections[i].getSegmentIndices();
        lineSegments[lineIdx[0]].type = LinePercept::C;
        lineSegments[lineIdx[1]].type = LinePercept::C;
      }//end if
    }//end if
  }//end for


  STOPWATCH_START("LineDetector ~ detect circle");
  if(circlePoints.size() > 3)
  {
    Vector2d middle;
    for(size_t i = 0; i < circleMiddlePoints.size(); i++) {
      middle += circleMiddlePoints[i];
    }
    middle = middle / static_cast<double> (circleMiddlePoints.size());

    Vector2d middle1;
    double radius1;
    if( Geometry::estimateCircle(circlePoints, middle1, radius1) && (middle - middle1).abs() < 150) // 15cm
    {
      getLinePercept().middleCircleWasSeen = true;
      getLinePercept().middleCircleCenter = (middle + middle1) / 2;
    }

    DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
      PEN("FF000099", 10);
      CIRCLE(middle.x, middle.y, 50);
      PEN("0000FF99", 10);
      CIRCLE(middle1.x, middle1.y, 50);
    );
  }//end if

  DEBUG_REQUEST("VirtualVisionProcessor:LineDetector:mark_circle",
    if(getLinePercept().middleCircleWasSeen) {
      const Vector2d& center = getLinePercept().middleCircleCenter;
      PEN("FFFFFF99", 10);
      CIRCLE(center.x, center.y, 50);
      PEN("FFFFFF99", 50);
      CIRCLE(center.x, center.y, getFieldInfo().centerCircleRadius - 25);
  });

  STOPWATCH_STOP("LineDetector ~ detect circle");

}//end classifyIntersections
*/


void VirtualVisionProcessor::addLine(const Vector3d& pol0, const Vector3d& pol1)
{
  // calculate the relative coordinates of the line
  const Vector3d bP = calculatePosition(pol0);
  const Vector3d eP = calculatePosition(pol1);
  const Vector2d bPos(bP.x, bP.y);
  const Vector2d ePos(eP.x, eP.y);
  
  //LinePercept::FieldLineSegment line;
  //line.lineOnField = Math::LineSegment(bPos, ePos);

  // NOTE: we currently don't need that
  // reprojection of the line in the image
  /*
  Vector2i p1, p2;
  if( CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), bP, p1) &&
      CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), eP, p2))
  {
    line.lineInImage.segment = Math::LineSegment(p1, p2);
  }
  */
  
  //getLinePercept().lines.push_back(line);
  getVirtualLinePercept().fieldLineSegments.emplace_back(bPos, ePos);
}//end addLine

void VirtualVisionProcessor::updatePlayers()
{
  getPlayersPercept().reset();
  getPlayersPercept().theFrameInfo = getFrameInfo();
  seenPlayerPointsMap.clear();

  map<string, Vector3d >::const_iterator iter = getVirtualVision().data.begin();
  for(;iter != getVirtualVision().data.end(); ++iter)
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
      if(!StringTools::strTo(tokens[2],id)) {
        continue; // id couldn't be parsed
      }

      // get the name of my team
      if ( tokens[1] != getPlayerInfo().teamName ) {
        id = -id; // negative id means opponent
      }

      if ( id == (int)getPlayerInfo().playerNumber ) {
        continue;
      }

      if(!seenPlayerPointsMap[id].set(tokens[3], calculatePosition(iter->second))) {
        continue; // name of the body part couldn't be parsed
      }
      
      // team color
      if(id > 0) {
        seenPlayerPointsMap[id].color = getPlayerInfo().teamColor;
      } else {
        seenPlayerPointsMap[id].color = (getPlayerInfo().teamColor == GameData::red)?GameData::blue:GameData::red;
      }
      
    }//end if
  }//end for


  std::map<int,PlayerPoints>::const_iterator playerIter = seenPlayerPointsMap.begin();
  for(;playerIter != seenPlayerPointsMap.end(); ++playerIter)
  {
    PlayersPercept::Player player;

    // estrimate the center of gravity
    Vector3d cog;
    double numberOfSeenPoints = 0;
    for(int i = 0; i < PlayerPoints::numberOfPlayerPointType; i++)
    {
      if(playerIter->second.seen[i])
      {
        cog += playerIter->second.points[i];
        numberOfSeenPoints++;
      }
    }//end for
    
    if(numberOfSeenPoints > 0) {
      cog *= 1.0/numberOfSeenPoints;
    } else {
      continue;
    }

    // if the three points are seen estimate the direction 
    // of the robot
    if(playerIter->second.seen[PlayerPoints::head] &&
       playerIter->second.seen[PlayerPoints::lfoot] &&
       playerIter->second.seen[PlayerPoints::rfoot])
    {
      Vector3d a = 
        playerIter->second.points[PlayerPoints::rfoot] - 
        playerIter->second.points[PlayerPoints::head];
      Vector3d b = 
        playerIter->second.points[PlayerPoints::lfoot] - 
        playerIter->second.points[PlayerPoints::head];
      
      Vector3d w = (a^b).normalize();
      player.pose.rotation = (Vector2d(w.x,w.y)).angle();
      player.poseRotationValid = true;
    }//end if

    player.pose.translation = Vector2d(cog.x, cog.y);
    player.teamColor = playerIter->second.color;
    player.teamColorIsValid = true;
    player.number = abs(playerIter->first);
    player.numberIsValid = true;
    
    getPlayersPercept().addPlayer(player);
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
  const GameData::TeamColor teamColor = getPlayerInfo().teamColor;
  ASSERT( teamColor == GameData::blue || teamColor == GameData::red );
  ASSERT( teamColor == 0 || teamColor == 1 );
  
  const map<string, Vector2d >& fpof = flagPosOnField[teamColor];

  for(set<string>::const_iterator i=cornerNames.begin(); i!=cornerNames.end(); ++i)
  {
    map<string, Vector3d >::const_iterator j = getVirtualVision().data.find(*i);
    if ( j != getVirtualVision().data.end() )
    {
      Vector3d p = calculatePosition(j->second);
      Vector2d position(p.x, p.y);

      map<string, Vector2d >::const_iterator flagPos = fpof.find(*i);

      ASSERT(flagPos != fpof.end());

      getVirtualLinePercept().flags.emplace_back(position, flagPos->second);
    }

    DEBUG_REQUEST("VirtualVisionProcessor:corner_flags",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 20);
      const Vector2d& p = fpof.find(*i)->second;
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
