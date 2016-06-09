#ifndef FAKECAMERAMATRIXFINDER_H
#define FAKECAMERAMATRIXFINDER_H

#include <ModuleFramework/Module.h>
#include <Representations/Perception/CameraMatrix.h>

#include <Tools/Debug/DebugModify.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(FakeCameraMatrixFinder)
  PROVIDE(DebugModify)

  PROVIDE(CameraMatrix)
  PROVIDE(CameraMatrixTop)
END_DECLARE_MODULE(FakeCameraMatrixFinder)


/**
 * @brief Get the nearest camera matrix for the images from the buffer and
 *        sets them on the global blackboard.
 */
class FakeCameraMatrixFinder : public FakeCameraMatrixFinderBase
{
public:
  FakeCameraMatrixFinder();
  virtual ~FakeCameraMatrixFinder();

  virtual void execute();
};

#endif // FAKECAMERAMATRIXFINDER_H
