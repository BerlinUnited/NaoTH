/**
 * @file PathSymbols.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef _PathPlannerSymbols_H_
#define _PathPlannerSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/PathModel.h"

#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(PathSymbols)
  PROVIDE(PathModel)
END_DECLARE_MODULE(PathSymbols)

class PathSymbols: public PathSymbolsBase
{
public:

  PathSymbols() {
    theInstance = this;
  }
  ~PathSymbols() {}

  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:
  static PathSymbols* theInstance;

  static void setPathRoutine(int id);
  static int getPathRoutine();

  static void setPathRoutine2018(int id);
  static int getPathRoutine2018();

  static void setDirection(double rot);
  static double getDirection();
};

#endif /* _PathSymbols_H_ */
