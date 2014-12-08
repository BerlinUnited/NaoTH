/**
 * @author <a href="xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */



#include "DebugDrawings3D.h"
#include <DebugCommunication/DebugCommandManager.h>

using namespace std;

DebugDrawings3D::DebugDrawings3D()
{
//  REGISTER_DEBUG_COMMAND("debug_3d",
//    "return the debug 3d drawings which where collected in the internal buffer", this);
}

DebugDrawings3D::~DebugDrawings3D()
{
}

void naoth::Serializer<DebugDrawings3D>::serialize(const DebugDrawings3D& object, std::ostream& stream)
{
  const std::string& buf = object.getOutStream().str();
  if ("" != buf) {
    stream << buf;
  }
}

void naoth::Serializer<DebugDrawings3D>::deserialize(std::istream& /*stream*/, DebugDrawings3D& /*object*/)
{
}

void DebugDrawings3D::reset()
{
  outStream.str("");
  outStream.clear();
}

void DebugDrawings3D::addEntity(const std::string& name, const Pose3D& pose)
{
  addEntity(name, pose.rotation, pose.translation);
}

void DebugDrawings3D::addEntity(const std::string& name, const RotationMatrix& R, const Vector3d& p)
{
  outStream << "Entity " << name << " ";
  R.toCompactString(outStream);
  outStream << " " << p << "\n";
}

void DebugDrawings3D::addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const Pose3D& pose)
{
  addBox(ColorClasses::colorClassToHex(color), xdim, ydim, zdim, pose);
}

void DebugDrawings3D::addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3d& p)
{
  addBox(ColorClasses::colorClassToHex(color), xdim, ydim, zdim, R, p);
}

void DebugDrawings3D::addBox(const char* color, double xdim, double ydim, double zdim, const Pose3D& pose)
{
  addBox(color, xdim, ydim, zdim, pose.rotation, pose.translation);
}

void DebugDrawings3D::addBox(const char* color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3d& p)
{
  outStream << "Box " << color << ' ' << xdim << ' ' << ydim << ' ' << zdim << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
}

void DebugDrawings3D::addSphere(ColorClasses::Color color, double radius, const Vector3d& p)
{
  addSphere(ColorClasses::colorClassToHex(color), radius, p);
}

void DebugDrawings3D::addSphere(ColorClasses::Color color, double radius, double x, double y, double z)
{
  addSphere(ColorClasses::colorClassToHex(color), radius, x, y, z);
}

void DebugDrawings3D::addSphere(const char* color, double radius, const Vector3d& p)
{
  addSphere(color, radius, p.x, p.y, p.z);
}

void DebugDrawings3D::addSphere(const char* color, double radius, double x, double y, double z)
{
  outStream << "Sphere " << color << ' ' << radius << ' ' << x << ' ' << y << ' ' << z << '\n';
}

void DebugDrawings3D::addCone(ColorClasses::Color color, double radius, double height, const Pose3D& pose)
{
  addCone(ColorClasses::colorClassToHex(color), radius, height, pose);
}

void DebugDrawings3D::addCone(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3d& p)
{
  addCone(ColorClasses::colorClassToHex(color), radius, height, R, p);
}

void DebugDrawings3D::addCone(const char* color, double radius, double height, const Pose3D& pose)
{
  addCone(color, radius, height, pose.rotation, pose.translation);
}

void DebugDrawings3D::addCone(const char* color, double radius, double height, const RotationMatrix&R, const Vector3d& p)
{
  outStream << "Cone " << color << ' ' << radius << ' ' << height << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
}

void DebugDrawings3D::addCylinder(ColorClasses::Color color, double radius, double height, const Pose3D& pose)
{
  addCylinder(ColorClasses::colorClassToHex(color), radius, height, pose);
}

void DebugDrawings3D::addCylinder(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3d& p)
{
  addCylinder(ColorClasses::colorClassToHex(color), radius, height, R, p);
}

void DebugDrawings3D::addCylinder(const char* color, double radius, double height, const Pose3D& pose)
{
  addCylinder(color, radius, height, pose.rotation, pose.translation);
}

void DebugDrawings3D::addCylinder(const char* color, double radius, double height, const RotationMatrix&R, const Vector3d& p)
{
  outStream << "Cylinder " << color << ' ' << radius << ' ' << height << ' ';
  R.toCompactString(outStream);
  outStream << ' ' << p << '\n';
}

void DebugDrawings3D::addLine(ColorClasses::Color color, const Vector3d& p0, const Vector3d& p1)
{
  addLine(ColorClasses::colorClassToHex(color), p0, p1);
}

void DebugDrawings3D::addLine(const char* color, const Vector3d& p0, const Vector3d& p1)
{
  outStream << "Line " << color << ' ' << p0 << ' ' << p1 << '\n';
}

void DebugDrawings3D::addText3D(ColorClasses::Color color, const string& text, const Vector3d& p)
{
  addText3D(ColorClasses::colorClassToHex(color), text, p);
}

void DebugDrawings3D::addText3D(const char* color, const string& text, const Vector3d& p)
{
  outStream << "Text3D " << color << ' ' << text << ' ' << p << '\n';
}

void DebugDrawings3D::addCamera(const std::string& id, const Pose3D& cm, double focusLength, unsigned int width, unsigned height)
{
  outStream << "Camera " << id << ' ' << focusLength << ' ' << width << ' ' << height << ' ';
  cm.toCompactString(outStream);
  outStream << '\n';
}

void DebugDrawings3D::addCamera(const std::string& id, const Pose3D& rp, const Pose3D& cm, double focusLength, unsigned int width, unsigned height)
{
  outStream << "Camera " << id << ' ' << focusLength << ' ' << width << ' ' << height << ' ';
  cm.toCompactString(outStream);
  outStream << " Robot ";
  rp.toCompactString(outStream);
  outStream << '\n';
}
