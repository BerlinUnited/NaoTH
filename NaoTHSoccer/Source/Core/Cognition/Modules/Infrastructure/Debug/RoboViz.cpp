/**
 * @file RoboViz.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#include "RoboViz.h"
#include <iomanip>
#include "Tools/Debug/DebugRequest.h"
#include "PlatformInterface/Platform.h"

using namespace std;

RoboViz::RoboViz()
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  string interfaceName = "lo";
  if(config.hasKey("RoboViz", "interface"))
  {
    interfaceName = config.getString("RoboViz", "interface");
  }

  unsigned int port = 32769;
  if(config.hasKey("RoboViz", "port"))
  {
    port = config.getInt("RoboViz", "port");
  }

  theBroadCaster = new BroadCaster(interfaceName, port);

  DEBUG_REQUEST_REGISTER("RoboViz:test", "test drawing of RoboViz", false);
}

RoboViz::~RoboViz()
{
  delete theBroadCaster;
}

/*
 ----------
|0         | - 1 byte
|----------|
|0         | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::swapBuffers(const string& setName)
{
  stringstream cmd;
  cmd << static_cast<char>(0)
      << static_cast<char>(0)
      << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|1         | - 1 byte
|----------|
|0         | - 1 byte
|----------|
|Position X| - 6 bytes (float/string)
|----------|
|Position Y| - 6 bytes (float/string)
|----------|
|Radius    | - 6 bytes (float/string)
|----------|
|Thickness | - 6 bytes (float/string)
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawCircle(const Vector2d& pos, double radius, double thickness, const Vector3<unsigned char>& color, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(1)
      << static_cast<char>(0)
      << setw(6) << pos.x * 1e-3
      << setw(6) << pos.y * 1e-3
      << setw(6) << radius * 1e-3
      << setw(6) << thickness
      << color.x
      << color.y
      << color.z
      << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|1         | - 1 byte
|----------|
|1         | - 1 byte
|----------|
|Start X   | - 6 bytes (float/string)
|----------|
|Start Y   | - 6 bytes (float/string)
|----------|
|Start Z   | - 6 bytes (float/string)
|----------|
|End X     | - 6 bytes (float/string)
|----------|
|End Y     | - 6 bytes (float/string)
|----------|
|End Z     | - 6 bytes (float/string)
|----------|
|Thickness | - 6 bytes (float/string)
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawLine(const Vector3d& start, const Vector3d& end, double thickness, const Vector3<unsigned char>& color, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(1)
      << static_cast<char>(1)
      << setw(6) << start.x * 1e-3
      << setw(6) << start.y * 1e-3
      << setw(6) << start.z * 1e-3
      << setw(6) << end.x * 1e-3
      << setw(6) << end.y * 1e-3
      << setw(6) << end.z * 1e-3
      << setw(6) << thickness
      << color.x
      << color.y
      << color.z
      << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|1         | - 1 byte
|----------|
|2         | - 1 byte
|----------|
|Position X| - 6 bytes (float/string)
|----------|
|Position Y| - 6 bytes (float/string)
|----------|
|Position Z| - 6 bytes (float/string)
|----------|
|Size      | - 6 bytes (float/string)
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawPoint(const Vector3d& p, double size, const Vector3<unsigned char>& color, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(1)
      << static_cast<char>(2)
      << setw(6) << p.x * 1e-3
      << setw(6) << p.y * 1e-3
      << setw(6) << p.z * 1e-3
      << setw(6) << size
      << color.x
      << color.y
      << color.z
      << setName
      << static_cast<char>(0);
  ASSERT(cmd.str().size() == setName.length()+30);

  drawCommands.push_back(cmd.str());
}


/*
 ----------
|1         | - 1 byte
|----------|
|3         | - 1 byte
|----------|
|Position X| - 6 bytes (float/string)
|----------|
|Position Y| - 6 bytes (float/string)
|----------|
|Position Z| - 6 bytes (float/string)
|----------|
|Radius    | - 6 bytes (float/string)
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
----------
*/
void RoboViz::drawSphere(const Vector3d& pos, float radius, const Vector3<unsigned char>& color, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(1)
      << static_cast<char>(3)
      << setw(6) << pos.x * 1e-3
      << setw(6) << pos.y * 1e-3
      << setw(6) << pos.z * 1e-3
      << setw(6) << radius * 1e-3
      << color.x
      << color.y
      << color.z
      << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|1         | - 1 byte
|----------|
|4         | - 1 byte
|----------|
|NumVerts  | - 1 byte
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Alpha     | - 1 byte
|----------|
|Vertex 0  | - 18 bytes (3 float/string)
|----------|
|   ...    |
|----------|
|Vertex n  | - 18 bytes (3 float/string)
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawPolygon(const list<Vector3d>& vertex, const Vector3<unsigned char>& color, unsigned char alpha, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(1)
      << static_cast<char>(4)
      << static_cast<char>(vertex.size())
      << color.x
      << color.y
      << color.z
      << alpha;
  for(list<Vector3d>::const_iterator iter=vertex.begin(); iter!=vertex.end(); ++iter)
  {
    cmd << setw(6) << iter->x * 1e-3
        << setw(6) << iter->y * 1e-3
        << setw(6) << iter->z * 1e-3;
  }
  cmd << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|2         | - 1 byte
|----------|
|0         | - 1 byte
|----------|
|Position X| - 6 bytes (float/string)
|----------|
|Position Y| - 6 bytes (float/string)
|----------|
|Position Z| - 6 bytes (float/string)
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Text      | - n bytes (string)
|----------|
|0         | - 1 byte
|----------|
|Set Name  | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawAnnotation(const string& text, const Vector3d& pos, const Vector3<unsigned char>& color, const string& setName)
{
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(2)
      << static_cast<char>(0)
      << setw(6) << pos.x * 1e-3
      << setw(6) << pos.y * 1e-3
      << setw(6) << pos.z * 1e-3
      << color.x
      << color.y
      << color.z
      << text
      << static_cast<char>(0)
      << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

int RoboViz::getAgentAnnotationID()
{
  int id = getPlayerInfo().gameData.playerNumber - 1;
  if ( getPlayerInfo().gameData.teamColor != GameData::blue )
  {
    id += 128;
  }
  return id;
}

/*
 ----------
|2         | - 1 byte
|----------|
|1         | - 1 byte
|----------|
|Agent/Team| - 1 byte
|----------|
|Red       | - 1 byte
|----------|
|Green     | - 1 byte
|----------|
|Blue      | - 1 byte
|----------|
|Text      | - n bytes (string)
|----------|
|0         | - 1 byte
 ----------
*/
void RoboViz::drawAgentAnnotation(const string& text, const Vector3<unsigned char>& color)
{
  int id = getAgentAnnotationID();
  stringstream cmd;
  cmd << setprecision(3) << fixed
      << static_cast<char>(2)
      << static_cast<char>(1)
      << static_cast<char>(id)
      << color.x
      << color.y
      << color.z
      << text
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

/*
 ----------
|2         | - 1 byte
|----------|
|2         | - 1 byte
|----------|
|Agent/Team| - 1 byte
 ----------
*/
void RoboViz::cleanAgentAnnotation()
{
  int id = getAgentAnnotationID();
  stringstream cmd;
  cmd << setprecision(2) << fixed
      << static_cast<char>(2)
      << static_cast<char>(2)
      << static_cast<char>(id);

  drawCommands.push_back(cmd.str());
}

void RoboViz::execute()
{
  drawCommands.clear();

  DEBUG_REQUEST("RoboViz:test",
    test();
  );

  drawRobotPose();
  drawMotionRequest();

  if ( getPlayerInfo().isPlayingStriker )
  {
    drawBallAndAttackDir();
  }

  drawTimeToBall();

  swapBuffers(getRobotInfo().bodyNickName);
  theBroadCaster->send(drawCommands);
}

void RoboViz::drawRobotPose()
{
  string name = getRobotInfo().bodyNickName + ".RobotPose";
  Vector3<unsigned char> color = getPlayerInfo().isPlayingStriker ? Vector3<unsigned char>(255,0,0) : Vector3<unsigned char>(0,0,255);
  double thickness = 5;
  drawCircle(getRobotPose().translation, 200, thickness, color, name);

  Pose3D pose;
  pose.rotation = RotationMatrix::getRotationZ(getRobotPose().rotation) * getKinematicChain().theLinks[KinematicChain::Torso].R;
  pose.translation = Vector3d(getRobotPose().translation.x, getRobotPose().translation.y, 1);

  Vector3d forward = pose * Vector3d(300, 0, 0);
  drawLine(pose.translation, forward, thickness, Vector3<unsigned char>(255,0,0), name);

  Vector3d up = pose * Vector3d(0, 0, 500);
  drawLine(pose.translation, up, thickness, Vector3<unsigned char>(0,255,0), name);

  Vector3d left = pose * Vector3d(0, 300, 0);
  drawLine(pose.translation, left, thickness, Vector3<unsigned char>(0,0,255), name);
}

void RoboViz::drawMotionRequest()
{
  string name = getRobotInfo().bodyNickName + ".MotionRequest";
  const MotionRequest& mq = getMotionRequest();
  switch(mq.id)
  {
  case motion::walk:
  {
    Pose2D pose = getRobotPose();
    pose += getMotionStatus().plannedMotion.hip;
    drawSphere(Vector3d(pose.translation.x, pose.translation.y,0),
               100, Vector3<unsigned char>(0,255,255), name+".Walk");
    Vector2d start = pose.translation;
    pose.translate(mq.walkRequest.target.translation.x, mq.walkRequest.target.translation.y);
    drawLine(Vector3d(start.x, start.y, 1),
             Vector3d(pose.translation.x, pose.translation.y, 1),
             3, Vector3<unsigned char>(0,255,255), name+".Walk");
    break;
  }
  default: break;
  }
}

void RoboViz::drawBallAndAttackDir()
{
  Vector2d ballOnField = getRobotPose() * getBallModel().position;
  Vector3<unsigned char> color;
  ColorClasses::colorClassToRGB(getFieldInfo().ballColor, color.x, color.y, color.z);
  Vector3d ball(ballOnField.x, ballOnField.y, getFieldInfo().ballRadius);
  drawSphere(ball, (float)getFieldInfo().ballRadius, color, getRobotInfo().bodyNickName + ".ball");

  //Vector2d ballPerceptOnField = getRobotPose() * getBallPercept().bearingBasedOffsetOnField;
  //ball = Vector3d(ballPerceptOnField.x, ballPerceptOnField.y, getFieldInfo().ballRadius);
  //drawSphere(ball, (float)getFieldInfo().ballRadius, Vector3<unsigned char>(255,0,0), getRobotInfo().bodyNickName + ".ball");

  double dir = getRobotPose().rotation + getSoccerStrategy().attackDirection.angle();
  Vector3d atc(ball.x+cos(dir)*1000, ball.y+sin(dir)*1000, ball.z);
  drawLine(ball, atc, 1, color, getRobotInfo().bodyNickName + ".attackDirection");
}

void RoboViz::drawTimeToBall()
{
  Vector3d b(getRobotPose().translation.x, getRobotPose().translation.y, 600);
  double len = Math::clamp(10000 - getSoccerStrategy().timeToBall, 0.0, 10000.0);
  Vector3d e(b);
  e.z += len/10;
  unsigned int red = Math::clamp(static_cast<int>(len / 10000 * 255), 0, 255);
  drawLine(b, e, 3, Vector3<unsigned char>(red,255-red,0), getRobotInfo().bodyNickName + ".timeToBall");
}

void RoboViz::test()
{
  testStaticShapes();
  testAnimatedShapes();
}

void RoboViz::testStaticShapes()
{
  // draw 3D coordinate axes
  string n1("test.static.axes");
  drawLine(Vector3d(0,0,0), Vector3d(3000,0,0),3, Vector3<unsigned char>(255,0,0),n1);
  drawLine(Vector3d(0,0,0), Vector3d(0,3000,0),3, Vector3<unsigned char>(0,255,0),n1);
  drawLine(Vector3d(0,0,0), Vector3d(0,0,3000),3, Vector3<unsigned char>(0,0,255),n1);

  // draw 1 meter lines on field
  string n2("test.static.lines.field");
  drawLine(Vector3d(-9000,-6000,0), Vector3d(9000,-6000,0),1,Vector3<unsigned char>(153,230,153),n2);
  drawLine(Vector3d(-9000,6000,0), Vector3d(9000,6000,0),1, Vector3<unsigned char>(153,230,153), n2);

  for (int i = 0; i <= 18000; i+=1000)
    drawLine(Vector3d(-9000+i,-6000,0), Vector3d(-9000+i, 6000,0),1, Vector3<unsigned char>(153,230,153),n2);

  // draw some circles
  string n3("test.static.circles");
  drawCircle(Vector2d(-5000,0),3000,2,Vector3<unsigned char>(0,0,255),n3);
  drawCircle(Vector2d(5000,0),3000,2,Vector3<unsigned char>(0,0,255),n3);

  // draw some spheres
  string n4("test.static.spheres");
  drawSphere(Vector3d(-5000,0,2000),500,Vector3<unsigned char>(255,0,128),n4);
  drawSphere(Vector3d(5000,0,2000),500,Vector3<unsigned char>(255,0,128),n4);

  // draw a polygon
  string n5("test.static.polygons");
  list<Vector3d> v;
  v.push_back(Vector3d(0,0,0));
  v.push_back(Vector3d(1000,0,0));
  v.push_back(Vector3d(1000,1000,0));
  v.push_back(Vector3d(0,3000,0));
  v.push_back(Vector3d(-2000,-2000,0));
  drawPolygon(v, Vector3<unsigned char>(255,255,255), 128, n5);

  string staticSets("test.static");
  swapBuffers(staticSets);
}

void RoboViz::testAnimatedShapes()
{
  static double angle = 0;
  angle += 0.05;

  string n1("test.animated.points");
  for (int i = 0; i < 60; i++) {
    double p = i / 60.0;
    double height = max(0.0, sin(angle + p * 18)) * 1000;
    drawPoint(Vector3d(-9000 + 18000 * p, p * 12000 - 6000, height), 5, Vector3<unsigned char>(0,0,0),n1);
  }

  double bx = cos(angle) * 2 * 1000;
  double by = sin(angle) * 2 * 1000;
  double bz = cos(angle) * 1000 + 1500;

  string n2("test.animated.spinner");
  drawLine(Vector3d(0,0,0),Vector3d(bx,by,bz),5,Vector3<unsigned char>(255,255,0),n2);
  drawLine(Vector3d(bx,by,bz),Vector3d(bx,by,0),5,Vector3<unsigned char>(255,255,0),n2);
  drawLine(Vector3d(0,0,0),Vector3d(bx,by,0),5,Vector3<unsigned char>(255,255,0),n2);

  string n3("test.animated.annotation");
  stringstream ss;
  ss<<static_cast<int>(bz);
  drawAnnotation(ss.str(), Vector3d(bx, by, bz), Vector3<unsigned char>(0,255,0), n3);

  string staticSets("test.animated");
  swapBuffers(staticSets);

  drawAgentAnnotation("Hi, I am "+getRobotInfo().bodyNickName, Vector3<unsigned char>(0,255,255));
}
