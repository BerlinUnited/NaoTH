#include "FakeCameraMatrixFinder.h"

FakeCameraMatrixFinder::FakeCameraMatrixFinder()
{
}

void FakeCameraMatrixFinder::execute()
{
    double yaw = 0;
    double pitch = 0;
    double roll = 0;

    double x = 0;
    double y = 0;
    double z = 0;

    MODIFY("FakeCameraMatrixFinder:Bottom:rot:yaw",yaw);
    MODIFY("FakeCameraMatrixFinder:Bottom:rot:pitch",pitch);
    MODIFY("FakeCameraMatrixFinder:Bottom:rot:roll",roll);

    MODIFY("FakeCameraMatrixFinder:Bottom:trans:x",x);
    MODIFY("FakeCameraMatrixFinder:Bottom:trans:y",y);
    MODIFY("FakeCameraMatrixFinder:Bottom:trans:z",z);

    getCameraMatrix().rotation    = RotationMatrix(yaw, pitch, roll);
    getCameraMatrix().translation = Vector3d(x, y, z);

    yaw = pitch = roll = x = y = z = 0;

    MODIFY("FakeCameraMatrixFinder:Top:rot:yaw",yaw);
    MODIFY("FakeCameraMatrixFinder:Top:rot:pitch",pitch);
    MODIFY("FakeCameraMatrixFinder:Top:rot:roll",roll);

    MODIFY("FakeCameraMatrixFinder:Top:trans:x",x);
    MODIFY("FakeCameraMatrixFinder:Top:trans:y",y);
    MODIFY("FakeCameraMatrixFinder:Top:trans:z",z);

    getCameraMatrixTop().rotation    = RotationMatrix(yaw, pitch, roll);
    getCameraMatrixTop().translation = Vector3d(x, y, z);

    getCameraMatrix().valid = true;
    getCameraMatrixTop().valid = true;
}


FakeCameraMatrixFinder::~FakeCameraMatrixFinder()
{

}

