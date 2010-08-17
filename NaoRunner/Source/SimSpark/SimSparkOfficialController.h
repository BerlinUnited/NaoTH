/**
 * @file SimSparkOfficialController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief for running in the simulation league: with official simualtor
 *
 */

#ifndef _SIMSPARKOFFICIALCONTROLLER_H
#define	_SIMSPARKOFFICIALCONTROLLER_H

#include "SimSparkController.h"

class SimSparkOfficialController : public SimSparkController
{
public:
  virtual string getHardwareIdentity() const { return "simspark_official"; }

protected:
  virtual void get(Image& data);

private:
  void updateInertialSensorByVision();
  Vector3d calculateCameraPosFromVision(const map<string, Vector3d>& flagVision) const;
  Vector3d calculateCameraPosFromThreeFlag(map<string, Vector3d>::const_iterator fa, map<string, Vector3d>::const_iterator fb, map<string, Vector3d>::const_iterator fc) const;
  RotationMatrix calculateCameraRotFromVision(const Vector3d& cameraPos, const map<string, Vector3d>& flagVision) const;
  RotationMatrix calculateCameraRotFromThreeFlag(const Vector3d& pos, map<string, Vector3d>::const_iterator fa, map<string, Vector3d>::const_iterator fb, map<string, Vector3d>::const_iterator fc) const;

  bool setupFlagGlobalPositions();
  map<string, Vector3d> flagGlobalPositions;
};

#endif	/* _SIMSPARKOFFICIALCONTROLLER_H */

