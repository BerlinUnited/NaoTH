#include "FakeCameraMatrixFinder.h"

FakeCameraMatrixFinder::FakeCameraMatrixFinder()
{
}

void FakeCameraMatrixFinder::execute()
{
    double yaw = 0; // around z
    double pitch = 0; // around y
    double roll = 0; // around x

    double x = 0;
    double y = 0;
    double z = 430;

    MODIFY("FakeCameraMatrixFinder:head:rot:yaw",yaw);
    MODIFY("FakeCameraMatrixFinder:head:rot:pitch",pitch);
    MODIFY("FakeCameraMatrixFinder:head:rot:roll",roll);

    MODIFY("FakeCameraMatrixFinder:head:trans:x",x);
    MODIFY("FakeCameraMatrixFinder:head:trans:y",y);
    MODIFY("FakeCameraMatrixFinder:head:trans:z",z);

    Pose3D head;
    head.rotation = RotationMatrix::getRotationZ(Math::pi / 180 * yaw)
                    * RotationMatrix::getRotationY(Math::pi / 180 * pitch)
                    * RotationMatrix::getRotationX(Math::pi / 180 * roll);
    head.translation = Vector3d(x, y, z);

    getCameraMatrix() = head;
    getCameraMatrix().translate(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].offset);
    getCameraMatrix().rotateY(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].rotationY);

    getCameraMatrixTop() = head;
    getCameraMatrixTop().translate(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].offset);
    getCameraMatrixTop().rotateY(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].rotationY);

    getCameraMatrix().valid = true;
    getCameraMatrixTop().valid = true;
}


FakeCameraMatrixFinder::~FakeCameraMatrixFinder()
{

}

