#ifndef CALIBRATIONBEHAVIORCONTROL_H
#define CALIBRATIONBEHAVIORCONTROL_H

#include <ModuleFramework/Module.h>
#include <Representations/Motion/Request/MotionRequest.h>
#include <Representations/Motion/Request/HeadMotionRequest.h>
#include <Representations/Infrastructure/LEDRequest.h>
#include <Representations/Motion/MotionStatus.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/ButtonData.h>

BEGIN_DECLARE_MODULE(CalibrationBehaviorControl)

REQUIRE(MotionStatus)
REQUIRE(FrameInfo)
REQUIRE(ButtonData)

PROVIDE(MotionRequest)
PROVIDE(BehaviorLEDRequest)
PROVIDE(GameControllerLEDRequest)
PROVIDE(HeadMotionRequest)

END_DECLARE_MODULE(CalibrationBehaviorControl)

class CalibrationBehaviorControl : public CalibrationBehaviorControlBase
{


  public:
  CalibrationBehaviorControl();

  virtual void execute();
  private:

  void setLED(unsigned int step);
  void setChest(double red, double green, double blue);
  void calibrateInertial();
  void calibrateFSRTouch();

  FrameInfo stateChanged;
  int lastChestButtonEventCounter;

  enum CalibrationState
  {
    wait,
    inertial,
    fsrTouch,
    numOfCalibrationState
  } state;
};

#endif // CALIBRATIONBEHAVIORCONTROL_H
