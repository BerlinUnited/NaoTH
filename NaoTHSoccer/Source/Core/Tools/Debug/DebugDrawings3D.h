/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _DEBUGDRAWINGS3D_H
#define	_DEBUGDRAWINGS3D_H

#include "Tools/DataStructures/Singleton.h"
#include <Tools/ColorClasses.h>
#include "Tools/Math/Pose3D.h"
#include <DebugCommunication/DebugCommandExecutor.h>

//#include "pthread.h"

//#include <pthread.h>

using namespace naoth;

class DebugDrawings3D : public Singleton<DebugDrawings3D>, public DebugCommandExecutor
{
  friend class Singleton<DebugDrawings3D>;
protected:
  DebugDrawings3D();
  ~DebugDrawings3D();
public:
  bool isActive() const { return activeCount > 0; }

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string, std::string>& arguments,
    std::ostream &outstream);

  void update();

  void addEntity(const std::string& name, const Pose3D& pose);
  void addEntity(const std::string& name, const RotationMatrix& R, const Vector3<double>& p);

  void addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const Pose3D& pose);
  void addBox(ColorClasses::Color color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3<double>& p);
  void addBox(const char* color, double xdim, double ydim, double zdim, const Pose3D& pose);
  void addBox(const char* color, double xdim, double ydim, double zdim, const RotationMatrix&R, const Vector3<double>& p);

  void addSphere(ColorClasses::Color color, double radius, const Vector3<double>& p);
  void addSphere(ColorClasses::Color color, double radius, double x, double y, double z);
  void addSphere(const char* color, double radius, const Vector3<double>& p);
  void addSphere(const char* color, double radius, double x, double y, double z);

  void addCone(ColorClasses::Color color, double radius, double height, const Pose3D& pose);
  void addCone(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p);
  void addCone(const char* color, double radius, double height, const Pose3D& pose);
  void addCone(const char* color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p);

  void addCylinder(ColorClasses::Color color, double radius, double height, const Pose3D& pose);
  void addCylinder(ColorClasses::Color color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p);
  void addCylinder(const char* color, double radius, double height, const Pose3D& pose);
  void addCylinder(const char* color, double radius, double height, const RotationMatrix&R, const Vector3<double>& p);

  void addLine(ColorClasses::Color color, const Vector3<double>& p0, const Vector3<double>& p1);
  void addLine(const char* color, const Vector3<double>& p0, const Vector3<double>& p1);

  void addText3D(ColorClasses::Color color, const std::string& text, const Vector3<double>& p);
  void addText3D(const char* color, const std::string& text, const Vector3<double>& p);

  void addCamera(const Pose3D& cm, double focusLength, unsigned int width, unsigned height);
  void addCamera(const Pose3D& rp, const Pose3D& cm, double focusLength, unsigned int width, unsigned height);
private:
  std::stringstream outStream;

  int activeCount;

  //pthread_mutex_t theMutex;
};

#ifdef DEBUG
#define ENTITY DebugDrawings3D::getInstance().addEntity
#define BOX_3D DebugDrawings3D::getInstance().addBox
#define SPHERE DebugDrawings3D::getInstance().addSphere
#define CONE DebugDrawings3D::getInstance().addCone
#define CYLINDER DebugDrawings3D::getInstance().addCylinder
#define LINE_3D DebugDrawings3D::getInstance().addLine
#define TEXT_3D DebugDrawings3D::getInstance().addText3D
#else //DEBUG
/* ((void)0) - that's a do-nothing statement */
#define ENTITY(...) ((void)0)
#define BOX_3D(...) ((void)0)
#define SPHERE(...) ((void)0)
#define CONE(...) ((void)0)
#define CYLINDER(...) ((void)0)
#define LINE_3D(...) ((void)0)
#define TEXT_3D(...) ((void)0)
#endif //DEBUG
#endif	/* _DEBUGDRAWINGS3D_H */

