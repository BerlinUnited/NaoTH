#include "FakeCameraMatrixFinder.h"
#include <Tools/NaoInfo.h>
#include <Tools/CameraGeometry.h>

FakeCameraMatrixFinder::FakeCameraMatrixFinder()
{
    DEBUG_REQUEST_REGISTER("FakeCameraMatrixFinder:project_image_borders",
                           "projects the borders of the image onto the ground",
                           false);
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

    DEBUG_REQUEST("FakeCameraMatrixFinder:project_image_borders",
        // TODO: make it better
        Vector2d p1; Vector2d p2;
        Vector2d r1; Vector2d r2; Vector2d r3; Vector2d r4;

        CameraGeometry::calculateArtificialHorizon(getCameraMatrix(), getCameraInfo(), p1, p2);
        p1.y++;
        p2.y++;

        // clamp horizon to image borders
        p1.x = Math::clamp(p1.x, 0.0, static_cast<double>(getCameraInfo().resolutionWidth));
        p1.y = Math::clamp(p1.y, 0.0, static_cast<double>(getCameraInfo().resolutionHeight));
        p2.x = Math::clamp(p2.x, 0.0, static_cast<double>(getCameraInfo().resolutionWidth));
        p2.y = Math::clamp(p2.y, 0.0, static_cast<double>(getCameraInfo().resolutionHeight));

        // transform horizon and image borders to ground
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), p1, 0, r1);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), p2, 0, r2);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), 0, getCameraInfo().resolutionHeight, 0, r3);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), getCameraInfo().resolutionWidth, getCameraInfo().resolutionHeight, 0, r4);
        FIELD_DRAWING_CONTEXT;
        LINE(r3.x, r3.y, r4.x, r4.y);
        LINE(r4.x, r4.y, r2.x, r2.y);
        LINE(r2.x, r2.y, r1.x, r1.y);
        LINE(r1.x, r1.y, r3.x, r3.y);

        CameraGeometry::calculateArtificialHorizon(getCameraMatrixTop(), getCameraInfoTop(), p1, p2);
        p1.y++;
        p2.y++;

        // clamp horizon to image borders
        p1.x = Math::clamp(p1.x, 0.0, static_cast<double>(getCameraInfoTop().resolutionWidth));
        p1.y = Math::clamp(p1.y, 0.0, static_cast<double>(getCameraInfoTop().resolutionHeight));
        p2.x = Math::clamp(p2.x, 0.0, static_cast<double>(getCameraInfoTop().resolutionWidth));
        p2.y = Math::clamp(p2.y, 0.0, static_cast<double>(getCameraInfoTop().resolutionHeight));

        // transform horizon and image borders to ground
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), p1, 0, r1);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), p2, 0, r2);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), 0, getCameraInfoTop().resolutionHeight, 0, r3);
        CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), getCameraInfoTop().resolutionWidth, getCameraInfoTop().resolutionHeight, 0, r4);
        FIELD_DRAWING_CONTEXT;
        LINE(r3.x, r3.y, r4.x, r4.y);
        LINE(r4.x, r4.y, r2.x, r2.y);
        LINE(r2.x, r2.y, r1.x, r1.y);
        LINE(r1.x, r1.y, r3.x, r3.y);
    );
}


FakeCameraMatrixFinder::~FakeCameraMatrixFinder()
{

}

