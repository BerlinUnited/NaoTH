/**
 * @file PathSymbols.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef PATH_PLANNER_SYMBOLS_H
#define PATH_PLANNER_SYMBOLS_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/PathRequest.h"
#include "Representations/Modeling/PathStatus.h"

#include "Tools/Math/Common.h"

BEGIN_DECLARE_MODULE(PathSymbols)
  REQUIRE(PathStatus)
  PROVIDE(PathRequest)
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

#endif /* PATH_PLANNER_SYMBOLS_H */
