/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include <sstream>

#include "Tools/Math/Pose3D.h"

#include "DebugDrawings3D.h"
#include "Core/Cognition/Modules/Infrastructure/Debug/Debug.h"
#include "Core/Cognition/CognitionDebugServer.h"

DebugDrawings3D::DebugDrawings3D()
{
  activeCount = 0;
//  pthread_mutex_init(&theMutex, NULL);
  
  REGISTER_DEBUG_COMMAND("debug_3d",
    "return the debug 3d drawings which where collected in the internal buffer", this);
}

DebugDrawings3D::~DebugDrawings3D()
{
//  pthread_mutex_destroy(&theMutex);
}

void DebugDrawings3D::executeDebugCommand(
  const std::string& command, const std::map<std::string, std::string>& /*arguments*/,
  std::stringstream &outstream)
{
  if ("debug_3d" == command) {

//    pthread_mutex_lock(&theMutex);
    const std::string& buf = outStream.str();
    if ("" != buf) {
      outstream << buf;
    }
    outStream.str("");
    outStream.clear();
//    pthread_mutex_unlock(&theMutex);

    activeCount = 5;
  }
}

void DebugDrawings3D::update()
{
//  pthread_mutex_lock(&theMutex);
  outStream.str("");
  outStream.clear();
//  pthread_mutex_unlock(&theMutex);
  if ( activeCount > 0 ) activeCount--;
}

void DebugDrawings3D::addEntity(const std::string& name, const Pose3D& pose)
{
  addEntity(name, pose.rotation, pose.translation);
}

void DebugDrawings3D::addEntity(const std::string& name, const RotationMatrix& R, const Vector3<double>& p)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Entity " << name << " ";
  R.toCompactString(outStream);
  outStream << " " << p << "\n";
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const Pose3D& pose)
{
  addBox(ColorClasses::colorClassToHex(color), xdim, ydim, zdim, pose);
}

void DebugDrawings3D::addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3<double>& p)
{
  addBox(ColorClasses::colorClassToHex(color), xdim, ydim, zdim, R, p);
}

void DebugDrawings3D::addBox(const char* color, double xdim, double ydim, double zdim, const Pose3D& pose)
{
  addBox(color, xdim, ydim, zdim, pose.rotation, pose.translation);
}

void DebugDrawings3D::addBox(const char* color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3<double>& p)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Box " << color << ' ' << xdim << ' ' << ydim << ' ' << zdim << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addSphere(ColorClasses::Color color, double radius, const Vector3<double>& p)
{
  addSphere(ColorClasses::colorClassToHex(color), radius, p);
}

void DebugDrawings3D::addSphere(ColorClasses::Color color, double radius, double x, double y, double z)
{
  addSphere(ColorClasses::colorClassToHex(color), radius, x, y, z);
}

void DebugDrawings3D::addSphere(const char* color, double radius, const Vector3<double>& p)
{
  addSphere(color, radius, p.x, p.y, p.z);
}

void DebugDrawings3D::addSphere(const char* color, double radius, double x, double y, double z)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Sphere " << color << ' ' << radius << ' ' << x << ' ' << y << ' ' << z << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addCone(ColorClasses::Color color, double radius, double height, const Pose3D& pose)
{
  addCone(ColorClasses::colorClassToHex(color), radius, height, pose);
}

void DebugDrawings3D::addCone(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p)
{
  addCone(ColorClasses::colorClassToHex(color), radius, height, R, p);
}

void DebugDrawings3D::addCone(const char* color, double radius, double height, const Pose3D& pose)
{
  addCone(color, radius, height, pose.rotation, pose.translation);
}

void DebugDrawings3D::addCone(const char* color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Cone " << color << ' ' << radius << ' ' << height << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addCylinder(ColorClasses::Color color, double radius, double height, const Pose3D& pose)
{
  addCylinder(ColorClasses::colorClassToHex(color), radius, height, pose);
}

void DebugDrawings3D::addCylinder(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p)
{
  addCylinder(ColorClasses::colorClassToHex(color), radius, height, R, p);
}

void DebugDrawings3D::addCylinder(const char* color, double radius, double height, const Pose3D& pose)
{
  addCylinder(color, radius, height, pose.rotation, pose.translation);
}

void DebugDrawings3D::addCylinder(const char* color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Cylinder " << color << ' ' << radius << ' ' << height << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addLine(ColorClasses::Color color, const Vector3<double>& p0, const Vector3<double>& p1)
{
  addLine(ColorClasses::colorClassToHex(color), p0, p1);
}

void DebugDrawings3D::addLine(const char* color, const Vector3<double>& p0, const Vector3<double>& p1)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Line " << color << ' ' << p0 << ' ' << p1 << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addText3D(ColorClasses::Color color, const string& text, const Vector3<double>& p)
{
  addText3D(ColorClasses::colorClassToHex(color), text, p);
}

void DebugDrawings3D::addText3D(const char* color, const string& text, const Vector3<double>& p)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Text3D " << color << ' ' << text << ' ' << p << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addCamera(const Pose3D& cm, double focusLength, unsigned int width, unsigned height)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Camera " << focusLength << ' ' << width << ' ' << height << ' ';
  cm.toCompactString(outStream);
  outStream << '\n';
//  pthread_mutex_unlock(&theMutex);
}

void DebugDrawings3D::addCamera(const Pose3D& rp, const Pose3D& cm, double focusLength, unsigned int width, unsigned height)
{
//  pthread_mutex_lock(&theMutex);
  outStream << "Camera " << focusLength << ' ' << width << ' ' << height << ' ';
  cm.toCompactString(outStream);
  outStream << " Robot ";
  rp.toCompactString(outStream);
  outStream << '\n';
//  pthread_mutex_unlock(&theMutex);
}
