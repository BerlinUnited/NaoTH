/**
 * @file RoboViz.cpp
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#include "RoboViz.h"
#include "rvdraw.h"

/*

void drawCircle(float x, float y, float radius, float thickness, float r, float g, float b, const string* setName) {
  float center[2] = {x,y};
  float color[3] = {r,g,b};

  int bufSize = -1;
  unsigned char* buf = newCircle(center, 2.0f, 2.5f, color, setName, &bufSize);
  sendto(sockfd, buf, bufSize, 0, p->ai_addr, p->ai_addrlen);
  delete[] buf;
}

void drawSphere(float x, float y, float z, float radius, float r, float g, float b, const string* setName) {
  float center[3] = {x,y,z};
  float color[3] = {r,g,b};

  int bufSize = -1;
  unsigned char* buf = newSphere(center, radius, color, setName, &bufSize);
  sendto(sockfd, buf, bufSize, 0, p->ai_addr, p->ai_addrlen);
  delete[] buf;
}

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
}

void RoboViz::swapBuffers(const string* setName) {
  int bufSize = -1;
  unsigned char* buf = newBufferSwap(setName, &bufSize);
  drawCmd.append(reinterpret_cast<const char*>(buf), bufSize);
  delete[] buf;
  theBroadCaster.send(drawCmd);
  drawCmd.clear();
}

void RoboViz::drawLine(float x1, float y1, float z1, float x2, float y2, float z2, float thickness, float r, float g, float b,
    const string* setName) {
  float pa[3] = {x1,y1,z1};
  float pb[3] = {x2,y2,z2};
  float color[3] = {r,g,b};

  int bufSize = -1;
  unsigned char* buf = newLine(pa, pb, thickness, color, setName, &bufSize);
  drawCmd.append(reinterpret_cast<const char*>(buf), bufSize);
  delete[] buf;
}

void RoboViz::execute()
{
  test();
}

void RoboViz::test()
{
  renderAnimatedShapes();
}

void RoboViz::renderStaticShapes()
{
  // draw 3D coordinate axes
  string n1("static.axes");
  drawLine(0,0,0,3,0,0,3,1,0,0,&n1);
  drawLine(0,0,0,0,3,0,3,0,1,0,&n1);
  drawLine(0,0,0,0,0,3,3,0,0,1,&n1);

  // draw 1 meter lines on field
  /*string n2("static.lines.field");
  drawLine(-9,-6,0,9,-6,0,1,0.6f,0.9f,0.6f,&n2);
  drawLine(-9,6,0,9,6,0,1,0.6f,0.9f,0.6f,&n2);
  for (int i = 0; i <= 18; i++)
    drawLine(-9+i,-6,0,-9+i,6,0,1,0.6f,0.9f,0.6f,&n2);

  // draw some circles
  string n3("static.circles");
  drawCircle(-5,0,3,2,0,0,1,&n3);
  drawCircle(5,0,3,2,0,0,1,&n3);

  // draw some spheres
  string n4("static.spheres");
  drawSphere(-5,0,2,0.5f,1,0,0.5f,&n4);
  drawSphere(5,0,2,0.5f,1,0,0.5f,&n4);

  // draw a polygon
  string n5("static.polygons");
  float v[] = {0,0,0, 1,0,0, 1,1,0, 0,3,0, -2,-2,0};
  drawPolygon(v, 4, 1, 1, 1, 0.5f, &n5);
*/
  string staticSets("static");
  swapBuffers(&staticSets);
}

void RoboViz::renderAnimatedShapes()
{

}
