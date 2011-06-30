/**
 * @file RoboViz.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#include "RoboViz.h"
#include "rvdraw.h"
#include <iomanip>

/*



void drawPoint(float x, float y, float z, float size, float r, float g, float b, const string* setName) {
  float center[3] = {x,y,z};
  float color[3] = {r,g,b};

  int bufSize = -1;
  unsigned char* buf = newPoint(center, size, color, setName, &bufSize);
  sendto(sockfd, buf, bufSize, 0, p->ai_addr, p->ai_addrlen);
  delete[] buf;
}

void drawPolygon(const float* v, int numVerts, float r, float g, float b,
    float a, const string* setName) {
  float color[4] = {r,g,b,a};

  int bufSize = -1;
  unsigned char* buf = newPolygon(v, numVerts, color, setName, &bufSize);
  sendto(sockfd, buf, bufSize, 0, p->ai_addr, p->ai_addrlen);
  delete[] buf;
}

void drawAnnotation(const string* text, float x, float y, float z, float r,
    float g, float b, const string* setName) {
  float color[3] = {r,g,b};
  float pos[3] = {x,y,z};

  int bufSize = -1;
  unsigned char* buf = newAnnotation(text, pos, color, setName, &bufSize);
  sendto(sockfd, buf, bufSize, 0, p->ai_addr, p->ai_addrlen);
  delete[] buf;
}
*/

using namespace std;

RoboViz::RoboViz()
  :theBroadCaster("wlan0",32769)
{
  renderStaticShapes();
  theBroadCaster.send(drawCommands);
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
  cmd << static_cast<char>(1)
      << static_cast<char>(0)
      << setw(6) << pos.x
      << setw(6) << pos.y
      << setw(6) << radius
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
  cmd << static_cast<char>(1)
      << static_cast<char>(1)
      << setw(6) << start.x
      << setw(6) << start.y
      << setw(6) << start.z
      << setw(6) << end.x
      << setw(6) << end.y
      << setw(6) << end.z
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
  cmd << static_cast<char>(1)
      << static_cast<char>(3)
      << setw(6) << pos.x
      << setw(6) << pos.y
      << setw(6) << pos.z
      << setw(6) << radius
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
  cmd << static_cast<char>(1)
      << static_cast<char>(4)
      << static_cast<char>(vertex.size())
      << color.x
      << color.y
      << color.z
      << alpha;
  for(list<Vector3d>::const_iterator iter=vertex.begin(); iter!=vertex.end(); ++iter)
  {
    cmd << setw(6) << iter->x
        << setw(6) << iter->y
        << setw(6) << iter->z;
  }
  cmd << setName
      << static_cast<char>(0);

  drawCommands.push_back(cmd.str());
}

void RoboViz::execute()
{
  drawCommands.clear();

  test();

  theBroadCaster.send(drawCommands);
}

void RoboViz::test()
{
  renderAnimatedShapes();
}

void RoboViz::renderStaticShapes()
{
  // draw 3D coordinate axes
  string n1("static.axes");
  drawLine(Vector3d(0,0,0), Vector3d(3,0,0),3, Vector3<unsigned char>(255,0,0),n1);
  drawLine(Vector3d(0,0,0), Vector3d(0,3,0),3, Vector3<unsigned char>(0,255,0),n1);
  drawLine(Vector3d(0,0,0), Vector3d(0,0,3),3, Vector3<unsigned char>(0,0,255),n1);

  // draw 1 meter lines on field
  string n2("static.lines.field");
  drawLine(Vector3d(-9,-6,0), Vector3d(9,-6,0),1,Vector3<unsigned char>(153,230,153),n2);
  drawLine(Vector3d(-9,6,0), Vector3d(9,6,0),1, Vector3<unsigned char>(153,230,153), n2);

  for (int i = 0; i <= 18; i++)
    drawLine(Vector3d(-9+i,-6,0), Vector3d(-9+i, 6,0),1, Vector3<unsigned char>(153,230,153),n2);

  // draw some circles
  string n3("static.circles");
  drawCircle(Vector2d(-5,0),3,2,Vector3<unsigned char>(0,0,255),n3);
  drawCircle(Vector2d(5,0),3,2,Vector3<unsigned char>(0,0,255),n3);

  // draw some spheres
  string n4("static.spheres");
  drawSphere(Vector3d(-5,0,2),0.5,Vector3<unsigned char>(255,0,128),n4);
  drawSphere(Vector3d(5,0,2),0.5,Vector3<unsigned char>(255,0,128),n4);

  // draw a polygon
  string n5("static.polygons");
  list<Vector3d> v;
  v.push_back(Vector3d(0,0,0));
  v.push_back(Vector3d(1,0,0));
  v.push_back(Vector3d(1,1,0));
  v.push_back(Vector3d(0,3,0));
  v.push_back(Vector3d(-2,-2,0));
  drawPolygon(v, Vector3<unsigned char>(255,255,255), 128, n5);

  string staticSets("static");
  swapBuffers(staticSets);
}

void RoboViz::renderAnimatedShapes()
{

}
