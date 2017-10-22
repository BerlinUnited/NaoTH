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

BEGIN_DECLARE_MODULE(PathSymbols)
  PROVIDE(PathModel)
END_DECLARE_MODULE(PathSymbols)

class PathSymbols: public PathSymbolsBase
{
public:

  PathSymbols() {}
  ~PathSymbols() {}

  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
};

#endif /* _PathSymbols_H_ */
