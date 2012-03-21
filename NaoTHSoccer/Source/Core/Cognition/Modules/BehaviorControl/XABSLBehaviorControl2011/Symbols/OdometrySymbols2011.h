/**
* @file OdometrySymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class OdometrySymbols
*/

#ifndef __OdometrySymbols2011_H_
#define __OdometrySymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(OdometrySymbols2011)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(OdometrySymbols2011)

class OdometrySymbols2011: public OdometrySymbols2011Base
{

public:
  OdometrySymbols2011()
    :
    odometryData(getOdometryData()),
    motionStatus(getMotionStatus())
  {
    theInstance = this;
  };
  ~OdometrySymbols2011(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static OdometrySymbols2011* theInstance;

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
};//end class OdometrySymbols

#endif // __OdometrySymbols2011_H_
