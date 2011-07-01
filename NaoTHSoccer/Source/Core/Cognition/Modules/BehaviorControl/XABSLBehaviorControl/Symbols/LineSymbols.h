/**
* @file LineSymbols.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class LineSymbols
*/

#ifndef __LineSymbols_H_
#define __LineSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Perception/LinePercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

BEGIN_DECLARE_MODULE(LineSymbols)
  REQUIRE(LinePercept)
  REQUIRE(FrameInfo)
END_DECLARE_MODULE(LineSymbols)

class LineSymbols: public LineSymbolsBase
{

public:
  LineSymbols()
  :
  linePercept(getLinePercept()),
  frameInfo(getFrameInfo())
  {
    theInstance = this;
  };
  ~LineSymbols(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static LineSymbols* theInstance;

  static double getLineTimeSinceLastSeen();
  static double getEstDistanceToOrthPoint();
  static double getAngleToEstOrthPoint();
  static double getDistanceToClosestPoint();
  static double getAngleToClosestPoint();

  // representations
  const LinePercept& linePercept;
  const naoth::FrameInfo& frameInfo;

};//end class LineSymbols

#endif // __LineSymbols_H_
