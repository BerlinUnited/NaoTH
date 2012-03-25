/**
* @file OdometrySymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class OdometrySymbols
*/

#ifndef __OdometrySymbols_H_
#define __OdometrySymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(OdometrySymbols)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(OdometrySymbols)

class OdometrySymbols: public OdometrySymbolsBase
{

public:
  OdometrySymbols()
    :
    odometryData(getOdometryData()),
    motionStatus(getMotionStatus())
  {
    theInstance = this;
  };
  virtual ~OdometrySymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static OdometrySymbols* theInstance;

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

#endif // __OdometrySymbols_H_
