/**
* @file OdometrySymbols2012.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class OdometrySymbols2012
*/

#ifndef __OdometrySymbols2012_H_
#define __OdometrySymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(OdometrySymbols2012)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(OdometrySymbols2012)

class OdometrySymbols2012: public OdometrySymbols2012Base
{

public:
  OdometrySymbols2012()
    :
    odometryData(getOdometryData()),
    motionStatus(getMotionStatus())
  {
    theInstance = this;
  };
  virtual ~OdometrySymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static OdometrySymbols2012* theInstance;

  // representations
  OdometryData const& odometryData;
  MotionStatus const& motionStatus;


  // seter and getter
  static double getOdometry_x();
  static double getOdometry_y();
  static double getOdometry_angle();

  Vector2<double> relativePoint;
  OdometryData lastRobotOdometry;

  Vector2<double> previewParameter;
  static double getOdometryPreview_x();
  static double getOdometryPreview_y();
};//end class OdometrySymbols2012

#endif // __OdometrySymbols2012_H_
