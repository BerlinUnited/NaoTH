/**
 * @file NaoMotionController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAO_MOTION_CONTROLLER_H
#define	_NAO_MOTION_CONTROLLER_H

#include "DCMHandler.h"
#include "Tools/NaoControllerBase.h"
#include <Tools/DataStructures/Singleton.h>

namespace naoth
{
class NaoMotionController : public NaoControllerBase<NaoMotionController>, public Singleton<NaoMotionController>
{
protected:
  friend class Singleton<NaoMotionController>;
  NaoMotionController();
  virtual ~NaoMotionController();
  
public:

  template<typename T>
  void get(T& data)
  {
    NaoControllerBase<NaoMotionController>::get(data);
  }

  /////////////////////// init ///////////////////////
  void init(ALPtr<ALBroker> pB);

  /////////////////////// run ///////////////////////
  void updateSensorData();
  void setActuatorData();

public:

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

private:
  DCMHandler theDCMHandler;
  
  SharedMemory<LibNaothData> libNaothData;
  SharedMemory<NaothData> naothData;
};

} // end namespace naoth
#endif	/* _NAO_MOTION_CONTROLLER_H */

